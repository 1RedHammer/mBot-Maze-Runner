#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H

#include <stdint.h>

// Handles maze tracking and navigation logic

class MazeSolver {
private:
    // Maze dimensions
    static constexpr uint8_t MAZE_WIDTH = 3;
    static constexpr uint8_t MAZE_HEIGHT = 3;
    
    // Start and target positions in the maze
    static constexpr uint8_t START_X = 0, START_Y = 0;
    static constexpr uint8_t TARGET_X = 2, TARGET_Y = 2;

    // Maze state tracking:
    // 0 = unexplored
    // 1 = visited
    // 2 = wall/blocked
    uint8_t mazeState[MAZE_HEIGHT][MAZE_WIDTH];
    
    // Current position and facing direction
    uint8_t currentX, currentY;         // Position coordinates
    int8_t deltaX, deltaY;              // Direction vector (e.g. North = 0,1)

public:
    // Initialize a new maze with all spaces unexplored
    MazeSolver() : currentX(START_X), currentY(START_Y), deltaX(0), deltaY(1) {
        for (uint8_t y = 0; y < MAZE_HEIGHT; ++y) {
            for (uint8_t x = 0; x < MAZE_WIDTH; ++x) {
                mazeState[y][x] = 0; // Unexplored
            }
        }
        mazeState[START_Y][START_X] = 1; // Mark start as visited
    }

    // Record current position as visited or blocked
    void markCurrentPosition(bool isBlocked = false) {
        // TODO: Mark the current position in mazeState
    }

    void updatePosition() {
        // TODO: Update currentX/Y based on deltaX/Y done yay
        currentX += deltaX;
        currentY += deltaY;
    }

    void decideNextMove(){




        // TODO: decide where to go next
        int nextMovesImpossible[4] = {0, 0, 0, 0}; // N, E, S, W 
        // Check all 4 directions and update nextMoves based on mazeState
        if(currentX == 0){
            nextMovesImpossible[3] = 1;
        }
        if(currentX == MAZE_WIDTH-1){
            nextMovesImpossible[1] = 1;
        }
        if(currentY == 0){
            nextMovesImpossible[2] = 1;
        }
        if(currentY == MAZE_HEIGHT - 1){
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

    }


    // Position and direction getters
    uint8_t getCurrentX() const { return currentX; }
    uint8_t getCurrentY() const { return currentY; }
    int8_t getDeltaX() const { return deltaX; }
    int8_t getDeltaY() const { return deltaY; }
};

#endif