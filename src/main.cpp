#include <Arduino.h>
#include <MeMCore.h>

// Define motors
MeDCMotor motorLeft(M1);
MeDCMotor motorRight(M2);

void setup() {
  // Nothing required
}

int speed = 150;

void loop() {
  // Spin both motors forward at 100% speed
  motorLeft.run(speed);    // range: -255 to 255
  motorRight.run(speed);   // negative = reverse

  delay(2000);           // run for 2 seconds

  // Stop the motors
  motorLeft.run(0);
  motorRight.run(0);

  delay(2000);           // wait 2 seconds

  speed = -speed;
}
