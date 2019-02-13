#include <Arduino.h>
#include <Wire.h>

/**
 * Structure stores the distance travelled by the bot.
 * Using a union since Wire library doesn't allow writing float values.
 * The float value is calculated, and the equivalent character bytes are sent to Master.
 */
union Distance {
  float value;
  char bytes[sizeof(float)];
};

/**
 * Variable stores the number of times encoder was triggered
 * volatile - susceptible to change
 * register - for fater loading
 * unsigned - only counts in non-negative
 * long - possibly a large value
 */
volatile unsigned long ticks;

// ISR; increments ticks by 1 on each invokation
void countTicks() { ticks++; }

/**
 * Invoked when Slave recives instruction from Master.
 * Based on the code recieved, this method either starts or stops the encoder.
 * Encoder starts when code is 1, and stops when code is 0.
 * The code must be an integer value.
 * 
 * @param numBytes Number of bytes read from the master
 */
void receiveEvent(int numBytes) {
  byte code = Wire.read(); // Read code
  // TODO set encoder pin
  if (code == 1) {
    // Initialize encoder
    ticks = 0;
    // Only one way interrupt is necessary
    // countTicks is invoked when value of pin changes from LOW to HIGH
    attachInterrupt(digitalPinToInterrupt(0), countTicks, RISING);
  } else if (code == 0) {
    // Stop encoder
    detachInterrupt(digitalPinToInterrupt(0));
  }
}

/**
 * Method calculates distance travelled and sends the data to Master.
 * Invoked when Master requests data from Slave.
 * Distance is calculated using the number of ticks recorded.
 */
void calcDistace() {
  Distance dist;
  float pi = 3.14,
    diameter = 7, // Diameter of wheel
    // TODO find tickRate
    tickRate = 0; // Ticks in 1 rotation

  // Rotations made by the wheel
  float rotations = ticks / tickRate; 
  dist.value = (pi * diameter) * rotations; // Circumference * Number of rotations
  Wire.write(dist.bytes);
}

// Starting point
void setup() {
  // Join I2C bus with address #8
  Wire.begin(8);

  // Register events
  Wire.onReceive(receiveEvent);
  Wire.onRequest(calcDistace);
}

void loop() {}
