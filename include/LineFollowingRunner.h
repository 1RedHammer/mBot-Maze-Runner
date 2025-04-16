#ifndef LINE_FOLLOWING_RUNNER_H
#define LINE_FOLLOWING_RUNNER_H

#include "../include/MazeRunner.h"
#include "MazeSolver.h"

class LineFollowingRunner : public MazeRunner
{
public:
    LineFollowingRunner(MeDCMotor &left, MeDCMotor &right, MeUltrasonicSensor &sensor,
                        MeLineFollower &lineFollower, MeBuzzer &buz, MeRGBLed &rgbled)
        : MazeRunner(left, right, sensor, lineFollower, buz, rgbled), mazeSolver() {}

    void runMaze() override;    

private:
    MazeSolver mazeSolver;
};

#endif
