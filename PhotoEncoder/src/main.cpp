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

// IR pins
const byte ir_out = 3, ir_vcc = 5, ir_gnd = 4;

// Values read by the sensor
byte oldVal, newVal;

// Ecoder signal
// Determines whether to start the encoder or not.
bool startEncoder = false;

/**
 * Variable stores the number of times encoder was triggered
 * unsigned - only counts in non-negative
 * long - possibly a large value
 */
unsigned long ticks;

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
  if (code == 1) {
    // Initialize encoder
    ticks = 0;
    startEncoder = true;
    // Turn on the IR sensor
    digitalWrite(ir_vcc, HIGH);
    // Read initial value
    oldVal = digitalRead(ir_out);
  } else if (code == 0) {
    // Stop encoder
    startEncoder = false;
    // Turn of IR sensor
    digitalWrite(ir_vcc, LOW);
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
    tickRate = 8; // Number of ticks per rotation

  // Rotations made by the wheel
  float rotations = ticks / tickRate; 
  dist.value = (pi * diameter) * rotations; // Circumference * Number of rotations
  Wire.write(dist.bytes);
}

// Starting point
void setup() {
  // Set IR pin modes
  pinMode(ir_vcc, OUTPUT);
  pinMode(ir_gnd, OUTPUT);
  pinMode(ir_out, INPUT);

  // Always at LOW
  // Only ir_vcc state varies
  digitalWrite(ir_gnd, LOW);

  // Join I2C bus with address #8
  Wire.begin(8);

  // Register events
  Wire.onReceive(receiveEvent);
  Wire.onRequest(calcDistace);
}

void loop() {
  // Check whether to start the encoder or not
  if (startEncoder) {
    newVal = digitalRead(ir_out); // Read sensor data
    
    // Change in state
    if (newVal != val) {
      ticks++; // Update tick count
      oldVal = newVal; // Store value
    }
  }
}
