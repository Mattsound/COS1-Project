#include "Menu.h"

#include "AStarPathfinder.h"
#include "BfsPathfinder.h"
#include "InputHelper.h"
#include "MazePresets.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>

namespace
{
    enum MenuOption
    {
        SolveMaze = 1,
        PlayMode = 2,
        CompareAlgorithms = 3,
        Exit = 4
    };

    enum NextStep
    {
        BackToMainMenu = 0,
        SolveThisMaze = 1,
        CompareThisMaze = 2,
        NewMaze = 3
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
              << "     Pick a built-in or random maze and an algorithm, then watch it find the shortest path.\n"
              << "2. Play mode (coming soon)\n"
              << "     Race an AI enemy to the goal.\n"
              << "3. Compare algorithms\n"
              << "     Run BFS and A* on the same maze and see which explores fewer cells.\n"
              << "4. Exit\n";
}

int Menu::readChoice() const
{
    int choice = 0;
    if (!InputHelper::readInt("Choose an option", SolveMaze, Exit, choice))
        return Exit; // input ended, so leave instead of looping forever

    return choice;
}

void Menu::handleChoice(int choice)
{
    switch (choice)
    {
    case SolveMaze:
        runMazeSession(Mode::Solve);
        break;
    case PlayMode:
        std::cout << "\nPlay mode isn't available yet. It's planned for a future update.\n"
                  << "In the meantime, try Solve a maze (1) or Compare algorithms (3).\n";
        // Keep the message on screen until the user is ready.
        if (!InputHelper::waitForEnter())
            m_running = false;
        break;
    case CompareAlgorithms:
        runMazeSession(Mode::Compare);
        break;
    case Exit:
        m_running = false;
        break;
    }
}

void Menu::runMazeSession(Mode mode)
{
    if (!chooseMaze())
        return;

    while (m_running)
    {
        if (mode == Mode::Solve)
        {
            if (!solveCurrentMaze())
                return; // went back from the algorithm picker
        }
        else
        {
            compareCurrentMaze();
        }

        if (!m_running)
            return;

        // The results stay on screen while the user decides what to do next.
        switch (askWhatNext())
        {
        case SolveThisMaze:
            mode = Mode::Solve;
            break;
        case CompareThisMaze:
            mode = Mode::Compare;
            break;
        case NewMaze:
            if (!chooseMaze())
                return;
            break;
        default:
            return; // back to the main menu
        }
    }
}

int Menu::askWhatNext()
{
    std::cout << "\nWhat next?\n"
              << SolveThisMaze << ". Solve this maze with an algorithm\n"
              << CompareThisMaze << ". Compare algorithms on this maze\n"
              << NewMaze << ". Pick a new maze\n"
              << BackToMainMenu << ". Back to main menu\n";

    int choice = BackToMainMenu;
    if (!InputHelper::readInt("Choose an option", BackToMainMenu, NewMaze, choice))
    {
        m_running = false; // input ended, so shut down cleanly
        return BackToMainMenu;
    }
    return choice;
}

bool Menu::solveCurrentMaze()
{
    Pathfinder* pathfinder = choosePathfinder();
    if (pathfinder == nullptr)
        return false;

    std::cout << "\nHere's the maze:\n\n";
    m_maze.render();

    PathResult result = pathfinder->findPath(m_maze);
    showResult(*pathfinder, result);

    if (askToShowExplored())
    {
        std::cout << "\nEvery cell " << pathfinder->getShortName() << " explored:\n\n";
        m_maze.render(result.path, result.explored);
        printLegend(true);
    }

    return true;
}

void Menu::compareCurrentMaze()
{
    std::vector<PathResult> results;
    for (const auto& pathfinder : m_pathfinders)
        results.push_back(pathfinder->findPath(m_maze));

    printSideBySide(results, false);
    printStatsTable(results);
    printComparisonSummary(results);

    if (askToShowExplored())
    {
        std::cout << "\nEvery cell each algorithm explored:\n";
        printSideBySide(results, true);
        printLegend(true);
    }
}

void Menu::printSideBySide(const std::vector<PathResult>& results, bool showExplored) const
{
    // Each maze is drawn under its algorithm's name.
    const int columnWidth = m_maze.getCols() + ColumnGap;

    std::cout << "\n  ";
    for (const auto& pathfinder : m_pathfinders)
        std::cout << std::left << std::setw(columnWidth) << pathfinder->getShortName();
    std::cout << "\n\n";

    std::vector<std::vector<std::string>> drawings;
    for (const PathResult& result : results)
        drawings.push_back(m_maze.renderLines(result.path, showExplored ? result.explored : std::vector<Cell*>{}));

    for (int row = 0; row < m_maze.getRows(); ++row)
    {
        std::cout << "  ";
        for (const auto& drawing : drawings)
            std::cout << std::left << std::setw(columnWidth) << drawing[row];
        std::cout << '\n';
    }
}

void Menu::printStatsTable(const std::vector<PathResult>& results) const
{
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
        std::cout << std::setw(17) << results[i].getCellsExplored() << '\n';
    }
}

