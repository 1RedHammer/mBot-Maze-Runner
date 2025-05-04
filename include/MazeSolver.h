#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H

#include <stdint.h>
#include "GlobalConstants.h"

// Handles maze tracking and navigation logic

class MazeSolver {
private:
    
    // Start and target positions in the maze
    uint8_t startColumn, startRow;
    uint8_t targetColumn, targetRow;    
            
    // Maze state tracking:
    // 0 = unexplored
    // 1 = visited
    // 2 = wall/blocked
    uint8_t mazeState[GlobalConstants::MAZE_HEIGHT][GlobalConstants::MAZE_WIDTH];
    
    // Current position
    uint8_t currentColumn, currentRow;



    // array indice offset {row, column} for each direction
    int8_t directionOffsets[4][2] = { {-1, 0}, // North
                                      {0, 1}, // East
                                      {1, 0},  // South
                                      {0, -1}   // West
                                    };
    
    //getters
    uint8_t getCurrentColumn() const { return currentColumn; }
    uint8_t getCurrentRow() const { return currentRow; }
    int8_t getOffsetColumn() const { return directionOffsets[currentDirection][1]; }
    int8_t getOffsetRow() const { return directionOffsets[currentDirection][0]; }
    uint8_t getTargetColumn() const { return targetColumn; }
    uint8_t getTargetRow() const { return targetRow; }

public:

    enum Direction {
        NORTH = 0,
        EAST = 1,
        SOUTH = 2,
        WEST = 3,
        NONE = -100    
    };
   
    Direction currentDirection = NONE; // Initialize current direction to NONE

    // Constructor with start and target positions
    MazeSolver(uint8_t startX, uint8_t startY, uint8_t targetX, uint8_t targetY, MazeSolver::Direction startDirection) 
        : startColumn(startX), startRow(GlobalConstants::MAZE_HEIGHT - 1 - startY),
          targetColumn(targetX), targetRow(GlobalConstants::MAZE_HEIGHT - 1 - targetY),
          currentColumn(startX), currentRow(GlobalConstants::MAZE_HEIGHT - 1 - startY),
          currentDirection(startDirection) {

        // Initialize the maze state to unexplored (0)
        for (uint8_t row = 0; row < GlobalConstants::MAZE_HEIGHT; ++row) {
            for (uint8_t column = 0; column < GlobalConstants::MAZE_WIDTH; ++column) {
                mazeState[row][column] = 0; // Unexplored
            }
        }
        mazeState[startRow][startColumn] = 1; // Mark start as visited
 
    }

    enum State {
        STATE_UNEXPLORED = 0, // Unexplored cell
        STATE_VISITED = 1,    // Visited cell
        STATE_BLOCKED = 2,
        STATE_UNREACHABLE = 3     // Blocked cell
    };

    enum Action {
        STOP = 0,
        MOVE_FORWARD = 1,
        TURN_LEFT = 2,
        TURN_RIGHT = 3,
        U_TURN = 4,
        MOVE_BACKWARD = 5
    };

    enum Mode {
        MODE_EXPLORING = 0,       // Exploring and mapping the maze
        MODE_EXPLORING_WITH_PATHFINDING = 1, // Exploring with pathfinding
        MODE_RETURNING_HOME = 2,  // Returning to the starting point
        MODE_RACING_TO_TARGET = 3,   // Racing to the target
        MODE_RACE_COMPLETED = 4,
        MODE_TARGET_UNREACHABLE = 5                
    };

    Mode mode = MODE_EXPLORING; // Default mode

    struct IntersectionResult {
        Mode mode;
        Action action;        
        Direction direction;                
        uint8_t currentX;
        uint8_t currentY;
    };

    struct ModeAndDirection {
        Mode mode;
        Direction direction;
    };

    IntersectionResult processIntersection(State state) {

        // Update the current position to the intersection, based on direction of the movement that just finished
        currentRow += directionOffsets[currentDirection][0]; // Update current column based on offset
        currentColumn += directionOffsets[currentDirection][1]; // Update current row based on offset

        //update state of cell at the intersection
        mazeState[currentRow][currentColumn] = state;
        
        // if the intersection is blocked, we need to turn around
        if (state == STATE_BLOCKED) {
        
            currentDirection = (Direction)((currentDirection + 2) % 4); // Turn around
            return {mode, U_TURN, currentDirection, getCurrentX(), getCurrentY()};
        }

    
        // otherwise, we need to decide the next direction to go

        ModeAndDirection next = decideNext();

        Action action = determineActionBasedOnDirection(next.direction); // Determine action based on direction, mode may change too.

        currentDirection = next.direction; // Update current direction
        mode = next.mode; // Update mode

        return {mode, action, currentDirection, getCurrentX(), getCurrentY()}; // Return the action and mode

    }

