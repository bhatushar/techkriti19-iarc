#include <Arduino.h>
#include <Driver.h>
#include <Wire.h>

// Contructor
Driver::Driver(byte mPins[][2], byte base) {
    // Assigning pins
    mLeft.positive = mPins[0][0];
    mLeft.negative = mPins[0][1];
    mRight.positive = mPins[1][0];
    mRight.negative = mPins[1][1];
    // Setting pin mode
    pinMode(mLeft.positive, OUTPUT);
    pinMode(mLeft.negative, OUTPUT);
    pinMode(mRight.positive, OUTPUT);
    pinMode(mRight.negative, OUTPUT);

    // Setting base voltage
    baseVolt = base;

    // join I2C bus (address optional for master)
    Wire.begin();
}

// Destructor
Driver::~Driver() {}

// Writes signal to motor
void Driver::Motor::apply(byte v1, byte v2) {
    analogWrite(positive, v1);
    analogWrite(negative, v2);
}

// Drive bot in desired direction
void Driver::move(byte direction, byte volt, byte rotate) {
    // Constant of rotation
    // Calculated using given volt
    // Multiplied with the rotation angle to get delay time
    int kR = 0;
    
    if (rotate) stop(); // Stop before rotating

    switch(direction) {
        case FORWARD:
            // Rotate left and right motors in the same direction
            mLeft.apply(baseVolt + volt, 0);
            mRight.apply(baseVolt + volt, 0);
            break;
        case BACKWARD:
            // Rotate motors in same direction, but in reverse
            mLeft.apply(0, baseVolt + volt);
            mRight.apply(0, baseVolt + volt);
            break;
        case LEFT:
            if (rotate) {
                // Keep left wheel fixed, rotate right wheel
                mRight.apply(baseVolt + volt, 0);
                delay(kR * rotate);
                mRight.apply(0, 0); // Stop right wheel
            } else {
                // Don't rotate, but slide
                // Rotate left motor slower than right motor
                mLeft.apply(baseVolt, 0);
                mRight.apply(baseVolt + volt, 0);
            }
            break;
        case RIGHT:
            if (rotate) {
                // Keep right wheel fixed, rotate left wheel
                mLeft.apply(baseVolt + volt, 0);
                delay(kR * rotate);
                mLeft.apply(0, 0); // Stop left wheel
            } else {
                // Don't rotate, but slide
                // Rotate left motor faster than right motor
                mLeft.apply(baseVolt + volt, 0);
                mRight.apply(baseVolt, 0);
            }
            break;
    }
}

// Stop motors
void Driver::stop() {
    mLeft.apply(0, 0);
    mRight.apply(0, 0);
}

// Start encoding
void Driver::initEncoder() {
    Wire.beginTransmission(8);
    Wire.write(1); // Start encoder
    Wire.endTransmission();
}

// Stop encoding
void Driver::stopEncoder() {
    Wire.beginTransmission(8);
    Wire.write(0); // Stop encoder
    Wire.endTransmission();
}

// Return distance travelled
float Driver::getDistanceTravelled() {
    short size = sizeof(float);
    union Distance {
        float value;
        char bytes[sizeof(float)];
    } dist;
    
    Wire.requestFrom(8, size);
    Wire.readBytes(dist.bytes, size); // Read bytes
    return dist.value; // Return equivalent float value
}
