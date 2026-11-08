#pragma once

#include "Maze.h"
#include "MazeGenerator.h"
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

    // Each returns true if it showed results (false if the user went back).
    bool solveMaze();
    bool compareAlgorithms();

    // Lets the user pick a built-in or random maze and loads it into m_maze.
    // Returns false if they went back or input ended.
    bool chooseMaze();

    // Asks for a difficulty, then generates and loads a new random maze.
    bool chooseRandomMaze();

    bool loadLayout(const std::vector<std::string>& layout);

    // Lets the user pick one of m_pathfinders. Returns nullptr if they went
    // back or input ended.
    Pathfinder* choosePathfinder();

    void showResult(const Pathfinder& pathfinder, const PathResult& result) const;

    // Compare mode output, split into steps.
    void printSideBySide(const std::vector<PathResult>& results, bool showExplored) const;
    void printStatsTable(const std::vector<PathResult>& results) const;
    void printComparisonSummary(const std::vector<PathResult>& results) const;

    void printLegend(bool withExplored) const;

    // Asks whether to show the explored-cells overlay. Returns false for "no"
    // or if input ended (which also stops the menu).
    bool askToShowExplored();

    Maze m_maze;
    MazeGenerator m_generator;

    // Every available algorithm, stored through the base class so the menu
    // can run any of them the same way.
    std::vector<std::unique_ptr<Pathfinder>> m_pathfinders;

    bool m_running = true;
};
