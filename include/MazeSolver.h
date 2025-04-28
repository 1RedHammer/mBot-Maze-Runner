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
    
    // Current position and facing direction
    uint8_t currentColumn, currentRow;         // Position coordinates
    int8_t deltaColumn, deltaRow;              // Direction vector (e.g. North = 0,1)

    
    //getters
    uint8_t getCurrentColumn() const { return currentColumn; }
    uint8_t getCurrentRow() const { return currentRow; }
    int8_t getDeltaColumn() const { return deltaColumn; }
    int8_t getDeltaRow() const { return deltaRow; }
    int8_t getTargetColumn() const { return targetColumn; }
    int8_t getTargetRow() const { return targetRow; }

public:

    enum Direction {
        NORTH = 0,
        EAST = 1,
        SOUTH = 2,
        WEST = 3,
        NONE = -1    
    };
   
    // Constructor with start and target positions
    MazeSolver(uint8_t startX, uint8_t startY, uint8_t targetX, uint8_t targetY, MazeSolver::Direction startDirection) 
        : startColumn(startX), startRow(GlobalConstants::MAZE_HEIGHT - 1 - startY),
          targetColumn(targetX), targetRow(GlobalConstants::MAZE_HEIGHT - 1 - targetY),
          currentColumn(startX), currentRow(GlobalConstants::MAZE_HEIGHT - 1 - startY)
        {

        // Initialize the maze state to unexplored (0)
        for (uint8_t row = 0; row < GlobalConstants::MAZE_HEIGHT; ++row) {
            for (uint8_t column = 0; column < GlobalConstants::MAZE_WIDTH; ++column) {
                mazeState[row][column] = 0; // Unexplored
            }
        }
        mazeState[startRow][startColumn] = 1; // Mark start as visited

        // Set the initial direction based on the start direction
        switch (startDirection) {
            case NORTH:
                deltaColumn = 0;
                deltaRow = -1;
                break;
            case EAST:
                deltaColumn = 1;
                deltaRow = 0;
                break;
            case SOUTH:
                deltaColumn = 0;
                deltaRow = 1;
                break;
            case WEST:
                deltaColumn = -1;
                deltaRow = 0;
                break;            
        }
        
    }

    enum State {
        STATE_UNEXPLORED = 0, // Unexplored cell
        STATE_VISITED = 1,    // Visited cell
        STATE_BLOCKED = 2     // Blocked cell
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
        MODE_RACING_TO_GOAL = 3   // Racing to the target or goal
    };

    Mode mode = MODE_EXPLORING; // Default mode


    // Record current position as visited or blocked
    void markCurrentPosition(State state) {        
        mazeState[currentRow][currentColumn] = state; // Mark current position
    }

    void updatePosition() {        
        currentColumn += deltaColumn;
        currentRow += deltaRow;
    }

    struct IntersectionResult {
        Action action;
        int8_t deltaX;
        int8_t deltaY;
        Mode mode;
        uint8_t currentX;
        uint8_t currentY;                
    };

    IntersectionResult processIntersection(State state) {
        updatePosition(); // Update the robot's position in the maze
        markCurrentPosition(state); // Mark current position as visited

        if (state == STATE_BLOCKED) {
            // Update deltaColumn and deltaRow to the opposite direction
            deltaColumn = -deltaColumn;
            deltaRow = -deltaRow;

            return {U_TURN, getDeltaX(), getDeltaY(), mode, getCurrentX(), getCurrentY()}; // If blocked, turn around
        }

        Action nextAction = decideNextMove();

        return {nextAction, getDeltaX(), getDeltaY(), mode, getCurrentX(), getCurrentY()};
    }

    Action decideNextMove() {

        Direction directionToGo = NONE; // Initialize direction to go
        // Keep track of the previous direction
        int beforeDeltaColumn = deltaColumn;
        int beforeDeltaRow = deltaRow;

        if (mode == MODE_EXPLORING || mode == MODE_EXPLORING_WITH_PATHFINDING) {
         
            mode = MODE_EXPLORING; // Default exploring mode

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



            // Try to turn right first        
            deltaRow = beforeDeltaColumn;
            deltaColumn = -beforeDeltaRow;

            if (deltaColumn == 0 && deltaRow == -1 && !directionRuledOut[NORTH]) {
                return TURN_RIGHT;
            }
            if (deltaColumn == 1 && deltaRow == 0 && !directionRuledOut[EAST]) {
                return TURN_RIGHT;
            }
            if (deltaColumn == 0 && deltaRow == 1 && !directionRuledOut[SOUTH]) {
                return TURN_RIGHT;
            }
            if (deltaColumn == -1 && deltaRow == 0 && !directionRuledOut[WEST]) {
                return TURN_RIGHT;
            }

            // Try to go forward
            deltaRow = beforeDeltaRow;
            deltaColumn = beforeDeltaColumn;
            if (deltaColumn == 0 && deltaRow == -1 && !directionRuledOut[NORTH]) {            
                return MOVE_FORWARD;
            }
            if (deltaColumn == 1 && deltaRow == 0 && !directionRuledOut[EAST]) {            
                return MOVE_FORWARD;
            }
            if (deltaColumn == 0 && deltaRow == 1 && !directionRuledOut[SOUTH]) {            
                return MOVE_FORWARD;
            }
            if (deltaColumn == -1 && deltaRow == 0 && !directionRuledOut[WEST]) {            
                return MOVE_FORWARD;
            }

            // Try to turn left        
            deltaRow = -beforeDeltaColumn;
            deltaColumn = beforeDeltaRow;
            if (deltaColumn == 0 && deltaRow == -1 && !directionRuledOut[NORTH]) {
                return TURN_LEFT;
            }
            if (deltaColumn == 1 && deltaRow == 0 && !directionRuledOut[EAST]) {
                return TURN_LEFT;
            }
            if (deltaColumn == 0 && deltaRow == 1 && !directionRuledOut[SOUTH]) {
                return TURN_LEFT;
            }
            if (deltaColumn == -1 && deltaRow == 0 && !directionRuledOut[WEST]) {
                return TURN_LEFT;
            }

            // No way to go, find the unexplored cell and go there
            bool stillUnexplored = false;
            int unexploredColumn = -1;
            int unexploredRow = -1;
            for (int row = 0; row < GlobalConstants::MAZE_HEIGHT; row++) {
                for (int column = 0; column < GlobalConstants::MAZE_WIDTH; column++) {
                    if (mazeState[row][column] == STATE_UNEXPLORED) {
                        stillUnexplored = true;
                        unexploredColumn = column;
                        unexploredRow = row;
                        break;
                    }
                }
            }

            if (stillUnexplored) {
                mode = MODE_EXPLORING_WITH_PATHFINDING; // Set the mode to exploring with pathfinding
                directionToGo = findPathTo(unexploredRow, unexploredColumn);
                return determineActionBasedOnDirection(directionToGo, beforeDeltaColumn, beforeDeltaRow);
            }
            else 
            {
                mode = MODE_RETURNING_HOME; // Set the mode to returning home        
            }

        } // End of exploring mode
        

        if (mode == MODE_RETURNING_HOME) 
        {
            directionToGo = findPathTo(startRow, startColumn); // Find path to the starting point
            if (directionToGo != NONE) {
                return determineActionBasedOnDirection(directionToGo, beforeDeltaColumn, beforeDeltaRow); // Move towards the starting point
            } else {
                mode = MODE_RACING_TO_GOAL; // Set the mode to racing to the goal
            }
        }

        if (mode == MODE_RACING_TO_GOAL) {
            directionToGo = findPathTo(targetRow, targetColumn); // Find path to the target
            if (directionToGo != NONE) {
                return determineActionBasedOnDirection(directionToGo, beforeDeltaColumn, beforeDeltaRow); // Move towards the target
            } 
        }        

        // No unexplored cells found, stop the robot, for now
        deltaColumn = 0;
        deltaRow = 0;
        return STOP;
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
                    pathLength[r][c] = -2; // Mark blocked cells
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

    Action determineActionBasedOnDirection(Direction directionToGo, int beforeDeltaColumn, int beforeDeltaRow) {
        if (directionToGo == NORTH) {
            deltaColumn = 0;
            deltaRow = -1;
        }
        if (directionToGo == EAST) {
            deltaColumn = 1;
            deltaRow = 0;
        }
        if (directionToGo == SOUTH) {
            deltaColumn = 0;
            deltaRow = 1;
        }
        if (directionToGo == WEST) {
            deltaColumn = -1;
            deltaRow = 0;
        }
        if (deltaColumn == beforeDeltaColumn && deltaRow == beforeDeltaRow) {
            return MOVE_FORWARD;
        }
        if (deltaColumn == -beforeDeltaColumn && deltaRow == -beforeDeltaRow) {
            return U_TURN;
        }
        if (deltaColumn == beforeDeltaRow && deltaRow == -beforeDeltaColumn) {
            return TURN_LEFT;
        }
        if (deltaColumn == -beforeDeltaRow && deltaRow == beforeDeltaColumn) {
            return TURN_RIGHT;
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

    int8_t getDeltaX() const { 
        return deltaColumn; // Column corresponds to X-coordinate
    }

    int8_t getDeltaY() const { 
        return -deltaRow; // Return negative deltaRow to account for reversed Y-axis
    }

    //setters
    void setCurrent(uint8_t x, uint8_t y) {
        currentColumn = x;
        currentRow = GlobalConstants::MAZE_HEIGHT - 1 - y; // Reverse the Y-axis
    }

    void setDelta(int8_t x, int8_t y) {
        deltaColumn = x;
        deltaRow = -y; // Reverse the Y-axis
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
    
};

#endif