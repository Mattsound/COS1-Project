#include "Menu.h"

#include "AStarPathfinder.h"
#include "BfsPathfinder.h"
#include "InputHelper.h"
#include "MazePresets.h"

#include <algorithm>
#include <iomanip>
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

    const int ColumnGap = 6; // spaces between mazes drawn side by side
}

Menu::Menu()
{
    m_pathfinders.push_back(std::make_unique<BfsPathfinder>());
    m_pathfinders.push_back(std::make_unique<AStarPathfinder>());
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
        compareAlgorithms();
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

    Pathfinder* pathfinder = choosePathfinder();
    if (pathfinder == nullptr)
        return;

    std::cout << "\nHere's the maze:\n\n";
    m_maze.render();

    PathResult result = pathfinder->findPath(m_maze);
    showResult(*pathfinder, result);
}

void Menu::compareAlgorithms()
{
    if (!chooseMaze())
        return;

    std::vector<PathResult> results;
    for (const auto& pathfinder : m_pathfinders)
        results.push_back(pathfinder->findPath(m_maze));

    // Draw every solved maze side by side, each under its algorithm's name.
    const int mazeWidth = m_maze.getCols();
    const int columnWidth = mazeWidth + ColumnGap;

    std::cout << "\n  ";
    for (const auto& pathfinder : m_pathfinders)
        std::cout << std::left << std::setw(columnWidth) << pathfinder->getShortName();
    std::cout << "\n\n";

    std::vector<std::vector<std::string>> drawings;
    for (const PathResult& result : results)
        drawings.push_back(m_maze.renderLines(result.path));

    for (int row = 0; row < m_maze.getRows(); ++row)
    {
        std::cout << "  ";
        for (const auto& drawing : drawings)
            std::cout << std::left << std::setw(columnWidth) << drawing[row];
        std::cout << '\n';
    }

    // Stats table.
    std::cout << "\n  " << std::left << std::setw(30) << "Algorithm"
              << std::right << std::setw(13) << "Path length"
              << std::setw(17) << "Cells explored" << '\n'
              << "  " << std::string(60, '-') << '\n';
    for (size_t i = 0; i < results.size(); ++i)
    {
        std::cout << "  " << std::left << std::setw(30) << m_pathfinders[i]->getName() << std::right;
        if (results[i].found)
            std::cout << std::setw(13) << results[i].getPathLength();
        else
            std::cout << std::setw(13) << "no path";
        std::cout << std::setw(17) << results[i].cellsExplored << '\n';
    }

    // Summary of what the numbers mean.
    std::cout << '\n';
    if (!results[0].found)
    {
        std::cout << "No path exists, so every algorithm had to search every cell it could reach.\n";
        return;
    }

    size_t fewest = 0;
    for (size_t i = 1; i < results.size(); ++i)
    {
        if (results[i].cellsExplored < results[fewest].cellsExplored)
            fewest = i;
    }

    const int baseline = results[0].cellsExplored;
    const int saved = baseline - results[fewest].cellsExplored;
    if (fewest == 0 || saved == 0)
    {
        std::cout << "Every algorithm explored the same number of cells on this maze.\n";
    }
    else
    {
        std::cout << m_pathfinders[fewest]->getShortName() << " explored " << saved << " fewer cells than "
                  << m_pathfinders[0]->getShortName() << " (" << (saved * 100 / baseline) << "% less).\n";
    }

    bool sameLength = std::all_of(results.begin(), results.end(), [&](const PathResult& r)
    {
        return r.getPathLength() == results[0].getPathLength();
    });
    if (sameLength)
        std::cout << "All paths are the same length, so each algorithm found a shortest path.\n";
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

Pathfinder* Menu::choosePathfinder()
{
    const int count = static_cast<int>(m_pathfinders.size());

    std::cout << "\nChoose an algorithm:\n";
    for (int i = 0; i < count; ++i)
        std::cout << i + 1 << ". " << m_pathfinders[i]->getName() << '\n';
    std::cout << "0. Back to main menu\n";

    int choice = 0;
    if (!InputHelper::readInt("Choose an algorithm: ", 0, count, choice))
    {
        m_running = false; // input ended, so shut down cleanly
        return nullptr;
    }

    if (choice == 0)
        return nullptr;

    return m_pathfinders[choice - 1].get();
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
