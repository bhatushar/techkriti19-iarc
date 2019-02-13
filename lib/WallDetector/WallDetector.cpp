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
    AVG_DIST = (MIN_DIST + MAX_DIST) / 2;

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
    
    /* 
     * Speed of sound in air, v = 346 m/s
     * Time taken, t = 0.5(t E-6) s
     * distance = v * t = 0.173t
     */
    mm = duration * 0.173;
}

// Detects deviatipon from wall
int WallDetector::detect(byte wall) {
    // Unknown wall index
    if (wall != LEFT && wall != RIGHT) return 0;

    sensors[wall].calcDistance(); // Distance from given wall
    sensors[FRONT].calcDistance(); // Distance from front wall; used in PID

    // Extreme point
    if (sensors[wall].mm >= MAX_DIST) return MAX_DIST;

    // Deviation from center line
    return sensors[wall].mm - AVG_DIST;
}

// Calculate voltage
// ! No measures if the bot reaches MIN_DIST from wall
int WallDetector::calcVolt(int err) {
    // A value is generated only if bot doesn't cross the average distance from front wall
    if (AVG_DIST < sensors[FRONT].mm) {
        /*
         * kP1: Constant of propotionality. Sets propotionality with err value.
         * kP2: Constant of propotionality. Sets propotionality with distance from front wall. 
         *      Responsible for slowing down bot as it approaches front wall.
         * kI: Constant of integration. Sets integral  relation with err value.
         * kD: Constant of differentiation. Sets differential relation with err value.
         */
        // TODO tune pid constants
        int kP1 = 0, kP2 = 0, kI = 0, kD = 0,
            P, D;
        
        /*
         * Relatinal variable with distance from front wall.
         * If the distance from front wall is greater  than MAX_DIST, it is set to zero (ignored).
         * Otherwise it set to (Average distance - Distance from front wall). Always negative.
         * Responsible for slowing down bot as it approaches front wall.
         */
        int x = (sensors[FRONT].mm > MAX_DIST) ? 0 : AVG_DIST - sensors[FRONT].mm;
        
        // Standard PID caluclations
        P = (kP1 * err) + (kP2 * x);1
        D = kD * (err - prevErr);
        errSum += err;
        prevErr = err;
        return abs(P + (kI * errSum) + D);
    } else return -1; // Wall on front, don't move
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
