#include <Arduino.h>
#include <Wire.h>
#include <Globals.h>
#include <zones.h>

/**
 * Invoked when a node is found.
 * Method finds the node type, prints the node details and drives the bot until the node is crossed.
 * The following data is printed:
 *  Node: <node count>
 *  Type: <node type>
 * 
 * @param nodeCount Current number of nodes found
 */
void printNode(int nodeCount) {
    String nodeType;
    // Move until center of node is reached
    do {
        Globals::driver.move(Driver::FORWARD, 0); // Move at base volt
        Globals::line.detect(); // Updates sensor data
    }while (!Globals::line.isNode());
    
    // Print count and type
    nodeType = Globals::line.nodeType();
    Globals::lcd.setCursor(6, 0);
    Globals::lcd.print(nodeCount);
    Globals::lcd.setCursor(6, 1);
    Globals::lcd.print(nodeType);

    // Move to cross rest of the node
    delay(2000); // Reach last rwo of the node
    do {
        Globals::driver.move(Driver::FORWARD, 0);
        Globals::line.detect();
    }while (!Globals::line.isNode()); // Move forward until node is crossed
}

// Section 1: Maze solving and node detection
short mazeSolving(short primaryTurn) {
    int err, volt;
    short nodeCount = 0, // Nodes encountered
        //primaryTurn = Driver::LEFT, // Hand to be on the wall
        wallSide = -1; // Wall index at the end of section
    String nodeType; // "TRUE" or "FALSE"

    // Initialize diplay
    Globals::lcd.setCursor(0,0);
    Globals::lcd.print("Node: ");
    Globals::lcd.setCursor(0,1);
    Globals::lcd.print("Type: ");

    do {
        // Get line data
        err = Globals::line.detect();
        volt = Globals::line.calcVolt(err);

        // TODO align axis of rotation before rotating
        
        // Bot deviating to left
        if (err < 0) {
            // Bot is at a 90 degree turn, rotate right
            if (Globals::line.is90Turn()) Globals::driver.move(Driver::RIGHT, volt, 90);
            // Not a hard turn
            else Globals::driver.move(Driver::RIGHT, volt, 90);
        } 
        // Bot deviating to right
        else if (err > 0) {
            // Bot is at a 90 degree turn, rotate left
            if (Globals::line.is90Turn()) Globals::driver.move(Driver::LEFT, volt, 90);
            // Not a hard turn
            else Globals::driver.move(Driver::LEFT, volt, 90);
        }
        /*
        No deviation, one of the following situations are possible:
            - Bot is on cross-section:
                - End of section is reached: Set wallSide and return from method
                - Normal cross-section: Turn to primanry side
            - Bot is on a 120 degree trisection: Turn to primary side
            - Bot is on a node: Cross the node, increase counter and display node type
        */
        else {
            // Cross-section
            if (Globals::line.isCrossSection()) {
                // At least one node is present
                if (nodeCount > 0) {
                    // Check for left wall
                    if (Globals::wall.hasWall(WallDetector::LEFT)) wallSide = WallDetector::LEFT;
                    // Check for right wall
                    else if (Globals::wall.hasWall(WallDetector::RIGHT)) wallSide = WallDetector::RIGHT;
                }
                // No wall, turn to primary side
                else Globals::driver.move(primaryTurn, volt, 90);
            }
            // 120 degree trisection
            else if (Globals::line.is120Junction()) Globals::driver.move(primaryTurn, volt, 60);
            // Node found
            else if (Globals::line.isNode()) {
                nodeCount++;
                printNode(nodeCount); 
            }
            // NOTA; keep moving forward
            else Globals::driver.move(Driver::FORWARD, volt);
        }
    } while (wallSide == -1); // Loop until wall is found

    // Clear display
    Globals::lcd.clear();

    return wallSide;
}

