#ifndef LINE_FOLLOWING_RUNNER_H
#define LINE_FOLLOWING_RUNNER_H

#include "../include/MazeRunner.h"

class LineFollowingRunner : public MazeRunner
{
public:
    LineFollowingRunner(MeDCMotor &left, MeDCMotor &right, MeUltrasonicSensor &sensor,
                        MeLineFollower &lineFollower, MeBuzzer &buz, MeRGBLed &rgbled)
        : MazeRunner(left, right, sensor, lineFollower, buz, rgbled) {}

    void runMaze() override;    

protected:
private:
};

#endif
