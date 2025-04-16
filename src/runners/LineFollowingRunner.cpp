#include "../../include/LineFollowingRunner.h"

// Robot movement configuration
static constexpr float BASE_MOTOR_SPEED = 70;     // Base motor speed (0-255)
static constexpr float MIN_OBSTACLE_INCHES = 2;   // Minimum distance to obstacles
static constexpr float TURN_SPEED = 50;           // Speed for intersection turns

void LineFollowingRunner::runMaze() {
    // Movement direction (1=forward, -1=backward, 0=stop)
    int8_t direction = 1;  
    
    while (true) {
        // Basic forward movement
        // TODO: Set motor speeds for forward motion
        
        delay(10);  // Prevent sensor read overflow

        // Wall detection and avoidance
        // TODO: Check ultrasonic sensor and handle obstacles
        // - Check distance with ultrasonicSensor.distanceInch()
        // - Update maze state if blocked
        // - Reverse direction if needed

        // Line following logic
        int sensorState = lineFollower.readSensors();
    
        // Handle different line sensor states
        switch (sensorState) {
            case S1_OUT_S2_OUT:  // At intersection (no line detected)
                // TODO: Handle intersection navigation
                // - Stop and update maze position
                // - Determine next direction
                // - Execute turn if needed
                break;
                
            case S1_OUT_S2_IN:   // Line detected on right
                // TODO: Adjust course right
                break;
                
            case S1_IN_S2_OUT:   // Line detected on left
                // TODO: Adjust course left
                break;
                
            case S1_IN_S2_IN:    // Centered on line
                // TODO: Maintain straight course
                break;
        }
        
        delay(10);
    }
}