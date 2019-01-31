#ifndef DRIVER_H
#define DRIVER_H

class Driver {
private:
    /**
     * The robot has two motors, the left motor and the right motor.
     * Each motor has two pins, a positive pin and a negative pin.
     */
    struct Motor {
        // Motor terminals
        int positive, negative;
        /**
         * Method writes digital signal to the positive and negative pins of the motor, respectively.
         * 
         * @param v1 Value to be written at positive terminal
         * @param v2 Value to be written at negative terminal
         */
        void apply(int, int);
    } mLeft, mRight; // Left and right motors.

    // Minimum voltage to be applied to the motors
    int baseVolt;
    
    // Encoder outputs
    int encoderA, encoderB;
    // Number of ticks made by encoder.
    volatile static int ticks;

    /**
     * Increment ticks everytime the encoder rotates
     */
    static void countTicks();

public:
    // Directional constants
    const static short LEFT = 0, FORWARD = 1, RIGHT = 2, BACKWARD = 3;

    /**
     * Constructor
     * Initializes motor pins and A/B output pins of encoder.
     * Also sets pin mode of motor pins.
     * 
     * @param mPins[][2] (positive, negative) pin pairs of left and right motor respectively.
     * @param base Minimum voltage to be applied
     * @param enPins[] Encoder output A and B pin respectively.
     */    
    Driver(int[][2], int, int[]);

    // Destructor
    ~Driver();

    /**
     * Drives the bot in desired direction by applying the given voltage to the respective motors.
     * It can also rotate the bot at the same position.
     * 
     * @param direction One of the FORWARD, LEFT, RIGHT or BACKWARD direction
     * @param v Voltage to be applied
     * @param rotate Whether the bot should be rotated or not (default = false)
     */
    void move(int, int, bool = false);
    
    /**
     * Stops all the motors by writing 0 on all pins.
     */
    void stop();
    
    /**
     * Attaches interrupt to encoder pins
     */
    void initEncoder();
    
    /**
     * Calculate the distance travelled using the number of ticks
     */
    float getDistanceTravelled();
};

#endif
