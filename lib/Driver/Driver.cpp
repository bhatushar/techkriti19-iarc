#include <Arduino.h>
#include <Driver.h>

// Contructor
Driver::Driver(int mPins[][2], int base, int enPins[]) {
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

    // Assigning endoer pin
    // Pin mode is set in initEncoder() method
    encoderA = enPins[0];
    encoderB = enPins[1];
}

// Destructor
Driver::~Driver() {}

// Writes signal to motor
void Driver::Motor::apply(int v1, int v2) {
    analogWrite(positive, v1);
    analogWrite(negative, v2);
}

// Drive bot in desired direction
void Driver::move(int direction, int volt, int rotate) {
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

void Driver::initEncoder() {
    pinMode(encoderA, INPUT);
    pinMode(encoderB, INPUT);
    attachInterrupt(digitalPinToInterrupt(encoderA), countTicks, CHANGE);
}

void Driver::countTicks() {
    // TODO check whether using both interrupt pins gives a more accurate result
    ticks++;
}

float Driver::getDistanceTravelled() {
    //int distance, radius = 2.1, pi = 3.14;
    // todo implement rest of it, Im so lazy right now
    return 0.0;//distance;
}
