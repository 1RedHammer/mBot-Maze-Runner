#ifndef WALL_DETECTING_RUNNER_H
#define WALL_DETECTING_RUNNER_H

#include "../include/MazeRunner.h"

class WallDetectingRunnger : public MazeRunner
{
public:
    WallDetectingRunnger(MeDCMotor &left, MeDCMotor &right, MeUltrasonicSensor &sensor, 
                        MeLineFollower &lineFollower, MeBuzzer &buz, MeRGBLed &rgbled)
        : MazeRunner(left, right, sensor, lineFollower, buz, rgbled) {}

    void runMaze() override;

private:
    static constexpr int MOTOR_SPEED = 200; // range from 0 to 255

    void turn(int angle);
    void moveForward(float feet);
    void stop();
    void checkCenter();
};

#endif