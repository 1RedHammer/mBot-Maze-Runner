#include <Arduino.h>
#include <MeMCore.h>

// Define motors
MeDCMotor motorLeft(M1);
MeDCMotor motorRight(M2);


MeUltrasonicSensor ultrasonicSensor(PORT_3);
// Define the built-in buzzer
MeBuzzer buzzer; // No port needed for the built-in buzzer


void setup() {
  // Nothing required
  Serial.begin(9600);
  buzzer.tone(262, 500); // Play a tone at 262 Hz (C4) for 500 ms
  delay(3000);
}


int speed = 120;
int maze[3][3]={
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8}
};

void turn(int angle){
  if (angle > 0) { // Turn right
    motorLeft.run(-speed);
    motorRight.run(-speed);
  } else { // Turn left
    motorLeft.run(speed);
    motorRight.run(speed);
  }
  const float turnDurationMultiplier = 6.75;
  //turn for a fixed duration according to the angle
  delay(abs(angle) * turnDurationMultiplier); // Adjust the multiplier for speed of turn
  motorLeft.run(0);
  motorRight.run(0);
  
}
void moveForward(float feet){
  float initialDistance = ultrasonicSensor.distanceInch();

  // Target distance to move forward (1 foot = 12 inches)
  float targetDistance = initialDistance - (12.0 * feet);

  // Move forward until the robot has moved 1 foot
  while (ultrasonicSensor.distanceInch() > targetDistance) {
    motorLeft.run(-speed);    // Move forward
    motorRight.run(speed);  
    delay(50);               // Small delay for stability
  }

  // Stop the motors after moving 1 foot
  motorLeft.run(0);
  motorRight.run(0);
}

void stop() {
  motorLeft.run(0);
  motorRight.run(0);
}

void checkCenter() {
  float bodylength = 6.5;
  turn(90);
  if(ultrasonicSensor.distanceInch() > 12){
    turn(-90);
    return;
  }
  while (ultrasonicSensor.distanceInch() < (12 - bodylength)/2) {
    motorLeft.run(speed);
    motorRight.run(-speed);
    delay(50);
  }
  while(ultrasonicSensor.distanceInch() > (12 - bodylength)/2) {
    motorLeft.run(-speed);
    motorRight.run(speed);
    delay(50);
  }
  turn(-90);
  stop();
}

void loop() {
    checkCenter();
    moveForward(1.0);
    checkCenter();
    turn(90);
    moveForward(2.0);
    checkCenter();
    turn(-90);
    moveForward(0.6);
    checkCenter();
    stop();
    buzzer.tone(500, 1000);

    //stop forever
    while (true) {
        delay(1000);
    }
}