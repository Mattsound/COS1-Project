#include "Menu.h"

#include "BfsPathfinder.h"
#include "InputHelper.h"
#include "MazePresets.h"

#include <iostream>

namespace
{
    enum MenuOption
    {
        SolveMaze = 1,
        PlayMode = 2,
        CompareAlgorithms = 3,
        Exit = 4
    };
}

void Menu::run()
{
    while (m_running)
    {
        showMainMenu();
        handleChoice(readChoice());
    }

    std::cout << "\nThanks for playing. Goodbye!\n";
}

void Menu::showMainMenu() const
{
    std::cout << "\n==============================\n"
              << "        MAZE PATHFINDER\n"
              << "==============================\n"
              << "1. Solve a maze\n"
              << "2. Play mode\n"
              << "3. Compare algorithms\n"
              << "4. Exit\n";
}

int Menu::readChoice() const
{
    int choice = 0;
    if (!InputHelper::readInt("Choose an option: ", SolveMaze, Exit, choice))
        return Exit; // input ended, so leave instead of looping forever

    return choice;
}

void Menu::handleChoice(int choice)
{
    switch (choice)
    {
    case SolveMaze:
        solveMaze();
        break;
    case PlayMode:
        std::cout << "\n[Play mode] Coming soon: race the AI to the goal.\n";
        break;
    case CompareAlgorithms:
        std::cout << "\n[Compare algorithms] Coming soon: BFS vs. A* stats.\n";
        break;
    case Exit:
        m_running = false;
        break;
    }
}

void Menu::solveMaze()
{
    if (!chooseMaze())
        return;

    std::cout << "\nHere's the maze:\n\n";
    m_maze.render();

    BfsPathfinder bfs;
    PathResult result = bfs.findPath(m_maze);
    showResult(bfs, result);
}

bool Menu::chooseMaze()
{
    const auto& presets = MazePresets::getAll();
    const int count = static_cast<int>(presets.size());

    std::cout << "\nChoose a maze:\n";
    for (int i = 0; i < count; ++i)
        std::cout << i + 1 << ". " << presets[i].name << '\n';
    std::cout << "0. Back to main menu\n";

    int choice = 0;
    if (!InputHelper::readInt("Choose a maze: ", 0, count, choice))
    {
        m_running = false; // input ended, so shut down cleanly
        return false;
    }

    if (choice == 0)
        return false;

    std::string error;
    if (!m_maze.loadFromLayout(presets[choice - 1].layout, error))
    {
        std::cout << "Could not load that maze: " << error << '\n';
        return false;
    }

    return true;
}

void Menu::showResult(const Pathfinder& pathfinder, const PathResult& result) const
{
    std::cout << "\nSolving with " << pathfinder.getName() << "...\n\n";

    if (!result.found)
    {
        std::cout << "No path exists from S to G. Walls block every route.\n"
                  << "Cells explored: " << result.cellsExplored << '\n';
        return;
    }

    m_maze.render(result.path);
    std::cout << "\nLegend: S = start, G = goal, # = wall, . = path\n"
              << "Path length:    " << result.getPathLength() << " moves\n"
              << "Cells explored: " << result.cellsExplored << '\n';
}
