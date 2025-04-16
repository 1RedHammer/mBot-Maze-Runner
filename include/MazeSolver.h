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
        // TODO: Set all maze spaces to unexplored (0)
    }

    // Record current position as visited or blocked
    void markCurrentPosition(bool isBlocked = false) {
        // TODO: Mark the current position in mazeState
    }

    void updatePosition() {
        // TODO: Update currentX/Y based on deltaX/Y
    }


    // Position and direction getters
    uint8_t getCurrentX() const { return currentX; }
    uint8_t getCurrentY() const { return currentY; }
    int8_t getDeltaX() const { return deltaX; }
    int8_t getDeltaY() const { return deltaY; }
};

#endif