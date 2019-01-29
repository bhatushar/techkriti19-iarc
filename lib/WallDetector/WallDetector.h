#ifndef WALL_DETECTOR_H
#define WALL_DETECTOR_H

/**
 * WallDetector class conatins methods and sttributes provide wall following functionality.
 * The class interacts with the ultrasonic sensors connected to the robot.
 */
class WallDetector {
private:
    /**
     * The robot conatins 3 ultrasonic sensors which are used during the wall following section.
     * Each sensor is assiciated with a trigger pin and an echo pin.
     * Along with the pins, the sensor also stores the last measured distance (in mm).
     * It also contains a method to calculate distance.
     */
    struct UltrasonicSensor {
        int trig, echo; // Trigger and echo pin
        int mm; // Distance measured by the sensor
        void calcDistance(); //Calculates distance of the wall from the given sensor and stores that distance in mm attribute.
    } sensors[3]; // Left, front and right sensor

    int errSum,  // Sum of all the errors; Used in PID
        prevErr; // Stores the last error calculated

public:
    // Wall indices
    const static int LEFT = 0, FRONT = 1, RIGHT = 2;
    // Minimum and maximum distance allowed from the wall
    int MIN_DIST, MAX_DIST;
    
    /**
     * Contructor
     * Initializes senor pins and sets the mode for the respective pins.
     * Also sets the minimum and maximum threshold.
     * 
     * @param pins (trig, echo) pin pairs for left, front and right ultrasonic sensors respectively
     * @param thresh Minimum and maximum threshold, i.e., allowed distance from the wall
     */
    WallDetector(int[][2], int[]);

    /**
     * Method calculates deviation from the wall.
     * It invokes UltrasonicSensor::calcDistance() to measuree distance of the wall from the sensor.
     * The distance is compared with previously calculated distance, and the deviation is calculeted.
     * 
     * @param wall Wall index; LEFT, FRONT or RIGHT
     * @return Returns deviation if the distance is within threshold, otherwise the threshold value is returned.
     */
    int detect(int);

    /**
     * Calculates the analog voltage value which will be passed to the motors.
     * The value is generated the deviation, which must be caluclated using WallDetector::detect() method.
     * Valtage signal is calculeted as follows:
     *  - Prodotionality factor is kP times the error
     *  - Differential factor is kD times the substraction of previous error from the current error.
     *  - Integral factor is kI times the sum of all the previous errors.
     * The method also stores the error value in prevError.
     * 
     * @param err Devaition of the bot with respect to the wall
     * @return Voltage to be applied to the motors
     */
    int calcVolt(int);

    // Destructor
    ~WallDetector();
};

#endif
