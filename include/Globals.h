#ifndef GLOBALS_H
#define GLOBALS_H

#include <WallDetector.h>
#include <LineDetector.h>
#include <Driver.h>

class Globals {
public:
    static WallDetector wall;
    static LineDetector line;
    static Driver driver;
};

#endif