    Direction findPathTo(uint8_t row, uint8_t column) {
        //make an new array to store the path lengths of each cell to the target cell [row][column]
        int8_t pathLength[GlobalConstants::MAZE_HEIGHT][GlobalConstants::MAZE_WIDTH];

        for (uint8_t r = 0; r < GlobalConstants::MAZE_HEIGHT; ++r) {
            for (uint8_t c = 0; c < GlobalConstants::MAZE_WIDTH; ++c) {
                pathLength[r][c] = -1; // Initialize path lengths to -1
            }
        }
        pathLength[row][column] = 0; // the target cell is 0 away from itself

        //mark all the blocked cells
        for (uint8_t r = 0; r < GlobalConstants::MAZE_HEIGHT; ++r) {
            for (uint8_t c = 0; c < GlobalConstants::MAZE_WIDTH; ++c) {
                if (mazeState[r][c] == STATE_BLOCKED) {
                    pathLength[r][c] = -100; // Mark blocked cells
                }
            }
        }

        // Breadth-first search algorithm to find the shortest path lengths from the target cell
        bool done = false;
        int currentPathLength = 0;
        while(!done){
            bool foundLongerPath = false;
            for (uint8_t r = 0; r < GlobalConstants::MAZE_HEIGHT; ++r) {
                for (uint8_t c = 0; c < GlobalConstants::MAZE_WIDTH; ++c) {
                    if(pathLength[r][c] == currentPathLength) {
                        // Check all 4 directions and update path lengths
                        if (r > 0 && pathLength[r-1][c] == -1) { // North
                            foundLongerPath = true;
                            pathLength[r-1][c] = currentPathLength + 1;
                        }
                        if (r < GlobalConstants::MAZE_HEIGHT - 1 && pathLength[r+1][c] == -1) { // South
                            foundLongerPath = true;
                            pathLength[r+1][c] = currentPathLength + 1;
                        }
                        if (c > 0 && pathLength[r][c-1] == -1) { // West    
                            foundLongerPath = true;
                            pathLength[r][c-1] = currentPathLength + 1;
                        }
                        if (c < GlobalConstants::MAZE_WIDTH - 1 && pathLength[r][c+1] == -1) { // East
                            foundLongerPath = true;
                            pathLength[r][c+1] = currentPathLength + 1;
                        }
                    }
                }
            }
            if(!foundLongerPath){
                done = true;
            }
            currentPathLength++;
        }

        // From the current runner position, find the direction to move based on the path length
        int pathLengthFound = pathLength[currentRow][currentColumn];

        if(currentRow != GlobalConstants::MAZE_HEIGHT-1  && pathLength[currentRow+1][currentColumn] == pathLengthFound-1){
            return SOUTH;
        }
        if(currentColumn != GlobalConstants::MAZE_WIDTH-1 && pathLength[currentRow][currentColumn+1] == pathLengthFound-1){
            return EAST;
        }
        if(currentRow != 0 && pathLength[currentRow-1][currentColumn] == pathLengthFound - 1){
            return NORTH;
        }
        if(currentColumn != 0 && pathLength[currentRow][currentColumn-1] == pathLengthFound-1){
            return WEST;
        }
        return NONE; // No path found
    };

