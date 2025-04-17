#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H

#include <stdint.h>
#include "GlobalConstants.h"

// Handles maze tracking and navigation logic

class MazeSolver {
private:
    // Maze dimensions    
    
    // Start and target positions in the maze
    uint8_t START_X = 0, START_Y = 0;
    uint8_t TARGET_X = 2, TARGET_Y = 2;
    

    // Maze state tracking:
    // 0 = unexplored
    // 1 = visited
    // 2 = wall/blocked
    uint8_t mazeState[GlobalConstants::MAZE_HEIGHT][GlobalConstants::MAZE_WIDTH];
    
    // Current position and facing direction
    uint8_t currentX, currentY;         // Position coordinates
    int8_t deltaX, deltaY;              // Direction vector (e.g. North = 0,1)

    int8_t mode = MODE_EXPLORING; // Default mode

public:
    // Initialize a new maze with all spaces unexplored
    MazeSolver() : currentX(START_X), currentY(START_Y), deltaX(0), deltaY(1) {

        
        for (uint8_t y = 0; y < GlobalConstants::MAZE_HEIGHT; ++y) {
            for (uint8_t x = 0; x < GlobalConstants::MAZE_WIDTH; ++x) {
                mazeState[y][x] = 0; // Unexplored
            }
        }

        mazeState[START_Y][START_X] = 1; // Mark start as visited
    }

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
        MODE_RETURNING_HOME = 1,  // Returning to the starting point
        MODE_RACING_TO_GOAL = 2   // Racing to the target or goal
    };

    enum State {
        STATE_UNEXPLORED = 0, // Unexplored cell
        STATE_VISITED = 1,    // Visited cell
        STATE_BLOCKED = 2     // Blocked cell
    };

    // Record current position as visited or blocked
    void markCurrentPosition(State state) {        
        mazeState[currentX][currentY] = state; // Mark current position
    }

    void updatePosition() {        
        currentX += deltaX;
        currentY += deltaY;
    }

    Action decideNextMove(){
        
        // TODO: decide where to go next
        int nextMovesImpossible[4] = {0, 0, 0, 0}; // N, E, S, W 
        // Check all 4 directions and update nextMoves based on mazeState
        if(currentX == 0){
            nextMovesImpossible[3] = 1;
        }
        if(currentX == GlobalConstants::MAZE_WIDTH - 1){
            nextMovesImpossible[1] = 1;
        }
        if(currentY == 0){
            nextMovesImpossible[2] = 1;
        }
        if(currentY == GlobalConstants::MAZE_HEIGHT - 1){
            nextMovesImpossible[0] = 1;
        }
        if(mazeState[currentX][currentY+1] == 2 || mazeState[currentX][currentY+1] == 1){
            nextMovesImpossible[0] = 1;
        }
        if(mazeState[currentX][currentY-1] == 2 || mazeState[currentX][currentY-1] == 1){
            nextMovesImpossible[2] = 1;
        }
        if(mazeState[currentX+1][currentY] == 2 || mazeState[currentX+1][currentY] == 1){
            nextMovesImpossible[1] = 1;
        }
        if(mazeState[currentX-1][currentY] == 2 || mazeState[currentX-1][currentY] == 1){
            nextMovesImpossible[3] = 1;
        }
      
        
        
        int beforeX = deltaX;
        int beforeY = deltaY;
        deltaX = beforeY;
        deltaY = -beforeX;
        //turn right
        if(deltaX == 0 && deltaY == 1){
            if(nextMovesImpossible[0] == 0){
                return TURN_RIGHT;
            }
        }
        if(deltaX == 1 && deltaY == 0){
            if(nextMovesImpossible[1] == 0){
                return TURN_RIGHT;
            }
        }
        if(deltaX == 0 && deltaY == -1){
            if(nextMovesImpossible[2] == 0){
                return TURN_RIGHT;
            }
        }
        if(deltaX == -1 && deltaY == 0){
            if(nextMovesImpossible[3] == 0){
                return TURN_RIGHT;
            }
        }
        //if can't turn right, go forward
        if(beforeX == 0 && beforeY == 1){
            if(nextMovesImpossible[0] == 0){
                deltaX = beforeX;
                deltaY = beforeY;
                return MOVE_FORWARD;
            }
        }
        if(beforeX == 1 && beforeY == 0){
            if(nextMovesImpossible[1] == 0){
                deltaX = beforeX;
                deltaY = beforeY;
                return MOVE_FORWARD;
            }
        }
        if(beforeX == 0 && beforeY == -1){
            if(nextMovesImpossible[2] == 0){
                deltaX = beforeX;
                deltaY = beforeY;
                return MOVE_FORWARD;
            }
        }
        if(beforeX == -1 && beforeY == 0){
            if(nextMovesImpossible[3] == 0){
                deltaX = beforeX;
                deltaY = beforeY;
                return MOVE_FORWARD;
            }
        }
        //if can't go forward, turn left
        deltaX = -beforeY;
        deltaY = beforeX;
        if(deltaX == 0 && deltaY == 1){
            if(nextMovesImpossible[0] == 0){
                return TURN_LEFT;
            }
        }
        if(deltaX == 1 && deltaY == 0){
            if(nextMovesImpossible[1] == 0){
                return TURN_LEFT;
            }
        }
        if(deltaX == 0 && deltaY == -1){
            if(nextMovesImpossible[2] == 0){
                return TURN_LEFT;
            }
        }
        if(deltaX == -1 && deltaY == 0){
            if(nextMovesImpossible[3] == 0){
                return TURN_LEFT;
            }
        }
        //if all else fails, turn around and go back
        return U_TURN;
    }

    

    //getters
    uint8_t getCurrentX() const { return currentX; }
    uint8_t getCurrentY() const { return currentY; }
    int8_t getDeltaX() const { return deltaX; }
    int8_t getDeltaY() const { return deltaY; }
    int8_t getTargetX() const { return TARGET_X; }
    int8_t getTargetY() const { return TARGET_Y; }
    int8_t getMode() const { return mode; }
    uint8_t getMazeCell(uint8_t x, uint8_t y) const {
        return mazeState[y][x];
    }

    //setters
    void setCurrent(uint8_t x, uint8_t y) {
        currentX = x;
        currentY = y;
    }

    void setDelta(int8_t x, int8_t y) {
        deltaX = x;
        deltaY = y;
    }

    void setStart(uint8_t x, uint8_t y) {
        START_X = x;
        START_Y = y;
    }

    void setTarget(uint8_t x, uint8_t y) {
        TARGET_X = x;
        TARGET_Y = y;
    }

    void setMode(int8_t newMode) { mode = newMode; }

    void setMazeCell(uint8_t x, uint8_t y, uint8_t value) {
        mazeState[x][y] = value;
    }
    
};

#endif