// Section 2: Wall following with wall switching
void wallFollowing(byte primary) {
    int err, volt;
    bool completed = false;
    do {
        err = Globals::wall.detect(primary);
        volt = Globals::wall.calcVolt(err);
        
        if (err == Globals::wall.MAX_DIST) {
            /*
             * No wall on primary side. It can be due to either one of the three conditions:
             *  - Wall switched: Wall present on the opposite side of primary
             *  - End of section reached: Determined by presence of line
             *  - Turn on the primary side: If the above two fails, rotate bot to primary side
             */
            Globals::driver.stop();

            // Check for wall on opposite side
            byte side = (primary == Globals::wall.LEFT) ? Globals::wall.RIGHT : Globals::wall.LEFT;
            if (Globals::wall.detect(side) <= Globals::wall.MAX_DIST) {
                // Wall on opposite side present
                // Switch primary wall
                primary = side;
            }
            // No wall on opposite side; check for line
            else if (Globals::line.detect() == 0 && Globals::line.isCrossSection()) {
                // Line found
                // Set completed flag
                completed = true; // Section complete
            }
            // No wall, no line; turn on primary side
            else {
                // Move forward to properly align after turning
                Globals::driver.move(Globals::driver.FORWARD, 0); // Move with base volt
                // TODO measure delay
                delay(500);
                
                byte turn = (primary == Globals::wall.LEFT) ? Globals::driver.RIGHT : Globals::driver.LEFT;
                Globals::driver.move(turn, 0, 90); // Rotate with base volt

                // Move to get wall on the side
                Globals::driver.move(Globals::driver.FORWARD, 0); // Move with base volt
                while (Globals::wall.detect(primary) >= Globals::wall.MAX_DIST) delay(200);
                Globals::driver.stop(); // Wall reached; contuinue in next iteration
            }
        } else if (volt == -1) {
            // Wall on front
            // Turn opposite to primary
            byte turn = (primary == Globals::wall.LEFT) ? Globals::driver.RIGHT : Globals::driver.LEFT;
            Globals::driver.move(turn, volt, 90); // Rotate by 90 degrees
        } else {
            // Move bot
            if (err < 0) {
                // Deviation to left
                // Move to right
                Globals::driver.move(Globals::driver.RIGHT, volt);
            } else if (err > 0) {
                // Deviation to right
                // Move to left
                Globals::driver.move(Globals::driver.LEFT, volt);
            } else  {
                // No deviation
                // Move forward
                Globals::driver.move(Globals::driver.FORWARD, volt);
            }
        }
    } while(!completed);
}

// Section 3: Measure distance between nodes
void distanceMeasuring() {
    int err, volt;
    // Number of nodes encountered
    short nodeCount = 0;
    
    // Do line following until two nodes are crosses
    // This part only contains a straight line to be followed; with two TRUE nodes in between
    do {
        // Line Following
        err = Globals::line.detect();
        volt = Globals::line.calcVolt(err);
        if (err < 0) Globals::driver.move(Driver::RIGHT, volt);
        else if (err > 0) Globals::driver.move(Driver::LEFT, volt);
        else Globals::driver.move(Driver::FORWARD, volt);
        
        // Check for node
        if (Globals::line.isNode()) {
            nodeCount++;
            // First node detected
            if (nodeCount == 1) {
                Globals::driver.stop(); // Move after inilizing encoder
                Globals::driver.initEncoder(); // Initialize encoder to calculate distance
            }
        }
    } while (nodeCount != 3);
    // Since the edge pattern of the node is matched, nodeCount will be 2 after crossing one node.
    // When bot reaches the second node, nodeCount will be 3.
    
    // Print distance
    Globals::driver.stop(); // Don't move or distance will be affected.
    Globals::lcd.setCursor(0, 0);
    Globals::lcd.print("Distance:");
    Globals::lcd.print(Globals::driver.getDistanceTravelled(), 2);
    Globals::lcd.print("cm");
    Globals::driver.stopEncoder(); // Stop encoder


    //Drive to finish line
    bool crossSection = false;
    do {
        // Line Following
        err = Globals::line.detect();
        volt = Globals::line.calcVolt(err);
        if (err < 0) Globals::driver.move(Driver::RIGHT, volt);
        else if (err > 0) Globals::driver.move(Driver::LEFT, volt);
        else {
            Globals::driver.move(Driver::FORWARD, volt);
            // Check for cross section here instead of inside while condition
            // Only possible when err = 0
            crossSection = Globals::line.isCrossSection();
        }
    } while (!crossSection);
    Globals::driver.stop();

    // Print finish
    Globals::lcd.setCursor(0,1);
    Globals::lcd.print("FINISH");

    // TODO  check if necessary
    delay (1000); // Safeguard measure
}
