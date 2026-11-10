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

    // Play mode: the user walks from S to G with W/A/S/D.
    void playMaze();

    enum class Mode
    {
        Solve,
        Compare
    };

    // Picks a maze, runs the chosen mode on it, then keeps offering "What
    // next?" (same maze another way, or a new maze) until the user returns
    // to the main menu.
    void runMazeSession(Mode mode);

    // Run on the maze already in m_maze. solveCurrentMaze() returns false if
    // the user went back from the algorithm picker instead.
    bool solveCurrentMaze();
    void compareCurrentMaze();

    // Shows the "What next?" options and returns the choice (0 = main menu,
    // which is also returned if input ended).
    int askWhatNext();

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
