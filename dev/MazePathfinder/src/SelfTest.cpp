#include "SelfTest.h"

#include "AStarPathfinder.h"
#include "BfsPathfinder.h"
#include "Maze.h"
#include "MazeGenerator.h"
#include "MazePresets.h"
#include "Player.h"

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

    // The explored list must start at S, end at G when a path was found, hold
    // each walkable cell at most once, and include every cell on the path.
    bool exploredListIsValid(const Maze& maze, const PathResult& result)
    {
        if (result.explored.empty() || result.explored.front() != maze.getStart())
            return false;
        if (result.found && result.explored.back() != maze.getGoal())
            return false;

        std::vector<bool> seen(maze.getCellCount(), false);
        for (const Cell* cell : result.explored)
        {
            size_t index = maze.indexOf(*cell);
            if (!cell->isWalkable() || seen[index])
                return false;
            seen[index] = true;
        }

        for (const Cell* cell : result.path)
        {
            if (!seen[maze.indexOf(*cell)])
                return false;
        }
        return true;
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

        int loaded = 0, complete = 0, solvedByBoth = 0, sameLength = 0, validExplored = 0;
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

            if (exploredListIsValid(maze, bfsResult) && exploredListIsValid(maze, aStarResult))
                ++validExplored;
        }

        const std::string label = MazeGenerator::getName(difficulty) + " (" +
            std::to_string(size.width) + " x " + std::to_string(size.height) + "), " +
            std::to_string(SeedsPerDifficulty) + " seeds: ";

        check(loaded == SeedsPerDifficulty, label + "every maze is a valid layout");
        check(complete == SeedsPerDifficulty, label + "every room is connected");
        check(solvedByBoth == SeedsPerDifficulty, label + "BFS and A* both find a path");
        check(sameLength == SeedsPerDifficulty, label + "BFS and A* paths are the same length");
        check(validExplored == SeedsPerDifficulty, label + "explored lists start at S, end at G, have no repeats, and cover the path");

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

    // With no path, both searches must give up only after exploring every
    // cell reachable from S (10 in the Sealed Vault preset).
    void testNoPath()
    {
        Maze maze;
        std::string error;
        maze.loadFromLayout(MazePresets::getAll()[2].layout, error);

        BfsPathfinder bfs;
        AStarPathfinder aStar;
        PathResult bfsResult = bfs.findPath(maze);
        PathResult aStarResult = aStar.findPath(maze);

        check(!bfsResult.found && !aStarResult.found, "Sealed Vault: both report no path");
        check(bfsResult.getCellsExplored() == 10 && aStarResult.getCellsExplored() == 10,
              "Sealed Vault: both explore all 10 reachable cells before giving up");
        check(exploredListIsValid(maze, bfsResult) && exploredListIsValid(maze, aStarResult),
              "Sealed Vault: explored lists start at S and have no repeats");
    }
}

namespace
{
    bool parsesTo(char key, Direction expected)
    {
        Direction direction;
        return Player::parseDirection(key, direction) && direction == expected;
    }

    // The direction that moves from one cell to a neighboring cell.
    Direction directionBetween(const Cell& from, const Cell& to)
    {
        if (to.getRow() < from.getRow()) return Direction::Up;
        if (to.getRow() > from.getRow()) return Direction::Down;
        if (to.getCol() < from.getCol()) return Direction::Left;
        return Direction::Right;
    }

    void testPlayer()
    {
        Direction unused;
        check(parsesTo('w', Direction::Up) && parsesTo('W', Direction::Up) &&
              parsesTo('a', Direction::Left) && parsesTo('S', Direction::Down) && parsesTo('d', Direction::Right),
              "W/A/S/D map to up/left/down/right in either case");
        check(!Player::parseDirection('x', unused) && !Player::parseDirection('1', unused) &&
              !Player::parseDirection(' ', unused),
              "Other keys are not treated as moves");

        // Small preset: S is at row 1, column 1 with walls above and to the left.
        Maze maze;
        std::string error;
        maze.loadFromLayout(MazePresets::getAll()[0].layout, error);
        Player player(maze.getStart());

        bool blocked = !player.move(maze, Direction::Up) && !player.move(maze, Direction::Left);
        check(blocked && player.getCell() == maze.getStart() && player.getMoveCount() == 0,
              "Moving into a wall is refused, the player stays put, and no move is counted");

        bool moved = player.move(maze, Direction::Right);
        check(moved && player.getCell() == maze.getCell(1, 2) && player.getMoveCount() == 1,
              "Moving into an open cell works and counts as one move");

        // A one-row maze with no border walls, so "up" would leave the grid.
        Maze edgeMaze;
        edgeMaze.loadFromLayout({ "S G" }, error);
        Player edgePlayer(edgeMaze.getStart());
        check(!edgePlayer.move(edgeMaze, Direction::Up) && !edgePlayer.move(edgeMaze, Direction::Left) &&
              edgePlayer.getCell() == edgeMaze.getStart(),
              "Moving off the edge of the grid is refused");

        // Follow BFS's shortest path one move at a time on random mazes. The
        // player should reach G in exactly as many moves as the path length.
        MazeGenerator generator;
        BfsPathfinder bfs;
        int reachedGoal = 0;
        for (unsigned int seed = 1; seed <= SeedsPerDifficulty; ++seed)
        {
            Maze randomMaze;
            randomMaze.loadFromLayout(generator.generate(Difficulty::Medium, seed), error);
            PathResult result = bfs.findPath(randomMaze);

            Player walker(randomMaze.getStart());
            bool everyMoveWorked = true;
            for (size_t i = 1; i < result.path.size(); ++i)
                everyMoveWorked = walker.move(randomMaze, directionBetween(*result.path[i - 1], *result.path[i])) && everyMoveWorked;

            if (everyMoveWorked && walker.getCell() == randomMaze.getGoal() &&
                walker.getMoveCount() == result.getPathLength())
                ++reachedGoal;
        }
        check(reachedGoal == SeedsPerDifficulty,
              "Walking BFS's path on 100 random mazes reaches G in exactly the path length");
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

    std::cout << "\nMaze with no path:\n";
    testNoPath();

    std::cout << "\nMaze layout validation:\n";
    testLayoutErrors();

    std::cout << "\nPlayer movement:\n";
    testPlayer();

    std::cout << "\n" << g_passed << " passed, " << g_failed << " failed\n";
    return g_failed == 0;
}
