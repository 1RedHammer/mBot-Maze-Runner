#include "../../include/LineFollowingRunner.h"

void LineFollowingRunner::runMaze() {

    float movingDirection = 1.0; // 1 for forward, -1 for backward, 0 for stop
    float motorSpeedAdjustment = 0.2; // Adjust this value to fine-tune the speed
    
    // Adjust this value to fine-tune the steering
    // 0.0 means no adjustment, positive values mean right increase, negative values mean left increase
    float steeringAdjustment = 0.0; 
    

    //define the starting position and direction
    int x = 0, y = 0; // starting position;
    const int finalX = 2, finalY = 2; // final position
    int dx = 0, dy = 1; // direction of movement 

    //init a m x n array (0-unmarked, 1-visited, 2-blocked) to store the maze information
    int m = 3, n = 3; // dimensions of the maze    
    int maze[m][n] = {0};    
    maze[x][y] = 1; // mark the starting position as visited
    maze[finalX][finalY] = 1; // mark the final position as visited



    while (true) {

        //move forward to the next intersection, if blocked by an obstacle, reverse to the last intersection
        //left motor is wired reversed, so we need to reverse the speed to go forward

        //motorRight.run( MOTOR_SPEED * movingDirection * (1.0 + steeringAdjustment));
        //motorLeft.run( -MOTOR_SPEED * movingDirection * (1.0 - steeringAdjustment));

        if (steeringAdjustment>0){
            motorLeft.run( -MOTOR_SPEED * movingDirection);
            motorRight.run(0);
        }
        else if(steeringAdjustment<0){
            motorRight.run( MOTOR_SPEED * movingDirection);
            motorLeft.run(0);            
        }
        else{
            motorLeft.run( -MOTOR_SPEED * movingDirection);
            motorRight.run( MOTOR_SPEED * movingDirection);
        }
        
        delay(10);  // Move forward for a short duration

        // Check for obstacles
        if (ultrasonicSensor.distanceInch() < OBSTACLE_DISTANCE ) {
            
            //obstacle detected, mark the target position as blocked
            x+= dx;
            y+= dy;                
            maze[x][y] = 2;

            //reverse

            delay(1000);

            movingDirection = -1.0; // Reverse direction
        }

        // Read line sensor values
        int sensorState = lineFollower.readSensors();
    
        switch (sensorState) {
            case S1_OUT_S2_OUT:  // No Black detected - both sensors off
                
                //reached an intersection
                
                //update the maze array to mark the intersection as visited
                x+= dx;
                y+= dy;                
                maze[x][y] = 1;


                //if all intersections in the matrix are marked. Find way home

                //ALGORITHM - RIGHT TURNING
                
                //determine next intersection if making a right turn, if such position is in bound and not visited, turn right

                //if not, try to go straight

                //if not, turn left

                //else, deadend, reverse to the last intersection


                //move to the center of the intercection
                motorRight.run(MOTOR_SPEED);
                motorLeft.run(-MOTOR_SPEED);
                delay(500);
                stop();
                delay(1000);
                                

                //make a big turn first to land the sensor on open white space
                motorRight.run(0);
                motorLeft.run(-MOTOR_SPEED);
                delay(1200);

                //look for the first road on the right
                while (lineFollower.readSensors() != S1_IN_S2_IN) {
                    motorRight.run(0);
                    motorLeft.run(-MOTOR_SPEED);
                    delay(50);
                }             
                break;
                
            case S1_OUT_S2_IN:  // Black on right sensor only - go straight with slight right turn
                steeringAdjustment = motorSpeedAdjustment;                 
                break;
                
            case S1_IN_S2_OUT:  // Black on left sensor only - go straight with slight left turn
                steeringAdjustment = -motorSpeedAdjustment;                                                 
                break;
                
            case S1_IN_S2_IN:  // Black under both sensors
                steeringAdjustment = 0 ;
                break;
        }
        
        delay(10);  // Small delay to prevent sensor reading too frequently
    }
}