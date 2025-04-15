#ifndef MAZE_RUNNER_H
#define MAZE_RUNNER_H

#include <MeMCore.h>

class MazeRunner {
protected:
    MeDCMotor& motorLeft;
    MeDCMotor& motorRight;
    MeUltrasonicSensor& ultrasonicSensor;
    MeLineFollower& lineFollower;
    MeBuzzer& buzzer;
public:
    MazeRunner(MeDCMotor& left, MeDCMotor& right, MeUltrasonicSensor& sensor, MeLineFollower& lineFollower,  MeBuzzer& buz)
        : motorLeft(left), motorRight(right), ultrasonicSensor(sensor), lineFollower(lineFollower), buzzer(buz) {}
    
    virtual ~MazeRunner() = default;
    virtual void runMaze() = 0;
    
protected:
    void stop() {
        motorLeft.run(0);
        motorRight.run(0);
    }
  
};
#endif