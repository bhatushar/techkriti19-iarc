#include <Arduino.h>
#include <LineDetector.h>

// Constructor
LineDetector::LineDetector(int pins[]) {
    int i;
    
    // Assiging pins
    for (i = 0; i < MAX_SENSORS; i++) {
        sensors[i].pin = pins[i];
        pinMode(pins[i], INPUT);
    }

    // Assigning weights
    // For 8 sensors: -3 -2 -1 0 0 1 2 3
    // Checking if total sensors are even or odd
    bool even = !(MAX_SENSORS % 2);
    // For even number of sensors, weight start with one less than the half of sensor count
    // For odd number of sensors, weight starts with half of the sensor count
    int value = even ? -(MAX_SENSORS/2 - 1) : -(MAX_SENSORS/2) ;
    for(int i = 0; i < MAX_SENSORS; i++, value++) {
      if(even && i == MAX_SENSORS/2)
        // Additional condition for even sensors
        // Two sensors in middle should have weight = 0
        value--;
      sensors[i].weight = value; //Adding weight to each sensor
    }

    // Calculate MAX_ERROR
    MAX_ERROR = 0;
    for (int i = MAX_SENSORS/2; i < MAX_SENSORS; i++)
        MAX_ERROR += sensors[i].weight;

    errSum = 0;
    prevErr = 0;
}

// Destructor
LineDetector::~LineDetector() {}

// Calculate deviation
int LineDetector::detect() {
    int err = 0;
    for (int i = 0; i < MAX_SENSORS; i++) {
        sensors[i].value = digitalRead(sensors[i].pin);
        // Only add error is sensor is not on line
        if (sensors[i].value == HIGH)
            err += sensors[i].weight;
    }

    // Return net deviation
    return err;
}

// Calulate voltage
int LineDetector::calcVolt(int err) {
    // TODO tune PID constants
    int kP = 0, kI = 0, kD = 0;
    int P = kP * err, // Propotionality
        D = kD * (err - prevErr); // Differential
    errSum += err; // Integral
    prevErr = err; // Store err for future use
    int retval = P + (kI * errSum) + D;
    return (retval < 0) ? -retval : retval; // Return absolute value
}

// Checks for cross-section
bool LineDetector::isCrossSection() {
    int sensorsOnLine = 0;
    for (int i = 0; i < MAX_SENSORS; i++)
        if (sensors[i].value == false) sensorsOnLine++;
    
    if (sensorsOnLine == MAX_SENSORS)
        // All sensors are on the line
        return true;
    
    return false;
}

// Checks for a node
bool LineDetector::isNode() {
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (i == 2 || i == 5) {
            // Only sensors[2] and sensors[5] should be on the black line
            if (sensors[i].value == true)
                return false;
        }
        else if (sensors[i].value == false)
            return false;
    }

    return true;
}

// Checks for 120 degree junction
bool LineDetector::is120Junction() {
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (i == 3 || i == 4) {
            if (sensors[i].value == false)
                // Middle sensors are on line
                return false;
        }
        else if (sensors[i].value == true)
            // Other sensors are not on line
            return false;
    }

    return true;
}

// Check for 90 degree turns
bool LineDetector::is90Turn() {
    int i;
    bool isTurn = true;
    // Check if the first half is on line
    for (i = 0; i < MAX_SENSORS/2; i++)
        if (sensors[i].value == true) {
            isTurn = false;
            break;
        }
    if (!isTurn) {
        // First half is not on line
        // Check if second half is on line
        isTurn = true;
        for (; i < MAX_SENSORS; i++)
            if (sensors[i].value == true) {
                isTurn = false;
                break;
            }
    }

    return isTurn;
}

// Identify node type
String LineDetector::nodeType() {
    // If the center two sensors are on black
    if (sensors[3].value == LOW && sensors[4].value == LOW)
        // FALSE node
        return "FALSE";
    // All on white; TRUE node
    return "TRUE";
}
