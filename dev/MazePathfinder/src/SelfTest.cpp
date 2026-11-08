#include "SelfTest.h"

#include "AStarPathfinder.h"
#include "BfsPathfinder.h"
#include "Maze.h"
#include "MazeGenerator.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{
    const int SeedsPerDifficulty = 100;

    int g_passed = 0;
    int g_failed = 0;

    void check(bool condition, const std::string& description)
    {
        if (condition)
            ++g_passed;
        else
            ++g_failed;

        std::cout << (condition ? "  PASS  " : "  FAIL  ") << description << '\n';
    }

    int countOpenCells(const std::vector<std::string>& layout)
    {
        int open = 0;
        for (const std::string& row : layout)
            for (char c : row)
                if (c != '#')
                    ++open;
        return open;
    }

    // Every generated maze must be complete (all rooms connected), solvable,
    // and solved to the same length by both algorithms.
    void testGenerator(Difficulty difficulty)
    {
        MazeGenerator generator;
        BfsPathfinder bfs;
        AStarPathfinder aStar;
        const MazeGenerator::Size size = MazeGenerator::getSize(difficulty);

        // Rooms sit at odd rows and columns. A perfect maze connects them with
        // exactly (rooms - 1) knocked-down walls, so its open cells must total
        // rooms + (rooms - 1). Any other count means a room was missed.
        const int rooms = ((size.width - 1) / 2) * ((size.height - 1) / 2);
        const int expectedOpen = 2 * rooms - 1;

        int loaded = 0, complete = 0, solvedByBoth = 0, sameLength = 0;
        for (unsigned int seed = 1; seed <= SeedsPerDifficulty; ++seed)
        {
            std::vector<std::string> layout = generator.generate(difficulty, seed);

            Maze maze;
            std::string error;
            if (!maze.loadFromLayout(layout, error))
                continue;
            ++loaded;

            if (countOpenCells(layout) == expectedOpen)
                ++complete;

            PathResult bfsResult = bfs.findPath(maze);
            PathResult aStarResult = aStar.findPath(maze);
            if (bfsResult.found && aStarResult.found)
            {
                ++solvedByBoth;
                if (bfsResult.getPathLength() == aStarResult.getPathLength())
                    ++sameLength;
            }
        }

        const std::string label = MazeGenerator::getName(difficulty) + " (" +
            std::to_string(size.width) + " x " + std::to_string(size.height) + "), " +
            std::to_string(SeedsPerDifficulty) + " seeds: ";

        check(loaded == SeedsPerDifficulty, label + "every maze is a valid layout");
        check(complete == SeedsPerDifficulty, label + "every room is connected");
        check(solvedByBoth == SeedsPerDifficulty, label + "BFS and A* both find a path");
        check(sameLength == SeedsPerDifficulty, label + "BFS and A* paths are the same length");

        MazeGenerator other;
        check(generator.generate(difficulty, 42) == other.generate(difficulty, 42),
              MazeGenerator::getName(difficulty) + ": the same seed always builds the same maze");
    }

    void testSizeRounding()
    {
        MazeGenerator generator;
        std::vector<std::string> layout = generator.generate(4, 6, 1);
        check(layout.size() == 7 && layout[0].size() == 5,
              "Requested 4 x 6 is rounded up to 5 x 7 (sizes must be odd, at least 5)");
    }

    void expectLoadError(const std::vector<std::string>& layout, const std::string& description)
    {
        Maze maze;
        std::string error;
        bool loaded = maze.loadFromLayout(layout, error);
        check(!loaded && !error.empty() && !maze.isLoaded(), description + " -> \"" + error + "\"");
    }

    void testLayoutErrors()
    {
        expectLoadError({}, "Empty layout is rejected");
        expectLoadError({ "#####", "#S G#", "####" }, "Rows of different lengths are rejected");
        expectLoadError({ "#####", "#SXG#", "#####" }, "Unknown character is rejected");
        expectLoadError({ "#####", "#  G#", "#####" }, "Missing start is rejected");
        expectLoadError({ "#####", "#SGG#", "#####" }, "Two goals are rejected");
    }
}

bool SelfTest::runAll()
{
    std::cout << "MAZE PATHFINDER SELF-TEST\n\n"
              << "Random maze generation:\n";
    testGenerator(Difficulty::Easy);
    testGenerator(Difficulty::Medium);
    testGenerator(Difficulty::Hard);
    testSizeRounding();

    std::cout << "\nMaze layout validation:\n";
    testLayoutErrors();

    std::cout << "\n" << g_passed << " passed, " << g_failed << " failed\n";
    return g_failed == 0;
}
