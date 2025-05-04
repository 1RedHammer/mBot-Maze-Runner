#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

namespace Global {
    constexpr uint8_t MAZE_HEIGHT = 4; // Example value
    constexpr uint8_t MAZE_WIDTH = 4;  // Example value

    enum Direction {
        NORTH = 0,
        EAST = 1,
        SOUTH = 2,
        WEST = 3,
        NO_DIRECTION = -100
    };

    enum MazeCellState {
        STATE_UNEXPLORED = 0, // Unexplored cell
        STATE_VISITED = 1,    // Visited cell
        STATE_BLOCKED = 2,
        STATE_UNREACHABLE = 3 // Blocked cell
    };

    enum RobotMovement {
        STOP = 0,
        MOVE_FORWARD = 1,
        TURN_LEFT = 2,
        TURN_RIGHT = 3,
        U_TURN = 4,
        MOVE_BACKWARD = 5
    };

    enum Mode {
        MODE_EXPLORING = 0,                // Exploring and mapping the maze
        MODE_EXPLORING_WITH_PATHFINDING = 1, // Exploring with pathfinding
        MODE_RETURNING_HOME = 2,           // Returning to the starting point
        MODE_RACING_TO_TARGET = 3,         // Racing to the target
        MODE_RACE_COMPLETED = 4,
        MODE_TARGET_UNREACHABLE = 5
    };
}



#endif