void Menu::printComparisonSummary(const std::vector<PathResult>& results) const
{
    std::cout << '\n';
    if (!results[0].found)
    {
        std::cout << "No path exists, so every algorithm had to search every cell it could reach.\n";
        return;
    }

    size_t fewest = 0;
    for (size_t i = 1; i < results.size(); ++i)
    {
        if (results[i].getCellsExplored() < results[fewest].getCellsExplored())
            fewest = i;
    }

    const int baseline = results[0].getCellsExplored();
    const int saved = baseline - results[fewest].getCellsExplored();
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

void Menu::printLegend(bool withExplored) const
{
    std::cout << "\nLegend: S = start, G = goal, # = wall, . = path";
    if (withExplored)
        std::cout << ", o = explored but not on the path";
    std::cout << '\n';
}

bool Menu::askToShowExplored()
{
    bool show = false;
    if (!InputHelper::readYesNo("\nShow every cell the search explored?", show))
    {
        m_running = false; // input ended, so shut down cleanly
        return false;
    }
    return show;
}

bool Menu::chooseMaze()
{
    const auto& presets = MazePresets::getAll();
    const int count = static_cast<int>(presets.size());

    const int randomOption = count + 1;

    std::cout << "\nChoose a maze:\n";
    for (int i = 0; i < count; ++i)
        std::cout << i + 1 << ". " << presets[i].name << '\n';
    std::cout << randomOption << ". Random maze (choose a difficulty)\n"
              << "0. Back to main menu\n";

    int choice = 0;
    if (!InputHelper::readInt("Choose a maze", 0, randomOption, choice))
    {
        m_running = false; // input ended, so shut down cleanly
        return false;
    }

    if (choice == 0)
        return false;

    if (choice == randomOption)
        return chooseRandomMaze();

    return loadLayout(presets[choice - 1].layout);
}

bool Menu::chooseRandomMaze()
{
    const Difficulty difficulties[] = { Difficulty::Easy, Difficulty::Medium, Difficulty::Hard };
    const int count = static_cast<int>(std::size(difficulties));

    std::cout << "\nChoose a difficulty:\n";
    for (int i = 0; i < count; ++i)
    {
        MazeGenerator::Size size = MazeGenerator::getSize(difficulties[i]);
        std::cout << i + 1 << ". " << MazeGenerator::getName(difficulties[i])
                  << " (" << size.width << " x " << size.height << ")\n";
    }
    std::cout << "0. Back to main menu\n";

    int choice = 0;
    if (!InputHelper::readInt("Choose a difficulty", 0, count, choice))
    {
        m_running = false;
        return false;
    }

    if (choice == 0)
        return false;

    // Keep seeds short so they're easy to read and note down.
    const unsigned int seed = std::random_device{}() % 100000;
    const Difficulty difficulty = difficulties[choice - 1];
    if (!loadLayout(m_generator.generate(difficulty, seed)))
        return false;

    std::cout << "\nGenerated random maze #" << seed << " (" << MazeGenerator::getName(difficulty)
              << ", " << m_maze.getCols() << " x " << m_maze.getRows() << ")\n";
    return true;
}

bool Menu::loadLayout(const std::vector<std::string>& layout)
{
    std::string error;
    if (!m_maze.loadFromLayout(layout, error))
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
    if (!InputHelper::readInt("Choose an algorithm", 0, count, choice))
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
                  << "Cells explored: " << result.getCellsExplored() << '\n';
        return;
    }

    m_maze.render(result.path);
    printLegend(false);
    std::cout << "Path length:    " << result.getPathLength() << " moves\n"
              << "Cells explored: " << result.getCellsExplored() << '\n';
}