    ModeAndDirection decideNext() {       

        Mode nextMode = mode;

        if (mode == MODE_EXPLORING || mode == MODE_EXPLORING_WITH_PATHFINDING) {
         
            nextMode = MODE_EXPLORING; // Default exploring mode

            bool directionRuledOut[4] = {false, false, false, false}; // N, E, S, W 
      
            // Check all 4 directions and update based on mazeState

            // Check if the current position is at the edge of the maze
            if (currentRow == 0) {
                directionRuledOut[NORTH] = true;
            }
            if (currentColumn == GlobalConstants::MAZE_WIDTH - 1) {
                directionRuledOut[EAST] = true;
            }
            if (currentColumn == 0) {
                directionRuledOut[WEST] = true;
            }
            if (currentRow == GlobalConstants::MAZE_HEIGHT - 1) {
                directionRuledOut[SOUTH] = true;
            }

            // Check if the adjacent cells are blocked or visited
            if (mazeState[currentRow + 1][currentColumn] == STATE_BLOCKED || mazeState[currentRow + 1][currentColumn] == STATE_VISITED) {
                directionRuledOut[SOUTH] = true;
            }
            if (mazeState[currentRow - 1][currentColumn] == STATE_BLOCKED || mazeState[currentRow - 1][currentColumn] == STATE_VISITED) {
                directionRuledOut[NORTH] = true;
            }
            if (mazeState[currentRow][currentColumn + 1] == STATE_BLOCKED || mazeState[currentRow][currentColumn + 1] == STATE_VISITED) {
                directionRuledOut[EAST] = true;
            }
            if (mazeState[currentRow][currentColumn - 1] == STATE_BLOCKED || mazeState[currentRow][currentColumn - 1] == STATE_VISITED) {
                directionRuledOut[WEST] = true;
            }
            
            
            //find the direction that is not ruled out in this order: right, forward, left
            for (int i = 1 ; i >= -1; i--) {
                if ( directionRuledOut[(currentDirection + i) % 4] == false) { //found a direction to go
                    return {nextMode, (Direction)((currentDirection + i+4) % 4)}; // Set the direction to go                    
                }
            }

            // No way to go, find unexplored cells
            for (int row = 0; row < GlobalConstants::MAZE_HEIGHT; row++) {
                for (int column = 0; column < GlobalConstants::MAZE_WIDTH; column++) {
                    if (mazeState[row][column] == STATE_UNEXPLORED) {

                        nextMode = MODE_EXPLORING_WITH_PATHFINDING; // Set the mode to exploring with pathfinding
                        Direction nextDirection = findPathTo(row, column);
        
                        if (nextDirection != NONE) {
                            return {nextMode, nextDirection}; // Move towards the unexplored cell
                        }
                        else {
                            mazeState[row][column] = STATE_UNREACHABLE; // Mark as unreachable if no path found
                        }                        
                    }
                }
            }
                        
            nextMode = MODE_RETURNING_HOME; // No unexplored cells found, set mode to returning home

            
        } // End of exploring mode
        
        if (nextMode == MODE_RETURNING_HOME) 
        {
            Direction nextDirection = findPathTo(startRow, startColumn); // Find path to the starting point
            if (nextDirection != NONE) {
                return {nextMode, nextDirection}; // Move towards the starting point
            } else { // we are already at the starting point

                if (mazeState[targetRow][targetColumn] == STATE_UNREACHABLE) {
                    nextMode = MODE_TARGET_UNREACHABLE; // Set mode to target unreachable
                } else {
                    nextMode = MODE_RACING_TO_TARGET;
                }
            }
        }      

        if (nextMode == MODE_RACING_TO_TARGET) {
            Direction nextDirection = findPathTo(targetRow, targetColumn); // Find path to the target
            if (nextDirection != NONE) {
                return {nextMode, nextDirection}; // Move towards the target
            }
            else { // we are already at the target
                if (targetRow == startRow && targetColumn == startColumn) {
                    nextMode = MODE_RACE_COMPLETED;
                    return {nextMode, NONE}; // Stop the robot
                }
            }
        }

        // No unexplored cells found, stop the robot, for now
        return {MODE_TARGET_UNREACHABLE, NONE} ; // No valid direction found

    }

    Action determineActionBasedOnDirection(Direction nextDirection) {
        
        if (nextDirection == NONE) {
            return STOP;
        }

        // Calculate the difference in direction, add 4 to handle negative values
        switch ((nextDirection - currentDirection + 4) % 4) 
        {
            case 0: // Forward
                return MOVE_FORWARD;
            case 1: // Right
                return TURN_RIGHT;
            case 2: // Backward
                return MOVE_BACKWARD;
            case 3: // Left
                return TURN_LEFT;         
        }        
        return STOP; // Default action if no valid direction is found
    }

    int8_t getMode() const { return mode; }

    uint8_t getStateAtCoordinates(uint8_t x, uint8_t y) const {
        return mazeState[GlobalConstants::MAZE_HEIGHT - 1 - y][x];
    }

    uint8_t getCurrentX() const { 
        return currentColumn; // Column corresponds to X-coordinate
    }

    uint8_t getCurrentY() const { 
        return GlobalConstants::MAZE_HEIGHT - 1 - currentRow; // Reverse the Y-axis
    }

    uint8_t getTargetX() const { 
        return targetColumn; // Column corresponds to X-coordinate
    }

    uint8_t getTargetY() const { 
        return GlobalConstants::MAZE_HEIGHT - 1 - targetRow; // Reverse the Y-axis
    }

    int8_t getOffsetX() const { 
        return getOffsetColumn();
    }

    int8_t getOffsetY() const { 
        return -getOffsetRow();
    }

    //setters
    void setCurrent(uint8_t x, uint8_t y) {
        currentColumn = x;
        currentRow = GlobalConstants::MAZE_HEIGHT - 1 - y; // Reverse the Y-axis
    }

    void setStart(uint8_t column, uint8_t row) {
        startColumn = column;
        startRow = GlobalConstants::MAZE_HEIGHT - 1 - row; // Reverse the Y-axis
    }

    void setTarget(uint8_t x, uint8_t y) {
        targetColumn = x;
        targetRow = GlobalConstants::MAZE_HEIGHT - 1 - y; // Reverse the Y-axis
    }
    
    void setStateAtCoordinates(uint8_t x, uint8_t y, uint8_t value) {        
        mazeState[GlobalConstants::MAZE_HEIGHT - 1 - y][x] = value; // Reverse the Y-axis        
    }

    void setDirection(Direction direction) {
        currentDirection = direction; // Set the current direction
    }
    
};

#endif