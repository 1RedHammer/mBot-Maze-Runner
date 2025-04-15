#include <Arduino.h>
#include <MeMCore.h>
#include "..\include\WallDetectingRunner.h"
#include "..\include\LineFollowingRunner.h"


// Define hardware components
MeDCMotor motorLeft(M1);
MeDCMotor motorRight(M2);
MeUltrasonicSensor ultrasonicSensor(PORT_3);
MeLineFollower lineFollower(PORT_4);

MeBuzzer buzzer;



//this code was moved from MeMCore.h to avoid compilation errors
MePort_Sig mePort[17] =
{
  { NC, NC }, { 11, 12 }, {  9, 10 }, { A2, A3 }, { A0, A1 },
  { NC, NC }, {  8, A6 }, { A7, 13 }, {  8, A6 }, {  6,  7 },
  {  5,  4 }, { NC, NC }, { NC, NC }, { NC, NC }, { NC, NC },
  { NC, NC },{ NC, NC },
};



// Create runner instance
MazeRunner* runner;


void setup() {
    Serial.begin(9600);
    buzzer.tone(262, 500);
    delay(3000);
    
    // Initialize the runner
    //runner = new WallDetectingRunnger(motorLeft, motorRight, ultrasonicSensor, buzzer);
    runner = new LineFollowingRunner(motorLeft, motorRight, ultrasonicSensor, lineFollower, buzzer);
    
}

void loop() {
    runner->runMaze();
    
    // Stop forever after completing the maze
    while (true) {
        delay(1000);
    }
}