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

    static constexpr int MOTOR_SPEED = 90;     // range from 0 to 255
    static constexpr int OBSTACLE_DISTANCE = 2; // Distance in inches to detect an obstacle

protected:
private:
};

#endif
