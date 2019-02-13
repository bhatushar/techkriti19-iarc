#include <Arduino.h>
#include <Globals.h>
#include <zones.h>

// Initialize global objects
byte usonic_pins[3][2] = {{0,0}, {0,0}, {0,0}};
uint16_t dist_range[2] = {0, 0};
WallDetector Globals::wall = WallDetector(usonic_pins, dist_range);

byte ir_pins[8] = {0,0,0,0,0,0,0,0};
LineDetector Globals::line = LineDetector(ir_pins);

byte motor_pins[2][2] = {{0,0}, {0,0}};
Driver Globals::driver = Driver(motor_pins, (byte) 100);

LiquidCrystal_I2C Globals::lcd = LiquidCrystal_I2C(0x27, 16, 2);

void setup() {
  byte primary = mazeSolving(Driver::LEFT);
  wallFollowing(primary);
  distanceMeasuring();
}

void loop() {
  // put your main code here, to run repeatedly:
}
