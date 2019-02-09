#include "Arduino.h"
#include "WallDetector.h"

// Constructor
WallDetector::WallDetector(int pins[][2], int thresh[]) {
    for (int i = 0; i < 3; i++) {
        sensors[i].trig = pins[i][0];
        sensors[i].echo = pins[i][1];
        pinMode(sensors[i].trig, OUTPUT);
        pinMode(sensors[i].echo, INPUT);
    }
    
    MIN_DIST = thresh[0];
    MAX_DIST = thresh[1];

    errSum = 0;
    prevErr = 0;
}

// Destructor
WallDetector::~WallDetector() {}

// Calculates distace from sensor
void WallDetector::UltrasonicSensor::calcDistance() {
    // The time it takes for the pulse to hit the wall and come back
    unsigned long duration;
    
    // Throw a pulse for 10 microseconds
    digitalWrite(trig, LOW);
    delayMicroseconds(5);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    // Calculate distance in cm
    duration = pulseIn(echo, HIGH);
    
    /* Speed of sound in air, v = 346 m/s
     * Time taken, t = 0.5(t E-6) s
     * distance = v * t = 0.173t
     */
    mm = duration * 0.173;
}

// Detects deviatipon from wall
int WallDetector::detect(short wall) {
    // Unknown wall index
    if (wall > 2 || wall < 0) return 0;
 
    int prevDist = sensors[wall].mm;
    sensors[wall].calcDistance();

    // Extreme points
    if (sensors[wall].mm <= MIN_DIST) throw MIN_DIST;
    else if (sensors[wall].mm >= MAX_DIST) throw MAX_DIST;

    // Deviation from last time
    return sensors[wall].mm - prevDist;
}

// Calculate voltage
int WallDetector::calcVolt(int err) {
    // TODO tune pid constants
    int kP = 0, kI = 0, kD = 0;
    int P = kP * err,
        D = kD * (err - prevErr);
    errSum += err;
    prevErr = err;
    int retval = P + (kI * errSum) + D;
    return (retval < 0) ? -retval : retval;
}

// Check for wall
bool WallDetector::hasWall(short wall) {
    // Unkown wall index
    if (wall > 2 || wall < 0) return false;
    
    sensors[wall].calcDistance();
    // Wall within range
    if (sensors[wall].mm <= MAX_DIST) return true;
    // No wall
    else return false;
}
