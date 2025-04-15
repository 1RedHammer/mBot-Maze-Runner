#ifndef WALL_DETECTING_RUNNER_H
#define WALL_DETECTING_RUNNER_H

#include "../include/MazeRunner.h"

class WallDetectingRunnger : public MazeRunner
{
public:
    WallDetectingRunnger(MeDCMotor &left, MeDCMotor &right, MeUltrasonicSensor &sensor, MeLineFollower &lineFollower, MeBuzzer &buz)
        : MazeRunner(left, right, sensor, lineFollower, buz) {}

    void runMaze() override;

protected:
    

private:
    static constexpr int MOTOR_SPEED = 200; // range from 0 to 255

    void turn(int angle);
    void moveForward(float feet);
    void stop();
    void checkCenter();

};
#endif