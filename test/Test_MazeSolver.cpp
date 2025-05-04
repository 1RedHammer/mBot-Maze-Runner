#include "unity.h"
#include "../include/MazeSolver.h"
#include <stdio.h>
#include <string>

/* had to install g++ from https://www.msys2.org/
    1. download
    2. Open MSYS2 MSYS terminal
    3. pacman -Syu       # Update system (may ask to restart)
       pacman -S mingw-w64-x86_64-gcc
    4. Open MSYS2 MinGW 64-bit terminal.
    5. g++ --version
    
	then configure env variables to add the path of gcc (C:\msys64\mingw64\bin)

    in PIO console, run "pio test -e native -v" to run the test locallly 
*/

MazeSolver *solver;

// a maze picture to be used for testing
const std::string 


mazePic[4] = {
    "...$",
    "...?",
    ".?..",    
    "A..."
};
/*
mazePic[3] = {
    "..$",
    "...",
    "A.."
};*/


struct drawMazeChars
{
    // note - better stick with common characters otherwise printf will work weirdly
    static const char CHAR_BLOCKED = 'X';    // Blocked cell known to the solver
    static const char CHAR_BLOCKED_Unkown = '?';  // Blocked cell unknown to the solver
    static const char CHAR_VISITED = 'o';    // Visited cell
    static const char CHAR_UNEXPLORED = '.'; // Unexplored cell
    static const char CHAR_UNREACHABLE = '%'; // Unreachable cell
    static const char CHAR_RUNNER = '#';     // Runner symbol
    static const char CHAR_NORTH = 'A';      // North
    static const char CHAR_EAST = '>';       // East
    static const char CHAR_SOUTH = 'V';      // South
    static const char CHAR_WEST = '<';       // West
    static const char CHAR_Target = '$';     // Target
};

uint8_t toRowIndex(uint8_t y) {
    return Global::MAZE_HEIGHT - 1 - y; // Reverse y-axis
}

uint8_t toMazeCoordinatesY(uint8_t indexY) {
    return Global::MAZE_HEIGHT - 1 - indexY; // Reverse y-axis
}

void initMaze(MazeSolver *sv)
{
    //iterate through the mazePic and set the maze state
    for (uint8_t y = 0; y < Global::MAZE_HEIGHT; ++y)
    {
        for (uint8_t x = 0; x < Global::MAZE_WIDTH; ++x)
        {                        
            uint8_t picCell = mazePic[y][x]; // Get the cell character
            uint8_t indexY = toRowIndex(y); // Reverse the y-axis for intuitive display            
          
            // Set the maze state based on the character
            if (picCell == drawMazeChars::CHAR_BLOCKED)
            {
                sv->setStateAtCoordinates(x, indexY, Global::STATE_BLOCKED); // Blocked cell
            }
            else if (picCell == drawMazeChars::CHAR_VISITED)
            {
                sv->setStateAtCoordinates(x, indexY, Global::STATE_VISITED); // Visited cell
            }
            else if (picCell == drawMazeChars::CHAR_UNEXPLORED)
            {
                sv->setStateAtCoordinates(x, indexY, Global::STATE_UNEXPLORED); // Unexplored cell
            }
            else if (picCell == drawMazeChars::CHAR_Target)
            {
                sv->setTarget(x, indexY); // Set target cell
            }
            else if (picCell == drawMazeChars::CHAR_RUNNER)
            {
                sv->setStateAtCoordinates(x, indexY, Global::STATE_VISITED);
                sv->setCurrent(x, indexY); // Set current position
            }
            else if (picCell == drawMazeChars::CHAR_NORTH)
            {
                sv->setStateAtCoordinates(x, indexY, Global::STATE_VISITED);
                sv->setDirection(Global::NORTH); // Set direction to north
            }
            else if (picCell == drawMazeChars::CHAR_EAST)
            {
                sv->setStateAtCoordinates(x, indexY, Global::STATE_VISITED);
                sv->setDirection(Global::EAST); // Set direction to east
             
            }
            else if (picCell == drawMazeChars::CHAR_SOUTH)
            {
                sv->setStateAtCoordinates(x, indexY, Global::STATE_VISITED);
                sv->setDirection(Global::SOUTH); // Set direction to south
            }
            else if (picCell == drawMazeChars::CHAR_WEST)
            {
                sv->setStateAtCoordinates(x, indexY, Global::STATE_VISITED);
                sv->setDirection(Global::WEST); // Set direction to west
            }
            else {
                // Default case for unexplored cells (this include the '?')
                sv->setStateAtCoordinates(x, indexY, Global::STATE_UNEXPLORED);
            }
        }
    }
        
}

void setUp(void)
{
    // Initialize the maze
    solver = new MazeSolver(0, 0, Global::MAZE_WIDTH - 1, Global::MAZE_HEIGHT - 1, Global::NORTH);

    //initMaze(solver); // Set the maze state
}

void tearDown(void)
{
    delete solver;
}

