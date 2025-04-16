#include "../../include/LineFollowingRunner.h"

static constexpr float BASE_MOTOR_SPEED = 70;     // range from 0 to 255
static constexpr float MIN_OBSTACLE_INCHES = 2;   // Distance in inches to detect an obstacle
static constexpr float MAX_TURN_RATIO = 1.0f;      // Max turning adjustment (1.0 = inner wheel stops)

static constexpr uint8_t MAZE_WIDTH = 3; // Maze dimensions
static constexpr uint8_t MAZE_HEIGHT = 3; // Maze dimensions
static constexpr uint8_t TARGET_X = 2, TARGET_Y = 2;
static constexpr uint8_t START_X = 0, START_Y = 0;

void LineFollowingRunner::runMaze() {
    // Movement control
    int8_t directionMultiplier = 1;  // 1=forward, -1=backward, 0=stop
    float turnAdjustment = 0.0;       // +right, -left
    
    // Maze navigation
    uint8_t x = START_X, y = START_Y;
    int8_t deltaX = 0, deltaY = 1;
    
    // Maze state tracking (0=unexplored, 1=visited, 2=blocked)
    uint8_t mazeState[MAZE_HEIGHT][MAZE_WIDTH] = {0};
    mazeState[x][y] = 1;                        // Mark start as visited
    mazeState[TARGET_X][TARGET_Y] = 1;          // Mark target as visited

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
        if (ultrasonicSensor.distanceInch() < MIN_OBSTACLE_INCHES  && directionMultiplier > 0) {
            
            //obstacle detected, mark the target position as blocked
            x += deltaX;
            y += deltaY;
            mazeState[x][y] = 2;

            //reverse
            directionMultiplier = -1.0; // Reverse direction
        }

        // Read line sensor values
        int sensorState = lineFollower.readSensors();
    
        switch (sensorState) {
            case S1_OUT_S2_OUT:  // No Black detected - both sensors off
                
                //reached an intersection
                
                //update the maze array to mark the intersection as visited
                x += deltaX;
                y += deltaY;                
                mazeState[x][y] = 1;


                //if all intersections in the matrix are marked. Find way home

                //ALGORITHM - RIGHT TURNING
                
                //determine next intersection if making a right turn, if such position is in bound and not visited, turn right

                //if not, try to go straight

                //if not, turn left

                //else, deadend, reverse to the last intersection


                //move to the center of the intersection

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