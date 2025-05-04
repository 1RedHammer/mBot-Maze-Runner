#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H

#include <stdint.h>
#include "GlobalConstants.h"

// Handles maze tracking and navigation logic

class MazeSolver {
private:
    
    Global::MazeCellState mazeState[Global::MAZE_HEIGHT][Global::MAZE_WIDTH];

    // Start and target positions in the maze
    uint8_t startColumn, startRow;
    uint8_t targetColumn, targetRow;    
    uint8_t currentColumn, currentRow;            
    
    // array indice offset {row, column} for each direction
    int8_t directionOffsets[4][2] = {
        {-1, 0}, // North
        {0, 1},  // East
        {1, 0},  // South
        {0, -1}  // West
    };

    Global::Direction currentDirection = Global::NO_DIRECTION;
    Global::Mode mode = Global::MODE_EXPLORING; // Default mode

                                
    //getters
    uint8_t getCurrentColumn() const { return currentColumn; }
    uint8_t getCurrentRow() const { return currentRow; }
    int8_t getOffsetColumn() const { return directionOffsets[currentDirection][1]; }
    int8_t getOffsetRow() const { return directionOffsets[currentDirection][0]; }
    uint8_t getTargetColumn() const { return targetColumn; }
    uint8_t getTargetRow() const { return targetRow; }

public:
    // Constructor with start and target positions
    MazeSolver(uint8_t startX, uint8_t startY, uint8_t targetX, uint8_t targetY, Global::Direction startDirection) 
        : startColumn(startX), startRow(Global::MAZE_HEIGHT - 1 - startY),
          targetColumn(targetX), targetRow(Global::MAZE_HEIGHT - 1 - targetY),
          currentColumn(startX), currentRow(Global::MAZE_HEIGHT - 1 - startY),
          currentDirection(startDirection) {

        // Initialize the maze state to unexplored (0)
        for (uint8_t row = 0; row < Global::MAZE_HEIGHT; ++row) {
            for (uint8_t column = 0; column < Global::MAZE_WIDTH; ++column) {
                mazeState[row][column] = Global::STATE_UNEXPLORED ; // Unexplored
            }
        }
        mazeState[startRow][startColumn] = Global::STATE_VISITED; // Mark start as visited
    }
    
    struct IntersectionResult {
        Global::Mode mode;
        Global::RobotMovement action;        
        Global::Direction direction;                
        uint8_t currentX;
        uint8_t currentY;
        bool modeChanged;
    };

    struct ModeAndDirection {
        Global::Mode mode;
        Global::Direction direction;
    };

    IntersectionResult processIntersection(Global::MazeCellState state) {

        // Update the current position to the intersection, based on direction of the movement that just finished
        currentRow += directionOffsets[currentDirection][0]; // Update current column based on offset
        currentColumn += directionOffsets[currentDirection][1]; // Update current row based on offset

        //update state of cell at the intersection
        mazeState[currentRow][currentColumn] = state;
        
        // if the intersection is blocked, we need to turn around
        if (state == Global::STATE_BLOCKED) {
        
            currentDirection = (Global::Direction)((currentDirection + 2) % 4); // Turn around
            return {mode, Global::U_TURN, currentDirection, getCurrentX(), getCurrentY(), false};
        }

    
        // otherwise, we need to decide the next direction to go

        ModeAndDirection next = decideNext();

        Global::RobotMovement action = determineActionBasedOnDirection(next.direction); // Determine action based on direction, mode may change too.

        currentDirection = next.direction; // Update current direction

        bool modeChanged = false;
        if (next.mode != mode) {
            modeChanged = true; // Mode has changed
        }
        mode = next.mode; // Update mode

        return {mode, action, currentDirection, getCurrentX(), getCurrentY(), modeChanged}; // Return the action and mode

    }

