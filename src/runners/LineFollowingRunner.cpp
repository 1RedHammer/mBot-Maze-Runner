#include "../../include/LineFollowingRunner.h"

static constexpr float MIN_OBSTACLE_INCHES = 2; // Distance in inches to detect an obstacle
static constexpr int LOW_SPEED = 35;            // Low speed for steering
static constexpr int HIGH_SPEED = 70;           // High speed for moving forward
static constexpr int TURN_SPEED = 80;           // Speed for turning in place
static constexpr int MIN_TIME_BETWEEN_INTERSECTIONS = 2500;

// Define musical notes
static constexpr int NOTE_C = 262;
static constexpr int NOTE_D = 294;
static constexpr int NOTE_E = 330;
static constexpr int NOTE_F = 349;
static constexpr int NOTE_G = 392;
static constexpr int NOTE_A = 440;
static constexpr int NOTE_B = 494;

// Add a static variable to track the current note
static int currentNoteIndex = 0;
// Extend the notes array to include the full scale
static constexpr int notes[] = {NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_G, NOTE_A, NOTE_B};

void LineFollowingRunner::turn(TurnDirection direction)
{
    // turn in-place
    motorRight.run(-TURN_SPEED * direction);
    motorLeft.run(-TURN_SPEED * direction);
    delay(300); // turn for a while to get close to the line

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
    delay(40);


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

void LineFollowingRunner::moveBackward()
{
    motorRight.run(-HIGH_SPEED);
    motorLeft.run(HIGH_SPEED);
}
void LineFollowingRunner::uTurn()
{
}

void LineFollowingRunner::runMaze()
{
    long lastIntersectionTime = 0;

    // Start moving
    moveForward();

    while (true)
    { // TODO - Check for obstacles

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
            delay(850); //this is very important
            motorRight.stop();
            motorLeft.stop();
            delay(100);

            // Play the current note and update the index
            buzzer.tone(notes[currentNoteIndex], 100); // Play the current note
            currentNoteIndex = (currentNoteIndex + 1) % (sizeof(notes) / sizeof(notes[0])); // Move to the next note
            
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
                moveForward();
                break;
            case MazeSolver::MOVE_BACKWARD:
                break;
            default:
                break;
            }
            break;
        }        
        delay(20); // Allow time for the sensor reading to stabilize
    }
}