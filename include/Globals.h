#ifndef GLOBALS_H
#define GLOBALS_H

#include <WallDetector.h>
#include <LineDetector.h>
#include <Driver.h>
#include <LiquidCrystal_I2C.h>

class Globals {
public:
    static WallDetector wall;
    static LineDetector line;
    static Driver driver;
    static LiquidCrystal_I2C lcd;
};

#endif
