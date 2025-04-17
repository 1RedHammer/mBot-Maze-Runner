#include "unity.h"
#include "../include/MazeSolver.h"

#include <stdio.h>

/* had to install g++, and native test mode */

MazeSolver* solver;

void setUp(void) { 
     // Initialize the maze solver with a 3x3 maze
     solver = new MazeSolver(); // Start at (0, 0)
}

void tearDown(void) { 
    delete solver;
}


void test_draw_maze(void) {

    //get each cell in the maze and put it into local array
    uint8_t maze[GlobalConstants::MAZE_WIDTH][GlobalConstants::MAZE_HEIGHT]; // Local maze array
    for (uint8_t y = 0; y < GlobalConstants::MAZE_HEIGHT; ++y) {
        for (uint8_t x = 0; x < GlobalConstants::MAZE_WIDTH; ++x) {
            maze[y][x] = solver->getMazeCell(y, x); // Get the state of each cell
        }
    }

    //get current position and delta
    uint8_t currentX = solver->getCurrentX();
    uint8_t currentY = solver->getCurrentY();
    int8_t deltaX = solver->getDeltaX();
    int8_t deltaY = solver->getDeltaY();

    //print maze to console

    //draw border
    printf("\n");

    for (int8_t y = GlobalConstants::MAZE_HEIGHT - 1; y >= 0; --y) { // Reverse the y-axis for intuitive display
        for (uint8_t x = 0; x < GlobalConstants::MAZE_WIDTH; ++x) {
            if (x == currentX && y == currentY) {
                char runnerSymbol = 'R'; // Default symbol
                if (deltaX == 0 && deltaY == 1) {
                    runnerSymbol = 'A'; // North
                } else if (deltaX == 1 && deltaY == 0) {
                    runnerSymbol = '>'; // East
                } else if (deltaX == 0 && deltaY == -1) {
                    runnerSymbol = 'V'; // South
                } else if (deltaX == -1 && deltaY == 0) {
                    runnerSymbol = '<'; // West
                }
                printf("%c ", runnerSymbol); // Print the runner with the arrow    
            } else if (maze[y][x] == 1) {
                printf(". "); // Visited cell
            } else if (maze[y][x] == 2) {
                printf("# "); // Blocked cell
            } else {
                printf("_ "); // Unexplored cell
            }
        }
        printf("\n");
    }   

    printf("\n");
    
    fflush(stdout); // Flush after each line
     
}


void test_simple_path(void) {    
    
    test_draw_maze(); // Print the initial maze

    solver-> updatePosition(); // Move to (0, 1)
    solver-> markCurrentPosition(); // Mark (0, 1) as visited    
    
    test_draw_maze();

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, solver->getCurrentX(), "X position should be 0");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, solver->getCurrentY(), "Y position should be 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, solver->getCurrentY(), "Y position should be 1");

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, solver->getDeltaX(), "Delta X should be 0"); 
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, solver->getDeltaY(), "Delta Y should be 1"); // Facing north


    solver->updatePosition(); // Move to (0, 2)
    solver->markCurrentPosition(); // Mark (0, 2) as visited

    test_draw_maze();
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, solver->getCurrentX(), "X position should be 0");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, solver->getCurrentY(), "Y position should be 2");

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, solver->getDeltaX(), "Delta X should be 0"); 
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, solver->getDeltaY(), "Delta Y should be 2"); // Facing north
   
}




int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_simple_path);
    //RUN_TEST(test_draw_maze);
    return UNITY_END();
}
