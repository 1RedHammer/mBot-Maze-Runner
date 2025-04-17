#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H

#include <stdint.h>
#include "GlobalConstants.h"

// Handles maze tracking and navigation logic

class MazeSolver {
private:
    // Maze dimensions    
    
    // Start and target positions in the maze
    static constexpr uint8_t START_X = 0, START_Y = 0;
    static constexpr uint8_t TARGET_X = 2, TARGET_Y = 2;
    

    // Maze state tracking:
    // 0 = unexplored
    // 1 = visited
    // 2 = wall/blocked
    uint8_t mazeState[GlobalConstants::MAZE_HEIGHT][GlobalConstants::MAZE_WIDTH];
    
    // Current position and facing direction
    uint8_t currentX, currentY;         // Position coordinates
    int8_t deltaX, deltaY;              // Direction vector (e.g. North = 0,1)

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

    // Record current position as visited or blocked
    void markCurrentPosition(bool isBlocked = false) {        
        if (isBlocked) {
            mazeState[currentY][currentX] = 2; // Mark as blocked
        } else {
            mazeState[currentY][currentX] = 1; // Mark as visited
        }
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
        if(currentX == GlobalConstants::MAZE_WIDTH-1){
            nextMovesImpossible[1] = 1;
        }
        if(currentY == 0){
            nextMovesImpossible[2] = 1;
        }
        if(currentY == GlobalConstants::MAZE_HEIGHT - 1){
            nextMovesImpossible[0] = 1;
        }
        if(mazeState[currentY][currentX+1] == 2 || mazeState[currentY][currentX+1] == 1){
            nextMovesImpossible[1] = 1;
        }
        if(mazeState[currentY][currentX-1] == 2 || mazeState[currentY][currentX-1] == 1){
            nextMovesImpossible[3] = 1;
        }
        if(mazeState[currentY+1][currentX] == 2 || mazeState[currentY+1][currentX] == 1){
            nextMovesImpossible[0] = 1;
        }
        if(mazeState[currentY-1][currentX] == 2 || mazeState[currentY-1][currentX] == 1){
            nextMovesImpossible[2] = 1;
        }
        
        return MOVE_FORWARD;

        /*
        
        int randomNumber = 0 ; //currently always go north

        switch (randomNumber) {
            case 0: // North
                deltaX = 0;
                deltaY = 1;
                break;
            case 1: // East
                deltaX = 1;
                deltaY = 0;
                break;
            case 2: // South
                deltaX = 0;
                deltaY = -1;
                break;
            case 3: // West
                deltaX = -1;
                deltaY = 0;
                break;
        }
                */

    }

    

    // Position and direction getters
    uint8_t getCurrentX() const { return currentX; }
    uint8_t getCurrentY() const { return currentY; }
    int8_t getDeltaX() const { return deltaX; }
    int8_t getDeltaY() const { return deltaY; }

    uint8_t getMazeCell(uint8_t x, uint8_t y) const {
        return mazeState[y][x];
    }
};

#endif