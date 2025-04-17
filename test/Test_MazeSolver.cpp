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
const std::string mazePic[GlobalConstants::MAZE_HEIGHT] = {
    ".X.$",
    "....",
    "....",    
    "A..."
};


struct drawMazeChars
{
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


void initMaze(MazeSolver *sv)
{

    //iterate through the mazePic and set the maze state
    for (uint8_t y = 0; y < GlobalConstants::MAZE_HEIGHT; ++y)
    {
        for (uint8_t x = 0; x < GlobalConstants::MAZE_WIDTH; ++x)
        {            
            //pic[0][0] = 0; // Unexplored

        

            if (mazePic[y][x] == drawMazeChars::CHAR_BLOCKED)
            {
                sv->setMazeCell(x, GlobalConstants::MAZE_HEIGHT - 1 - y, MazeSolver::State::STATE_BLOCKED); // Blocked cell
            }
            else if (mazePic[y][x] == drawMazeChars::CHAR_VISITED)
            {
                sv->setMazeCell(x, GlobalConstants::MAZE_HEIGHT - 1 - y, MazeSolver::State::STATE_VISITED); // Visited cell
            }
            else if (mazePic[y][x] == drawMazeChars::CHAR_UNEXPLORED)
            {
                sv->setMazeCell(x, GlobalConstants::MAZE_HEIGHT - 1 - y, MazeSolver::State::STATE_UNEXPLORED); // Unexplored cell
            }
            else if (mazePic[y][x] == drawMazeChars::CHAR_Target)
            {
                sv->setTarget(x, GlobalConstants::MAZE_HEIGHT - 1 - y); // Set target cell
            }
            else if (mazePic[y][x] == drawMazeChars::CHAR_RUNNER)
            {
                sv->setMazeCell(x, GlobalConstants::MAZE_HEIGHT - 1 - y, MazeSolver::State::STATE_VISITED);
                sv->setCurrent(x, GlobalConstants::MAZE_HEIGHT - 1 - y); // Set current position
            }
            else if (mazePic[y][x] == drawMazeChars::CHAR_NORTH)
            {
                sv->setMazeCell(x, GlobalConstants::MAZE_HEIGHT - 1 - y, MazeSolver::State::STATE_VISITED);
                sv->setDelta(0, 1); // Set delta to north
            }
            else if (mazePic[y][x] == drawMazeChars::CHAR_EAST)
            {
                sv->setMazeCell(x, GlobalConstants::MAZE_HEIGHT - 1 - y, MazeSolver::State::STATE_VISITED);
                sv->setDelta(1, 0); // Set delta to east
            }
            else if (mazePic[y][x] == drawMazeChars::CHAR_SOUTH)
            {
                sv->setMazeCell(x, GlobalConstants::MAZE_HEIGHT - 1 - y, MazeSolver::State::STATE_VISITED);
                sv->setDelta(0, -1); // Set delta to south
            }
            else if (mazePic[y][x] == drawMazeChars::CHAR_WEST)
            {
                sv->setMazeCell(x, GlobalConstants::MAZE_HEIGHT - 1 - y, MazeSolver::State::STATE_VISITED);
                sv->setDelta(-1, 0); // Set delta to west
            }
             
        }
    }
        
}

void setUp(void)
{
    // Initialize the maze solver with a 3x3 maze
    solver = new MazeSolver(); // Start at (0, 0)

    initMaze(solver); // Set the maze state
}

void tearDown(void)
{
    delete solver;
}

void drawMaze(MazeSolver *sv)
{

    // get each cell in the maze and put it into local array
    uint8_t maze[GlobalConstants::MAZE_WIDTH][GlobalConstants::MAZE_HEIGHT]; // Local maze array
    for (uint8_t y = 0; y < GlobalConstants::MAZE_HEIGHT; ++y)
    {
        for (uint8_t x = 0; x < GlobalConstants::MAZE_WIDTH; ++x)
        {
            maze[y][x] = sv->getMazeCell(y, x); // Get the state of each cell
        }
    }

    // get current position and delta
    uint8_t currentX = sv->getCurrentX();
    uint8_t currentY = sv->getCurrentY();
    int8_t deltaX = sv->getDeltaX();
    int8_t deltaY = sv->getDeltaY();
    int8_t targetX = sv->getTargetX();
    int8_t targetY = sv->getTargetY();

    // print maze to console

    // draw border
    printf("\n");

    for (int8_t y = GlobalConstants::MAZE_HEIGHT - 1; y >= 0; --y)
    { // Reverse the y-axis for intuitive display
        for (uint8_t x = 0; x < GlobalConstants::MAZE_WIDTH; ++x)
        {
            if (x == currentX && y == currentY)
            {
                char runnerSymbol = drawMazeChars::CHAR_RUNNER; // Runner symbol
                if (deltaX == 0 && deltaY == 1)
                {
                    runnerSymbol = drawMazeChars::CHAR_NORTH; // North
                }
                else if (deltaX == 1 && deltaY == 0)
                {
                    runnerSymbol = drawMazeChars::CHAR_EAST; // East
                }
                else if (deltaX == 0 && deltaY == -1)
                {
                    runnerSymbol = drawMazeChars::CHAR_WEST; // South
                }
                else if (deltaX == -1 && deltaY == 0)
                {
                    runnerSymbol = drawMazeChars::CHAR_SOUTH; // West
                }
                printf("%c ", runnerSymbol); // Print the runner with the arrow
            }
            else if (x == targetX && y == targetY)
            {
                printf("%c ", drawMazeChars::CHAR_Target); // Target cell
            }
            else if (maze[y][x] == 0)
            {
                printf("%c ", drawMazeChars::CHAR_UNEXPLORED); // Unexplored cell
            }
            else if (maze[y][x] == 1)
            {
                printf("%c ", drawMazeChars::CHAR_VISITED); // Visited cell
            }
            else if (maze[y][x] == 2)
            {
                printf("%c ", drawMazeChars::CHAR_BLOCKED); // Blocked cell
            }                      
            else
            {
                printf("%c ", "X "); // Invalid cell state
            }
        }
        printf("\n");
    }

    printf("\n");

    fflush(stdout); // Flush after each line
}

void test_explore(void)
{
    drawMaze(solver); // Draw the maze before exploring
    int limiter = 0; // Limiter to prevent infinite loop
    MazeSolver::Action action ; // Initial action

    do{
        solver->markCurrentPosition(MazeSolver::State::STATE_VISITED); // Mark current position as visited
        solver->updatePosition(); // Update the current position based on delta
        limiter++;

        printf("\nSTEP %d", limiter); // Print the step number
        

        action = solver->decideNextMove(); // Decide the next move
        printf(" -- Action: %d\n", action); // Print the action

        drawMaze(solver); // Draw the maze after exploring

    }
    while (action != MazeSolver::Action::STOP  && limiter < 5); // Stop if the target is reached or if the loop limit is reached
    
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_explore);

    return UNITY_END();
}