char getCellSymbol(uint8_t x, uint8_t y, MazeSolver *sv) {

    
    //printf ("Current (x,y) (%d, %d)(%d, %d)\n", x,y, sv->getCurrentX(), sv->getCurrentY());

    if (x == sv->getCurrentX() && y == sv->getCurrentY()) {
        if (sv->getOffsetX() == 0 && sv->getOffsetY() == 1) return drawMazeChars::CHAR_NORTH;
        if (sv->getOffsetX() == 1 && sv->getOffsetY() == 0) return drawMazeChars::CHAR_EAST;
        if (sv->getOffsetX() == 0 && sv->getOffsetY() == -1) return drawMazeChars::CHAR_SOUTH;
        if (sv->getOffsetX() == -1 && sv->getOffsetY() == 0) return drawMazeChars::CHAR_WEST;

        
        //printf ("Current (x,y) (%d, %d) delta (x,y) (%d, %d)\n", x,y, sv->getDeltaX(), sv->getDeltaY());

        return drawMazeChars::CHAR_RUNNER;
    }
    if (x == sv->getTargetX() && y == sv->getTargetY()) return drawMazeChars::CHAR_Target;

    uint8_t state = sv->getStateAtCoordinates(x, y);
    if (state == 0) return drawMazeChars::CHAR_UNEXPLORED;
    if (state == 1) return drawMazeChars::CHAR_VISITED;
    if (state == 2) return drawMazeChars::CHAR_BLOCKED;
    if (state == 3) return drawMazeChars::CHAR_UNREACHABLE;
    return '!'; // Invalid state
}

void drawMaze(MazeSolver *sv) {
    printf("\n");
    for (int8_t y = Global::MAZE_HEIGHT - 1; y >= 0; --y) {
        printf("  ");
        for (uint8_t x = 0; x < Global::MAZE_WIDTH; ++x) {
            printf("%c ", getCellSymbol(x, y, sv));
        }
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
}

const char* actionToString(Global::RobotMovement action) {
    switch (action) {
        case Global::RobotMovement::STOP: return "STOP";
        case Global::RobotMovement::MOVE_FORWARD: return "MOVE_FORWARD";
        case Global::RobotMovement::TURN_LEFT: return "TURN_LEFT";
        case Global::RobotMovement::TURN_RIGHT: return "TURN_RIGHT";
        case Global::RobotMovement::U_TURN: return "U_TURN";
        case Global::RobotMovement::MOVE_BACKWARD: return "MOVE_BACKWARD";
        default: return "UNKNOWN";
    }
}

const char* modeToString(Global::Mode mode) {
    switch (mode) {
        case Global::MODE_EXPLORING: return "EXPLORING";
        case Global::MODE_EXPLORING_WITH_PATHFINDING: return "EXPLORING_WITH_PATHFINDING";
        case Global::MODE_RETURNING_HOME: return "RETURNING_HOME";
        case Global::MODE_RACING_TO_TARGET: return "RACING_TO_TARGET";
        case Global::MODE_RACE_COMPLETED: return "RACE_COMPLETED";
        case Global::MODE_TARGET_UNREACHABLE: return "TARGET_UNREACHABLE";
        default: return "UNKNOWN";   
    }
}

const char* directionToString(Global::Direction direction) {
    switch (direction) {
        case Global::NORTH: return "NORTH";
        case Global::EAST: return "EAST";
        case Global::SOUTH: return "SOUTH";
        case Global::WEST: return "WEST";
        case Global::NO_DIRECTION: return "NO_DIRECTION";
        default: return "UNKNOWN";
    }
}

void test_explore(void)
{
    
    printf("Starting state of the maze:\n");
    drawMaze(solver); // Draw the maze before exploring
    int limiter = 0; 
    int maxSteps = 40; // Maximum steps to explore the maze
    MazeSolver::IntersectionResult result ;

    do{        
        printf("STEP %d\n", ++limiter);

        //get the next cell to explore
        printf("Current coordinates: (%d, %d)\n", solver->getCurrentX(), solver->getCurrentY());

        uint8_t nextX = solver->getCurrentX() + solver->getOffsetX();
        uint8_t nextY = solver->getCurrentY() + solver->getOffsetY(); 
        printf("Next coordinates: (%d, %d). Go ...\n", nextX, nextY);
                
        //use mazePic to check if the position is blocked, be sure to transform the coordinates to the mazePic indexes                
        if (mazePic[toRowIndex(nextY)][nextX] == drawMazeChars::CHAR_BLOCKED_Unkown
        || mazePic[toRowIndex(nextY)][nextX] == drawMazeChars::CHAR_BLOCKED) {        
            printf("Next is found to be blocked\n");
            result = solver->processIntersection(Global::STATE_BLOCKED); // Blocked cell
        }
        else {
            printf("Next is reached\n");
            result = solver->processIntersection(Global::STATE_VISITED); // Process the intersection            
        }

        printf("Solver Result = Action: %s, Mode: %s, Direction: %s  \n", actionToString(result.action), modeToString(result.mode), directionToString(result.direction)); // print result
        drawMaze(solver);
    }    
    while (result.action != Global::RobotMovement::STOP && limiter < maxSteps); // Stop if the target is reached or if the loop limit is reached
    
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_explore);

    return UNITY_END();
}
