#include "../../include/LineFollowingRunner.h"

void LineFollowingRunner::runMaze() {

    int movingDirection = 1; // 1 for forward, -1 for backward, 0 for stop
    
    
    // Adjust this value to fine-tune the steering
    // 0.0 means no adjustment, positive values mean right adjustment, negative values mean left adjustment
    float steeringAdjustment = 0.0; 
    

    while (true) {

        //move forward to the next intersection, if blocked by an obstacle, reverse to the last intersection

        motorLeft.run( -MOTOR_SPEED * movingDirection * (1 + steeringAdjustment)); // Adjust left motor speed for steering
        motorRight.run( MOTOR_SPEED * movingDirection * (1 - steeringAdjustment)); // Adjust right motor speed for steering        

        
        delay(100);  // Move forward for a short duration

        // Check for obstacles
        if (ultrasonicSensor.distanceInch() < OBSTACLE_DISTANCE ) {
            // Stop if obstacle detected            
            buzzer.tone(1000, 200);  // Alert sound

            delay(1000);

            movingDirection = -1; // Reverse direction            
        }

        // Read line sensor values
        int sensorState = lineFollower.readSensors();
    
        switch (sensorState) {
            case S1_IN_S2_IN:  // No line detected - both sensors off
                
                //could be turning and looking for the line
                
                buzzer.tone(500, 100);  // Alert that line is lost
                
                break;
                
            case S1_IN_S2_OUT:  // Line on right sensor only - go straight with slight right adjustment
                steeringAdjustment = 0.1; // Adjust right motor speed to tend rightward                
                break;
                
            case S1_OUT_S2_IN:  // Line on left sensor only - go straight with slight left adjustment
                steeringAdjustment = -0.1; // Adjust right motor speed to tend rightward
                break;
                
            case S1_OUT_S2_OUT:  // Line under both sensors - could be intersection or thick line
                

                //always turn right at intersection
                motorLeft.run(-MOTOR_SPEED); // Adjust left motor speed for turning
                motorRight.run(0); // Adjust right motor speed for turning

                delay(1000); //need to calibrate the turn duration

                break;
        }
        
        delay(10);  // Small delay to prevent sensor reading too frequently
    }
}



