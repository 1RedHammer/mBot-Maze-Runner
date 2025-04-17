#include "../../include/LineFollowingRunner.h"

static constexpr float BASE_MOTOR_SPEED = 80;     // range from 0 to 255
static constexpr float MIN_OBSTACLE_INCHES = 2;   // Distance in inches to detect an obstacle
static constexpr float MAX_TURN_RATIO = 1.0f;      // Max turning adjustment (1.0 = inner wheel stops)

//TODO: implement the movement functions in the base class
void LineFollowingRunner::turnLeft() {}

void LineFollowingRunner::turnRight() {    

    motorRight.run(BASE_MOTOR_SPEED);
    motorLeft.run(-BASE_MOTOR_SPEED);
    delay(100);                


    //make a big turn first to land the sensor on open white space                
    motorRight.run(0);
    motorLeft.run(-BASE_MOTOR_SPEED/2.0);
    delay(800);

    //look for the first road on the right
    while (lineFollower.readSensors() != S1_IN_S2_IN) {
        motorRight.run(0);
        motorLeft.run(-BASE_MOTOR_SPEED);
        delay(50);
    }

}
void LineFollowingRunner::moveForward() {}
void LineFollowingRunner::moveBackward() {}
void LineFollowingRunner::uTurn() {}



void LineFollowingRunner::runMaze() {
    // Movement control
    int8_t directionMultiplier = 1;  // 1=forward, -1=backward, 0=stop
    float turnAdjustment = 0.0;       // +right, -left
    
    
    while (true) {
        // Motor speed calculations
        float rightWheelRatio = 1.0, leftWheelRatio = 1.0;
        if (turnAdjustment > 0) {
            rightWheelRatio = 1.0 - turnAdjustment;
        } else if (turnAdjustment < 0) {
            leftWheelRatio = 1.0 + turnAdjustment;
        }
        
        motorRight.run(BASE_MOTOR_SPEED * directionMultiplier * rightWheelRatio);
        motorLeft.run(-BASE_MOTOR_SPEED * directionMultiplier * leftWheelRatio);

        delay(10);  // Move forward for a short duration

        // Check for obstacles
        
        // Read line sensor values
        int sensorState = lineFollower.readSensors();
    
        switch (sensorState) {
            case S1_OUT_S2_OUT:  // No Black detected - both sensors off
                
                //reached an intersection
                
                
                // TODO: Use mazeSolver to update the status of the maze and decide the next move

                mazeSolver.markCurrentPosition(MazeSolver::State::STATE_VISITED); // Mark current position as visited);
                mazeSolver.updatePosition();                
                MazeSolver::Action action =  mazeSolver.decideNextMove();

                switch (action) {
                    case MazeSolver::MOVE_FORWARD:
                        directionMultiplier = 1;
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
                        directionMultiplier = -1;
                        break;
                    default:
                        directionMultiplier = 0; // Stop
                }                
                break;
                
            case S1_OUT_S2_IN:  // Black on right sensor only - right turn
                turnAdjustment = MAX_TURN_RATIO;                 
                break;
                
            case S1_IN_S2_OUT:  // Black on left sensor only - left turn
                turnAdjustment = -MAX_TURN_RATIO;                                                 
                break;
                
            case S1_IN_S2_IN:  // Black under both sensors
                turnAdjustment = 0 ;
                break;
        }
        
        delay(10);  // Small delay to prevent sensor reading too frequently
    }
}