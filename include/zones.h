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

void wallFollowing(int);
void distanceMeasuring();

#endif
