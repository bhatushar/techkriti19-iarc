#ifndef LINE_DETECTOR_H
#define LINE_DETECTOR_H

/**
 * Line detector library provides line following and maze solving helpers.
 * The class interacts with the IR sensor array.
 */
class LineDetector{
private:
    // Maximum number of sensors available
    const static short MAX_SENSORS = 8;

    /**
     * The bot currently contains a line sensor array with 8 IR sensors.
     * However, the number of sensors are stored in a variable and not used directly in methods.
     * Unline ultrasonic sensors and motors, change in number of IR sensors doesn't affect overall working of the bot.
     * Each sensor has an assigned pin and a weight. It also stores the last read value.
     */
    struct IRSensor {
        int pin,
            weight;
        bool value; // Values are digitally read, so it can be stored in boolean type.
    } sensors[MAX_SENSORS];
    
    int errSum, // Sum of all caluclated errors; Used in PID
        prevErr; // Stores last recorded error
public:
    // Maximum error that can be calculated by the sensor. 
    int MAX_ERROR;

    /**
     * Constructor
     * Initializes the sensors[] array.
     * The weight for the pins is caluclated based on the number of sensors.
     * It also initializes errSum and prevErr to zero.
     * Calculted MAX_ERROR
     * 
     * @param pins Sensor pins in left to right sequence
     */
    LineDetector(int[]);
    
    // Destructor
    ~LineDetector();
    
    /**
     * Calculates and returns the devaition of the bot from the line.
     * It uses the weights of the sensors and adds all the weights of the sensors which are off the line.
     * If returned value is negative, bot is deviating to the left.
     * If returned value is positive, bot is deviating to the right.
     * If returned value is zero, bot is moving straight.
     * 
     * @return The net deviation.
     */
    int detect();

    /**
     * Calculates the voltage to be applied to the motors, using the error value.
     * The error value must be calculated using the LineDetector::detect() method.
     * 
     * @param err The deviation of the bot
     * @return Volage to be applied
     */
    int calcVolt(int);

    /**
     * Checks whether the bot is on a cross-section or not.
     * It does that by simply checking if all sensors are on the line.
     * The LineDetector::detect() method must be invoked before calling this method since it uses the value read by the sensors.
     * 
     * @return Cross-section status
     */
    bool isCrossSection();

    /**
     * Checks for the beginning of a node.
     * Nodes are 6x6 cm in dimension, and the beginning of every node is marked by an OFF-ON-OFF zone.
     * Assuming the bot is perfectly aligned, the sensors will follows this pattern
     * Sensor     |  1 | 2  | 3   | 4  | 5  | 6   | 7  | 8
     * -----------|----|----|-----|----|----|-----|----|---
     * Value read | ON | ON | OFF | ON | ON | OFF | ON | ON
     * The sensors on the extreme must read DIGITAL HIGH, this seperates node from a 120 degree junction.
     * The LineDetector::detect() method must be invoked before calling this method since it uses the value read by the sensors.
     * 
     * @return Node status
     */
    bool isNode();

    /**
     * Determines the type of node based on it's middle section.
     * In a false node, the two center nodes lie on black surface.
     * In a true node, all nodes on are white surface.
     * The LineDetector::detect() method must be invoked before calling this method since it uses the value read by the sensors.
     * 
     * @return "TRUE" or "FALSE"
     */
    String nodeType();

    /**
     * Checks if the bot is at a 120 degree junction.
     * Here, the line spilts into two lines, each making 120 degrees with the other. The lines are 3cm wide.
     * A junction will be detected if the two sensors in the center read DIGITAL HIGH while all other sensors read DIGITAL LOW.
     * The LineDetector::detect() method must be invoked before calling this method since it uses the value read by the sensors.
     * 
     * @return Junciton status
     */
    bool is120Junction();

    /**
     * Checks whether the bot is on a 90 degree turn.
     * A 90 degree turn must verify two conditions:
     *  - Line sensor must record maximum error (checked externally)
     *  - Half of the sensors must read zero value
     * The LineDetector::detect() method must be invoked before calling this method since it uses the value read by the sensors.
     * 
     * @return Turn status
     */
    bool is90Turn();
};

#endif
