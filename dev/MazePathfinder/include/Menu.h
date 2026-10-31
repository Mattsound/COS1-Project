#pragma once

#include "Maze.h"
#include "Pathfinder.h"

// Runs the main menu loop and sends each choice to the right feature.
class Menu
{
public:
    void run();

private:
    void showMainMenu() const;
    int readChoice() const;
    void handleChoice(int choice);

    void solveMaze();

    // Lets the user pick a built-in maze and loads it into m_maze.
    // Returns false if they went back or input ended.
    bool chooseMaze();

    void showResult(const Pathfinder& pathfinder, const PathResult& result) const;

    Maze m_maze;
    bool m_running = true;
};
