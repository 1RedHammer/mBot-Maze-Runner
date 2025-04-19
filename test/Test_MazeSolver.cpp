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
/*
mazePic[4] = {
    "...$",
    "....",
    "....",    
    "A..."
};
*/
mazePic[3] = {
    "..$",
    "...",
    "A.."
};


struct drawMazeChars
{
    // note - better stick with common characters otherwise printf will work weirdly

    static const char CHAR_BLOCKED = 'X';    // Blocked cell
    static const char CHAR_VISITED = 'o';    // Visited cell
    static const char CHAR_UNEXPLORED = '.'; // Unexplored cell
    static const char CHAR_RUNNER = 'R';     // Runner symbol
    static const char CHAR_NORTH = 'A';      // North
    static const char CHAR_EAST = '>';       // East
    static const char CHAR_SOUTH = 'V';      // South
    static const char CHAR_WEST = '<';       // West
    static const char CHAR_Target = '$';     // Target
};

uint8_t toArrayIndexY(uint8_t y) {
    return GlobalConstants::MAZE_HEIGHT - 1 - y; // Reverse y-axis
}

uint8_t toMazeCoordinatesY(uint8_t indexY) {
    return GlobalConstants::MAZE_HEIGHT - 1 - indexY; // Reverse y-axis
}

void initMaze(MazeSolver *sv)
{
    //iterate through the mazePic and set the maze state
    for (uint8_t y = 0; y < GlobalConstants::MAZE_HEIGHT; ++y)
    {
        for (uint8_t x = 0; x < GlobalConstants::MAZE_WIDTH; ++x)
        {                        
            uint8_t picCell = mazePic[y][x]; // Get the cell character
            uint8_t indexY = toArrayIndexY(y); // Reverse the y-axis for intuitive display            
          
            // Set the maze state based on the character
            if (picCell == drawMazeChars::CHAR_BLOCKED)
            {
                sv->setMazeCell(x, indexY, MazeSolver::State::STATE_BLOCKED); // Blocked cell
            }
            else if (picCell == drawMazeChars::CHAR_VISITED)
            {
                sv->setMazeCell(x, indexY, MazeSolver::State::STATE_VISITED); // Visited cell
            }
            else if (picCell == drawMazeChars::CHAR_UNEXPLORED)
            {
                sv->setMazeCell(x, indexY, MazeSolver::State::STATE_UNEXPLORED); // Unexplored cell
            }
            else if (picCell == drawMazeChars::CHAR_Target)
            {
                sv->setTarget(x, indexY); // Set target cell
            }
            else if (picCell == drawMazeChars::CHAR_RUNNER)
            {
                sv->setMazeCell(x, indexY, MazeSolver::State::STATE_VISITED);
                sv->setCurrent(x, indexY); // Set current position
            }
            else if (picCell == drawMazeChars::CHAR_NORTH)
            {
                sv->setMazeCell(x, indexY, MazeSolver::State::STATE_VISITED);
                sv->setDelta(0, 1); // Set delta to north
            }
            else if (picCell == drawMazeChars::CHAR_EAST)
            {
                sv->setMazeCell(x, indexY, MazeSolver::State::STATE_VISITED);
                sv->setDelta(1, 0); // Set delta to east
            }
            else if (picCell == drawMazeChars::CHAR_SOUTH)
            {
                sv->setMazeCell(x, indexY, MazeSolver::State::STATE_VISITED);
                sv->setDelta(0, -1); // Set delta to south
            }
            else if (picCell == drawMazeChars::CHAR_WEST)
            {
                sv->setMazeCell(x, indexY, MazeSolver::State::STATE_VISITED);
                sv->setDelta(-1, 0); // Set delta to west
            }             
        }
    }
        
}

void setUp(void)
{
    // Initialize the maze solver with a 3x3 maze
    solver = new MazeSolver(); // Start at (0, 0)

    //initMaze(solver); // Set the maze state
}

void tearDown(void)
{
    delete solver;
}

char getCellSymbol(uint8_t x, uint8_t y, MazeSolver *sv) {
    if (x == sv->getCurrentX() && y == sv->getCurrentY()) {
        if (sv->getDeltaX() == 0 && sv->getDeltaY() == 1) return drawMazeChars::CHAR_NORTH;
        if (sv->getDeltaX() == 1 && sv->getDeltaY() == 0) return drawMazeChars::CHAR_EAST;
        if (sv->getDeltaX() == 0 && sv->getDeltaY() == -1) return drawMazeChars::CHAR_SOUTH;
        if (sv->getDeltaX() == -1 && sv->getDeltaY() == 0) return drawMazeChars::CHAR_WEST;
        return drawMazeChars::CHAR_RUNNER;
    }
    if (x == sv->getTargetX() && y == sv->getTargetY()) return drawMazeChars::CHAR_Target;
    uint8_t state = sv->getMazeCell(y, x);
    if (state == 0) return drawMazeChars::CHAR_UNEXPLORED;
    if (state == 1) return drawMazeChars::CHAR_VISITED;
    if (state == 2) return drawMazeChars::CHAR_BLOCKED;
    return 'X'; // Invalid state
}

void drawMaze(MazeSolver *sv) {
    printf("\n");
    for (int8_t y = GlobalConstants::MAZE_HEIGHT - 1; y >= 0; --y) {
        for (uint8_t x = 0; x < GlobalConstants::MAZE_WIDTH; ++x) {
            printf("%c ", getCellSymbol(x, y, sv));
        }
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
}

void test_explore(void)
{
    drawMaze(solver); // Draw the maze before exploring
    int limiter = 0; 
    int maxSteps = 16; // Maximum steps to explore the maze
    MazeSolver::Action action ; // Initial action

    do{
        
        action = solver->processIntersection(MazeSolver::State::STATE_VISITED); // Process the intersection

        limiter++;
        printf("\nSTEP %d", limiter);
        printf(" -- Action: %d\n", action);

        drawMaze(solver);

    }
    while (action != MazeSolver::Action::STOP && limiter < maxSteps); // Stop if the target is reached or if the loop limit is reached
    
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_explore);

    return UNITY_END();
}