    Global::Direction findPathTo(uint8_t row, uint8_t column) {
        //make an new array to store the path lengths of each cell to the target cell [row][column]
        int8_t pathLength[Global::MAZE_HEIGHT][Global::MAZE_WIDTH];

        for (uint8_t r = 0; r < Global::MAZE_HEIGHT; ++r) {
            for (uint8_t c = 0; c < Global::MAZE_WIDTH; ++c) {
                pathLength[r][c] = -1; // Initialize path lengths to -1
            }
        }
        pathLength[row][column] = 0; // the target cell is 0 away from itself

        //mark all the blocked cells
        for (uint8_t r = 0; r < Global::MAZE_HEIGHT; ++r) {
            for (uint8_t c = 0; c < Global::MAZE_WIDTH; ++c) {
                if (mazeState[r][c] == Global::STATE_BLOCKED) {
                    pathLength[r][c] = -100; // Mark blocked cells
                }
            }
        }

        // Breadth-first search algorithm to find the shortest path lengths from the target cell
        bool done = false;
        int currentPathLength = 0;
        while(!done){
            bool foundLongerPath = false;
            for (uint8_t r = 0; r < Global::MAZE_HEIGHT; ++r) {
                for (uint8_t c = 0; c < Global::MAZE_WIDTH; ++c) {
                    if(pathLength[r][c] == currentPathLength) {
                        // Check all 4 directions and update path lengths
                        if (r > 0 && pathLength[r-1][c] == -1) { // North
                            foundLongerPath = true;
                            pathLength[r-1][c] = currentPathLength + 1;
                        }
                        if (r < Global::MAZE_HEIGHT - 1 && pathLength[r+1][c] == -1) { // South
                            foundLongerPath = true;
                            pathLength[r+1][c] = currentPathLength + 1;
                        }
                        if (c > 0 && pathLength[r][c-1] == -1) { // West    
                            foundLongerPath = true;
                            pathLength[r][c-1] = currentPathLength + 1;
                        }
                        if (c < Global::MAZE_WIDTH - 1 && pathLength[r][c+1] == -1) { // East
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

        if(currentRow != Global::MAZE_HEIGHT-1  && pathLength[currentRow+1][currentColumn] == pathLengthFound-1){
            return Global::SOUTH;
        }
        if(currentColumn != Global::MAZE_WIDTH-1 && pathLength[currentRow][currentColumn+1] == pathLengthFound-1){
            return Global::EAST;
        }
        if(currentRow != 0 && pathLength[currentRow-1][currentColumn] == pathLengthFound - 1){
            return Global::NORTH;
        }
        if(currentColumn != 0 && pathLength[currentRow][currentColumn-1] == pathLengthFound-1){
            return Global::WEST;
        }
        return Global::NO_DIRECTION; // No path found
    };

    ModeAndDirection decideNext() {       

        Global::Mode nextMode = mode;

        if (mode == Global::MODE_EXPLORING || mode == Global::MODE_EXPLORING_WITH_PATHFINDING) {
         
            nextMode = Global::MODE_EXPLORING; // Default exploring mode

            bool directionRuledOut[4] = {false, false, false, false}; // N, E, S, W 
      
            // Check all 4 directions and update based on mazeState

            // Check if the current position is at the edge of the maze
            if (currentRow == 0) {
                directionRuledOut[Global::NORTH] = true;
            }
            if (currentColumn == Global::MAZE_WIDTH - 1) {
                directionRuledOut[Global::EAST] = true;
            }
            if (currentColumn == 0) {
                directionRuledOut[Global::WEST] = true;
            }
            if (currentRow == Global::MAZE_HEIGHT - 1) {
                directionRuledOut[Global::SOUTH] = true;
            }

            // Check if the adjacent cells are blocked or visited
            if (mazeState[currentRow + 1][currentColumn] == Global::STATE_BLOCKED || mazeState[currentRow + 1][currentColumn] == Global::STATE_VISITED) {
                directionRuledOut[Global::SOUTH] = true;
            }
            if (mazeState[currentRow - 1][currentColumn] == Global::STATE_BLOCKED || mazeState[currentRow - 1][currentColumn] == Global::STATE_VISITED) {
                directionRuledOut[Global::NORTH] = true;
            }
            if (mazeState[currentRow][currentColumn + 1] == Global::STATE_BLOCKED || mazeState[currentRow][currentColumn + 1] == Global::STATE_VISITED) {
                directionRuledOut[Global::EAST] = true;
            }
            if (mazeState[currentRow][currentColumn - 1] == Global::STATE_BLOCKED || mazeState[currentRow][currentColumn - 1] == Global::STATE_VISITED) {
                directionRuledOut[Global::WEST] = true;
            }
            
            
            //find the direction that is not ruled out in this order: right, forward, left
            for (int i = 1 ; i >= -1; i--) {
                if ( directionRuledOut[(currentDirection + i) % 4] == false) { //found a direction to go
                    return {nextMode, (Global::Direction)((currentDirection + i+4) % 4)}; // Set the direction to go                    
                }
            }


            
            //no way to go, see if we can go to the target cell
            if (mazeState[targetRow][targetColumn] == Global::STATE_UNEXPLORED) {
                nextMode = Global::MODE_EXPLORING_WITH_PATHFINDING; // Set the mode to exploring with pathfinding
                Global::Direction nextDirection = findPathTo(targetRow, targetColumn);
            
                if (nextDirection != Global::NO_DIRECTION) {
                    return {nextMode, nextDirection}; // Move towards the target cell
                } else {
                    mazeState[targetRow][targetColumn] = Global::STATE_UNREACHABLE; // Mark as unreachable if no path found
                    return {Global::MODE_TARGET_UNREACHABLE, Global::NO_DIRECTION}; 
                }
            }
            

            // Find unexplored cells (this is after target cell already explored)
            for (int row = Global::MAZE_HEIGHT - 1; row >= 0; row--) {
                for (int column = Global::MAZE_WIDTH - 1; column >= 0; column--) {
                    if (mazeState[row][column] == Global::STATE_UNEXPLORED) {
            
                        nextMode = Global::MODE_EXPLORING_WITH_PATHFINDING; // Set the mode to exploring with pathfinding
                        Global::Direction nextDirection = findPathTo(row, column);
            
                        if (nextDirection != Global::NO_DIRECTION) {
                            return {nextMode, nextDirection}; // Move towards the unexplored cell
                        } else {
                            mazeState[row][column] = Global::STATE_UNREACHABLE; // Mark as unreachable if no path found                           
                        }
                    }
                }
            }
                        
            nextMode = Global::MODE_RETURNING_HOME; // No unexplored cells found, set mode to returning home

            
        } // End of exploring mode
        
        if (nextMode == Global::MODE_RETURNING_HOME) 
        {
            Global::Direction nextDirection = findPathTo(startRow, startColumn); // Find path to the starting point
            if (nextDirection != Global::NO_DIRECTION) {
                return {nextMode, nextDirection}; // Move towards the starting point
            } else { // we are already at the starting point

                if (mazeState[targetRow][targetColumn] == Global::STATE_UNREACHABLE) {
                    nextMode = Global::MODE_TARGET_UNREACHABLE; // Set mode to target unreachable
                } else {
                    nextMode = Global::MODE_RACING_TO_TARGET;
                }
            }
        }      

        if (nextMode == Global::MODE_RACING_TO_TARGET) {
            Global::Direction nextDirection = findPathTo(targetRow, targetColumn); // Find path to the target
            if (nextDirection != Global::NO_DIRECTION) {
                return {nextMode, nextDirection}; // Move towards the target
            }
            else { // we are already at the target
                if (targetRow == currentRow && targetColumn == currentColumn) {
                    nextMode = Global::MODE_RACE_COMPLETED;
                    return {nextMode, Global::NO_DIRECTION}; // Stop the robot
                }
            }
        }

        // No unexplored cells found, stop the robot, for now
        return {Global::MODE_TARGET_UNREACHABLE, Global::NO_DIRECTION} ; // No valid direction found

    }

    Global::RobotMovement determineActionBasedOnDirection(Global::Direction nextDirection) {
        
        if (nextDirection == Global::NO_DIRECTION) {
            return Global::STOP;
        }

        // Calculate the difference in direction, add 4 to handle negative values
        switch ((nextDirection - currentDirection + 4) % 4) 
        {
            case 0: // Forward
                return Global::MOVE_FORWARD;
            case 1: // Right
                return Global::TURN_RIGHT;
            case 2: // Backward
                return Global::U_TURN;
            case 3: // Left
                return Global::TURN_LEFT;         
        }        
        return Global::STOP; // Default action if no valid direction is found
    }

    int8_t getMode() const { return mode; }

    uint8_t getStateAtCoordinates(uint8_t x, uint8_t y) const {
        return mazeState[Global::MAZE_HEIGHT - 1 - y][x];
    }

    uint8_t getCurrentX() const { 
        return currentColumn; // Column corresponds to X-coordinate
    }

    uint8_t getCurrentY() const { 
        return Global::MAZE_HEIGHT - 1 - currentRow; // Reverse the Y-axis
    }

    uint8_t getTargetX() const { 
        return targetColumn; // Column corresponds to X-coordinate
    }

    uint8_t getTargetY() const { 
        return Global::MAZE_HEIGHT - 1 - targetRow; // Reverse the Y-axis
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
        currentRow = Global::MAZE_HEIGHT - 1 - y; // Reverse the Y-axis
    }

    void setStart(uint8_t column, uint8_t row) {
        startColumn = column;
        startRow = Global::MAZE_HEIGHT - 1 - row; // Reverse the Y-axis
    }

    void setTarget(uint8_t x, uint8_t y) {
        targetColumn = x;
        targetRow = Global::MAZE_HEIGHT - 1 - y; // Reverse the Y-axis
    }
    
    void setStateAtCoordinates(uint8_t x, uint8_t y, Global::MazeCellState value) {        
        mazeState[Global::MAZE_HEIGHT - 1 - y][x] = value; // Reverse the Y-axis        
    }

    void setDirection(Global::Direction direction) {
        currentDirection = direction; // Set the current direction
    }
    
};

#endif