#include "../../include/LineFollowingRunner.h"

static constexpr float MIN_OBSTACLE_INCHES = 2; // Distance in inches to detect an obstacle
static constexpr int LOW_SPEED = 30;            // Low speed for turning
static constexpr int HIGH_SPEED = 70;           // High speed for moving forward
static constexpr int TURN_SPEED = 90;           // Speed for turning
static constexpr int MIN_TIME_BETWEEN_INTERSECTIONS = 3000; 

void LineFollowingRunner::turn(TurnDirection direction)
{
    // turn in-place
    motorRight.run(-TURN_SPEED * direction);
    motorLeft.run(-TURN_SPEED * direction);
    delay(500); // T=turn for a while to get close to the line

    // find the line
    while (lineFollower.readSensors() != S1_IN_S2_IN)
    {
        motorRight.run(-TURN_SPEED * direction);
        motorLeft.run(-TURN_SPEED * direction);
        delay(10); // Allow time for the turn adjustment to take effect
    }

    // let it turn just a bit more to get the car straight
    delay(20);

    // Stop after turn
    motorRight.stop();
    motorLeft.stop();
    delay(200);
}
void LineFollowingRunner::turnLeft()
{
    turn(LEFT);
}

void LineFollowingRunner::turnRight()
{
    turn(RIGHT);
}
void LineFollowingRunner::moveForward()
{
    motorRight.run(HIGH_SPEED);
    motorLeft.run(-HIGH_SPEED);    
}

void LineFollowingRunner::moveBackward() {
    motorRight.run(-HIGH_SPEED);
    motorLeft.run(HIGH_SPEED);
}
void LineFollowingRunner::uTurn() {
    
}

void LineFollowingRunner::runMaze()
{
    long lastIntersectionTime = 0;

    while (true)
    {

        // TODO - Check for obstacles


        // Read line sensor values
        int sensorState = lineFollower.readSensors();
        switch (sensorState)
        {

        case S1_OUT_S2_OUT: // No black detected at either sensor, are we at an intersection?

            // Check if enough time has passed since the last intersection
            if (millis() - lastIntersectionTime < MIN_TIME_BETWEEN_INTERSECTIONS)
            {
                // not a real intersection
                // shouldn't we back up a bit?
                break;
            }
            
            // Yes, we are at an intersection. update the last intersection time
            lastIntersectionTime = millis();

            // Move forward to align wheels with the center of intersection
            delay(600);
            motorRight.stop();
            motorLeft.stop();

            buzzer.tone(1000, 100); // Beep to indicate intersection found
            delay(50);              

            // Use mazeSolver to update the status of the maze and decide the next move
            MazeSolver::Action action = mazeSolver.processIntersection(MazeSolver::State::STATE_VISITED);

            switch (action)
            {
            case MazeSolver::MOVE_FORWARD:
                moveForward();
                break;
            case MazeSolver::TURN_LEFT:
                turnLeft();
                break;
            case MazeSolver::TURN_RIGHT:
                turnRight();
                break;
            case MazeSolver::U_TURN:
                uTurn();
                break;
            case MazeSolver::MOVE_BACKWARD:
                break;
            default:
                break;
            }
            break;

        case S1_OUT_S2_IN: // Black on right sensor only - steer right

            motorRight.run(LOW_SPEED);
            motorLeft.run(-HIGH_SPEED);
            break;

        case S1_IN_S2_OUT: // Black on left sensor only - steer left

            motorRight.run(HIGH_SPEED);
            motorLeft.run(-LOW_SPEED);
            break;

        case S1_IN_S2_IN: // Black under both sensors, move forward
            
            motorRight.run(HIGH_SPEED);
            motorLeft.run(-HIGH_SPEED);
            break;
        }

        delay(200);
    }
}