#ifndef DRIVER_H
#define DRIVER_H

/**
 * The library is repsosible for movement of the bot. 
 * It interacts with the motors and the rotary encoder. It uses them to move the bot in desired direction and calculate the distance travelled.
 */
class Driver {
private:
    /**
     * The robot has two motors, the left motor and the right motor.
     * Each motor has two pins, a positive pin and a negative pin.
     */
    struct Motor {
        // Motor terminals
        byte positive, negative;
        /**
         * Method writes digital signal to the positive and negative pins of the motor, respectively.
         * 
         * @param v1 Value to be written at positive terminal
         * @param v2 Value to be written at negative terminal
         */
        void apply(byte, byte);
    } mLeft, mRight; // Left and right motors.

    // Minimum voltage to be applied to the motors
    byte baseVolt;

public:
    // Directional constants
    const static byte LEFT = 0, FORWARD = 1, RIGHT = 2, BACKWARD = 3;

    /**
     * Constructor
     * Initializes motor pins and A/B output pins of encoder.
     * Also sets pin mode of motor pins.
     * 
     * @param mPins[][2] (positive, negative) pin pairs of left and right motor respectively.
     * @param base Minimum voltage to be applied
     */    
    Driver(byte[][2], byte);

    // Destructor
    ~Driver();

    /**
     * Drives the bot in desired direction by applying the given voltage to the respective motors.
     * It can also rotate the bot. To rotate, an angle in degree is passed.
     * Since, it is a three-wheel setup, bot is rotated while keeping one wheel fixed and rotating the other.
     * 
     * @param direction One of the FORWARD, LEFT, RIGHT or BACKWARD direction
     * @param v Voltage to be applied
     * @param rotate Rotation angle in degree. Range: 0 to 180 (default = 0)
     */
    void move(byte, byte, byte = 0);
    
    /**
     * Stops all the motors by writing 0 on all pins.
     */
    void stop();
    
    /**
     * Writes signal to Slave to start encoder.
     */
    void initEncoder();

    /**
     * Writes signal to Slave to stop encoder.
     */
    void stopEncoder();
    
    /**
     * Requests distance from the Slave.
     * 
     * @return Distance Travelled
     */
    float getDistanceTravelled();
};

#endif
