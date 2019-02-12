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
    return wallSide;
}
