#pragma once

#include "Maze.h"
#include "MazeGenerator.h"
#include "Pathfinder.h"
#include "ResultView.h"

#include <memory>
#include <vector>

// Runs the main menu loop and sends each choice to the right feature.
class Menu
{
public:
    Menu();

    // Makes every random maze use this seed instead of a random one, so a
    // maze can be shared or a test can replay it exactly.
    void setFixedSeed(unsigned int seed) { m_fixedSeed = seed; m_hasFixedSeed = true; }

    void run();

private:
    unsigned int nextSeed() const;

    // Validated input that also stops the menu if input ends. Each returns
    // false only in that case, so callers just back out.
    bool askInt(const std::string& prompt, int min, int max, int& result);
    bool askKey(const std::string& prompt, const std::string& validKeys, const std::string& help, char& result);
    bool askYesNo(const std::string& prompt, bool& result);

    void showMainMenu() const;
    int readChoice() const;
    void handleChoice(int choice);

    // Play mode: pick a difficulty, then play random mazes against the enemy
    // until the user goes back to the main menu.
    void playMaze();
    // Easy/Medium/Hard picker shared by random mazes and Play mode. With
    // describeEnemy, each option also says how the enemy behaves.
    bool chooseDifficulty(Difficulty& difficulty, bool describeEnemy);
    void playOneGame(Difficulty difficulty);

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

    // Asks whether to show the explored-cells overlay. Returns false for "no"
    // or if input ended (which also stops the menu).
    bool askToShowExplored();

    Maze m_maze;
    MazeGenerator m_generator;

    // Every available algorithm, stored through the base class so the menu
    // can run any of them the same way.
    std::vector<std::unique_ptr<Pathfinder>> m_pathfinders;

    // Prints results. Declared after m_maze and m_pathfinders because it keeps
    // references to them, and members are built in declaration order.
    ResultView m_view;

    bool m_running = true;
    bool m_hasFixedSeed = false;
    unsigned int m_fixedSeed = 0;
};
