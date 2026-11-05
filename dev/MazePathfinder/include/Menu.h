#pragma once

#include "Maze.h"
#include "Pathfinder.h"

#include <memory>
#include <vector>

// Runs the main menu loop and sends each choice to the right feature.
class Menu
{
public:
    Menu();

    void run();

private:
    void showMainMenu() const;
    int readChoice() const;
    void handleChoice(int choice);

    void solveMaze();
    void compareAlgorithms();

    // Lets the user pick a built-in maze and loads it into m_maze.
    // Returns false if they went back or input ended.
    bool chooseMaze();

    // Lets the user pick one of m_pathfinders. Returns nullptr if they went
    // back or input ended.
    Pathfinder* choosePathfinder();

    void showResult(const Pathfinder& pathfinder, const PathResult& result) const;

    Maze m_maze;

    // Every available algorithm, stored through the base class so the menu
    // can run any of them the same way.
    std::vector<std::unique_ptr<Pathfinder>> m_pathfinders;

    bool m_running = true;
};
