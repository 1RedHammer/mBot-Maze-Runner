#ifndef LINE_FOLLOWING_RUNNER_H
#define LINE_FOLLOWING_RUNNER_H

#include "MazeRunner.h"
#include "MazeSolver.h"
#include "SoundPlayer.h"

class LineFollowingRunner : public MazeRunner
{
public:
    LineFollowingRunner(MeDCMotor &left, MeDCMotor &right, MeUltrasonicSensor &sensor,
                        MeLineFollower &lineFollower, MeBuzzer &buz, MeRGBLed &rgbled)
        : 
        MazeRunner(left, right, sensor, lineFollower, buz, rgbled),
        // Initialize the maze solver with start and target positions and direction
        mazeSolver(0, 0, GlobalConstants::MAZE_WIDTH - 1, GlobalConstants::MAZE_HEIGHT - 1, MazeSolver::Direction::NORTH),
        soundPlayer(buzzer) 
        {

        }


    void runMaze() override;    

    enum TurnDirection {
        LEFT = -1,
        RIGHT = 1,
    };

private:
    MazeSolver mazeSolver;
    SoundPlayer soundPlayer;

    void turn(TurnDirection direction, int angle);
    void turnLeft();
    void turnRight();
    void moveForward();
    void moveBackward();
    void uTurn();
    void act(MazeSolver::Action action);
};

#endif
