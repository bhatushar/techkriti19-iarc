#ifndef ZONES_H
#define ZONES_H

/**
 * Procedure to operate bot in section A-B. The zone includes:
 *  - Line following
 *  - Maze solving
 *  - Node detection
 *  - Node counting
 *  - Wall detection at end
 * Line following is done by using standard weight assignment and PID calculation.
 * Maze solving uses "X hand on the wall" algorithm. X can either be left or right.
 * 
 * The general approach is to follow steps for solving maze, using principle of line following.
 * Whenever a node is detected increase counter and determine it's time. It should be done independent of maze solving.
 * End of section is reached when bot is on a cross-section and a wall is found on any one side (left or right).
 * 
 * @param primarySide Side to be followed in the maze
 * @return Index of the wall at the end.
 */
short mazeSolving(short);

/**
 * Procedure to operate bot in section B-C and C-D. The zone includes:
 *  - Wall following
 *  - Wall switching
 *  - Front wall detection (Turn on opposite to primary side)
 *  - Turn on primary side
 * Wall is followed while maintaing an average distance, which acts as the center line between min and max distance.
 *  
 * @param primary Initial side on which wall is present.
 */
void wallFollowing(byte);

/**
 * Procedure to operate bot in section D-E. The zone includes:
 *  - Straight line following
 *  - Node detection
 *  - Measuring distance between two nodes
 * When the first node is detected, the encoder is initialised. It is stopped when the second node is detected.
 * The encoding is done using a secondary controller which acts as slave. Distance is requested once second node is detected.
 */
void distanceMeasuring();

#endif
