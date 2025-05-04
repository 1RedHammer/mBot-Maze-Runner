#include "../../include/LineFollowingRunner.h"
#include "../../include/SoundPlayer.h"

static constexpr float MIN_OBSTACLE_INCHES = 1.8; // Distance in inches to detect an obstacle
static constexpr int LOW_SPEED = 35;            // Low speed for steering
static constexpr int HIGH_SPEED = 70;           // High speed for moving forward
static constexpr int TURN_SPEED = 75;           // Speed for turning in place
static constexpr int MIN_TIME_BETWEEN_INTERSECTIONS = 2700;
static constexpr int TIME_TO_CENTER_INTERSECTION = 840; // Time to center the robot in the intersection
static constexpr float TURN_ANGLE_TO_TIME_FACTOR = 3.9; // Factor to convert turn angle to time in milliseconds
static constexpr int TIME_TO_ALIGN = 10; // Time to align the robot after a turn


// Add a static variable to track the current note
static int currentNoteIndex = 0;


void LineFollowingRunner::turn(TurnDirection direction, int angle)
{
    // turn in-place
    motorRight.run(-TURN_SPEED * direction);
    motorLeft.run(-TURN_SPEED * direction);
    delay( angle * TURN_ANGLE_TO_TIME_FACTOR ); // turn for a while to get close to the line

    motorRight.stop();
    motorLeft.stop(); 
    delay(100); // wait for a bit to let the car stop

    motorRight.run(-TURN_SPEED * direction);
    motorLeft.run(-TURN_SPEED * direction);

    // find the line 
    while (lineFollower.readSensors() != S1_IN_S2_IN)
    {        
        delay(20);
    }

    // let it turn just a bit more to get the car straight
    delay(TIME_TO_ALIGN);


}
void LineFollowingRunner::turnLeft()
{
    turn(LEFT, 90);
}

void LineFollowingRunner::turnRight()
{
    turn(RIGHT, 90);
}
void LineFollowingRunner::moveForward()
{
    motorRight.run(HIGH_SPEED);
    motorLeft.run(-HIGH_SPEED);
}

void LineFollowingRunner::moveBackward()
{
    motorRight.run(-HIGH_SPEED);
    motorLeft.run(HIGH_SPEED);
}
void LineFollowingRunner::uTurn()
{
    turn(LEFT, 180);
}

void LineFollowingRunner::act(Global::RobotMovement action)
{
    switch (action)
    {
    case Global::MOVE_FORWARD:
        moveForward();
        break;
    case Global::TURN_LEFT:
        turnLeft();
        break;
    case Global::TURN_RIGHT:
        turnRight();
        break;
    case Global::U_TURN:
        uTurn();
        break;
    case Global::MOVE_BACKWARD:
        moveBackward();
        break;
    case Global::STOP:
        motorRight.stop();
        motorLeft.stop();
        
        //infinite loop to stop the robot
        while (true)
        {
            delay(1000); // Wait for a second
        }

        break;
    default:
        break;
    }
}


void LineFollowingRunner::runMaze()
{
    long lastIntersectionTime = 0;

    // Start moving
    moveForward();

    while (true)
    {         
        if (ultrasonicSensor.distanceInch() < MIN_OBSTACLE_INCHES)
        {
            // Stop the motors
            motorRight.stop();
            motorLeft.stop();
            delay(200); // Wait for a second
            
            soundPlayer.playMelody(SoundPlayer::OBSTACLE_ENCOUNTERED);            

            MazeSolver::IntersectionResult result = mazeSolver.processIntersection(Global::STATE_BLOCKED);  
            
            // the only actions that make sense here are U_TURN and MOVE_BACKWARD
            act(result.action); // Act based on the action returned by the maze solver
        }
        
        // Read line sensor values
        int sensorState = lineFollower.readSensors();
        switch (sensorState)
        {        
        
        case S1_IN_S2_IN: // Black under both sensors, move forward

            motorRight.run(HIGH_SPEED);
            motorLeft.run(-HIGH_SPEED);
            break;

        case S1_IN_S2_OUT: // Black on left sensor only - steer left
            
            motorRight.run(HIGH_SPEED);
            motorLeft.run(-LOW_SPEED);
            break;

        case S1_OUT_S2_IN: // Black on right sensor only - steer right
            stop();
            motorRight.run(LOW_SPEED);
            motorLeft.run(-HIGH_SPEED);
            break;

        case S1_OUT_S2_OUT: // No black detected at either sensor, are we at an intersection?

            // Check if enough time has passed since the last intersection
            if (millis() - lastIntersectionTime < MIN_TIME_BETWEEN_INTERSECTIONS)
            {
                // not a real intersection                
                // shouldn't we back up a bit?
                stop();
                break;
            }

            // Yes, we are at an intersection. update the last intersection time
            lastIntersectionTime = millis();

            // Move forward to align wheels with the center of intersection
            delay(TIME_TO_CENTER_INTERSECTION); //this is very important
            motorRight.stop();
            motorLeft.stop();
            delay(100);

            // Play the current note and update the index
            soundPlayer.playNoteAtIndex(currentNoteIndex);
            currentNoteIndex = (currentNoteIndex + 1) % SoundPlayer::getNotesCount();
            
            delay(50);

            // Use mazeSolver to update the status of the maze and decide the next move
            MazeSolver::IntersectionResult result = mazeSolver.processIntersection(Global::STATE_VISITED);
            
            // Use Serial to print the result for debugging
            Serial.print("Action: ");
            Serial.print(result.action);
            Serial.print(", Mode: ");
            Serial.print(result.mode);
            Serial.print(", Direction: ");
            Serial.print(result.direction);            
            Serial.print(", Current Position: (");
            Serial.print(result.currentX);
            Serial.print(", ");
            Serial.print(result.currentY);
            Serial.println(")");

            if (result.modeChanged)
            {
                if (result.mode == Global::MODE_RETURNING_HOME)
                {
                    soundPlayer.playMelody(SoundPlayer::EXPLORATION_COMPLETED);
                }
                else if (result.mode == Global::MODE_RACING_TO_TARGET)
                {
                    soundPlayer.playMelody(SoundPlayer::RACE_STARTING);
                }                
            }    

            act(result.action); // Act based on the action returned by the maze solver
            
            break;
        }        
        delay(20); // Allow time for the sensor reading to stabilize
    }
}