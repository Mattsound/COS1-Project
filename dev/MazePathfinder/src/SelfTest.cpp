#include "SelfTest.h"

#include "AStarPathfinder.h"
#include "BfsPathfinder.h"
#include "Maze.h"
#include "MazeGenerator.h"
#include "MazePresets.h"
#include "Enemy.h"
#include "Game.h"
#include "Player.h"

#include <algorithm>
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
                everyMoveWorked = walker.move(randomMaze, Player::directionBetween(*result.path[i - 1], *result.path[i])) && everyMoveWorked;

            if (everyMoveWorked && walker.getCell() == randomMaze.getGoal() &&
                walker.getMoveCount() == result.getPathLength())
                ++reachedGoal;
        }
        check(reachedGoal == SeedsPerDifficulty,
              "Walking BFS's path on 100 random mazes reaches G in exactly the path length");
    }
}

namespace
{
    // findPathBetween() must work between any two cells, not just S and G.
    void testPathBetween()
    {
        MazeGenerator generator;
        BfsPathfinder bfs;
        AStarPathfinder aStar;
        std::string error;

        int reversedMatches = 0;
        for (unsigned int seed = 1; seed <= SeedsPerDifficulty; ++seed)
        {
            Maze maze;
            maze.loadFromLayout(generator.generate(Difficulty::Medium, seed), error);

            const int forward = bfs.findPath(maze).getPathLength();
            PathResult bfsBack = bfs.findPathBetween(maze, maze.getGoal(), maze.getStart());
            PathResult aStarBack = aStar.findPathBetween(maze, maze.getGoal(), maze.getStart());

            bool endsRight = bfsBack.found && aStarBack.found &&
                             bfsBack.path.front() == maze.getGoal() && bfsBack.path.back() == maze.getStart() &&
                             aStarBack.path.front() == maze.getGoal() && aStarBack.path.back() == maze.getStart();
            if (endsRight && bfsBack.getPathLength() == forward && aStarBack.getPathLength() == forward)
                ++reversedMatches;
        }
        check(reversedMatches == SeedsPerDifficulty,
              "Searching G to S on 100 random mazes gives a path from G to S the same length as S to G (BFS and A*)");

        Maze maze;
        maze.loadFromLayout(MazePresets::getAll()[0].layout, error);
        PathResult same = aStar.findPathBetween(maze, maze.getStart(), maze.getStart());
        check(same.found && same.getPathLength() == 0, "Searching from a cell to itself finds a 0-move path");
        check(!aStar.findPathBetween(maze, nullptr, maze.getGoal()).found &&
              !bfs.findPathBetween(maze, maze.getStart(), nullptr).found,
              "A missing start or goal cell reports no path instead of crashing");
    }
}

namespace
{
    // Chases a target that stands still. Returns the number of turns it took
    // to catch it (or -1 if it never did), and checks every turn that the
    // enemy stood on an open cell and got exactly stepsPerTurn moves closer
    // (or reached the target).
    int turnsToCatch(Maze& maze, Enemy& enemy, Cell* target, bool& everyTurnValid)
    {
        BfsPathfinder ruler; // measures the real distance after each turn
        everyTurnValid = true;
        int distance = ruler.findPathBetween(maze, enemy.getCell(), target).getPathLength();

        // Fix the limit up front: distance shrinks every turn, so using it in
        // the loop condition would stop the chase halfway.
        const int maxTurns = distance + 1;
        for (int turn = 1; turn <= maxTurns; ++turn)
        {
            bool caught = enemy.takeTurn(maze, target);
            int newDistance = ruler.findPathBetween(maze, enemy.getCell(), target).getPathLength();

            int expected = std::max(0, distance - enemy.getStepsPerTurn());
            if (!enemy.getCell()->isWalkable() || newDistance != expected || caught != (newDistance == 0))
                everyTurnValid = false;

            distance = newDistance;
            if (caught)
                return turn;
        }
        return -1;
    }

    void testEnemy()
    {
        MazeGenerator generator;
        BfsPathfinder bfs;
        AStarPathfinder aStar;
        std::string error;

        int caughtOnTime = 0, fastCaughtOnTime = 0;
        for (unsigned int seed = 1; seed <= SeedsPerDifficulty; ++seed)
        {
            Maze maze;
            maze.loadFromLayout(generator.generate(Difficulty::Medium, seed), error);
            const int distance = bfs.findPath(maze).getPathLength();
            bool valid = false;

            Enemy enemy(maze.getGoal(), aStar, 1);
            if (turnsToCatch(maze, enemy, maze.getStart(), valid) == distance && valid)
                ++caughtOnTime;

            Enemy fastEnemy(maze.getGoal(), aStar, 2);
            if (turnsToCatch(maze, fastEnemy, maze.getStart(), valid) == (distance + 1) / 2 && valid)
                ++fastCaughtOnTime;
        }
        check(caughtOnTime == SeedsPerDifficulty,
              "Enemy (1 step/turn) gets exactly 1 move closer each turn and catches a still target in d turns on 100 mazes");
        check(fastCaughtOnTime == SeedsPerDifficulty,
              "Enemy (2 steps/turn) gets 2 moves closer each turn and catches a still target in d/2 turns (rounded up)");

        Maze small;
        small.loadFromLayout(MazePresets::getAll()[0].layout, error);
        Enemy onTarget(small.getStart(), aStar);
        check(onTarget.takeTurn(small, small.getStart()) && onTarget.getCell() == small.getStart(),
              "An enemy already on the target reports a catch without moving");

        Maze vault;
        vault.loadFromLayout(MazePresets::getAll()[2].layout, error);
        Enemy walledOff(vault.getGoal(), aStar);
        bool neverCaught = true;
        for (int turn = 0; turn < 5; ++turn)
            neverCaught = !walledOff.takeTurn(vault, vault.getStart()) && neverCaught;
        check(neverCaught && walledOff.getCell() == vault.getGoal(),
              "Sealed Vault: an enemy walled off from the target stays put and never catches it");

        Enemy clamped(small.getGoal(), aStar, 0);
        check(clamped.getStepsPerTurn() == 1, "Steps per turn below 1 are raised to 1");
    }
}

namespace
{
    void testGameRules()
    {
        AStarPathfinder aStar;
        std::string error;
        const Game::Settings easy = Game::getSettings(Difficulty::Easy);
        const Game::Settings medium = Game::getSettings(Difficulty::Medium);
        const Game::Settings hard = Game::getSettings(Difficulty::Hard);

        check(easy.enemyMovesEvery == 2 && easy.enemyStepsPerTurn == 1 && easy.extraHeadStart == 6 &&
              medium.enemyMovesEvery == 1 && medium.enemyStepsPerTurn == 1 && medium.extraHeadStart == 3 &&
              hard.enemyMovesEvery == 1 && hard.enemyStepsPerTurn == 1 && hard.extraHeadStart == 0,
              "Difficulty settings: Easy half speed +6 turns, Medium full speed +3 turns, Hard full speed +0 turns");

        // Small preset: S at (1,1), open cells to the right at (1,2) and (1,3).
        Maze small;
        small.loadFromLayout(MazePresets::getAll()[0].layout, error);

        Game blocked(small, aStar, medium, { small.getCell(5, 1), 0 });
        bool refused = !blocked.playTurn(Direction::Up);
        check(refused && blocked.getTurn() == 0 && blocked.getEnemyCell() == small.getCell(5, 1) &&
              blocked.getState() == GameState::Playing,
              "A blocked move uses no turn and the enemy doesn't move");

        Game walkInto(small, aStar, easy, { small.getCell(1, 3), 0 });
        walkInto.playTurn(Direction::Right); // turn 1: an Easy enemy waits
        walkInto.playTurn(Direction::Right); // turn 2: step onto the enemy
        check(walkInto.getState() == GameState::Caught && walkInto.getTurn() == 2,
              "Walking onto the enemy's cell is a loss");

        Game easyTiming(small, aStar, easy, { small.getCell(5, 1), 0 });
        easyTiming.playTurn(Direction::Right);
        bool waitedOnTurn1 = easyTiming.getEnemyCell() == small.getCell(5, 1);
        easyTiming.playTurn(Direction::Right);
        bool movedOnTurn2 = easyTiming.getEnemyCell() != small.getCell(5, 1);
        check(waitedOnTurn1 && movedOnTurn2, "On Easy the enemy only moves after every second turn");

        Game mediumTiming(small, aStar, medium, { small.getCell(5, 1), 0 });
        mediumTiming.playTurn(Direction::Right);
        check(mediumTiming.getEnemyCell() != small.getCell(5, 1), "On Medium the enemy moves after every turn");

        // With a 3-turn head start the enemy sleeps through turns 1-3.
        Game sleepy(small, aStar, medium, { small.getCell(5, 1), 3 });
        sleepy.playTurn(Direction::Right);
        sleepy.playTurn(Direction::Right);
        sleepy.playTurn(Direction::Down);
        bool sleptThrough3 = sleepy.getEnemyCell() == small.getCell(5, 1) && sleepy.isEnemyAwake();
        sleepy.playTurn(Direction::Down);
        check(sleptThrough3 && sleepy.getEnemyCell() != small.getCell(5, 1),
              "With a 3-turn head start the enemy sleeps for turns 1-3 and moves after turn 4");

        // S G on top, open cell under S. The enemy starts under S, follows the
        // player, and is right behind them when they step onto G.
        Maze tiny;
        tiny.loadFromLayout({ "S G", " ##" }, error);
        Game photoFinish(tiny, aStar, medium, { tiny.getCell(1, 0), 0 });
        photoFinish.playTurn(Direction::Right);
        bool enemyRightBehind = photoFinish.getEnemyCell() == tiny.getStart();
        photoFinish.playTurn(Direction::Right);
        check(enemyRightBehind && photoFinish.getState() == GameState::Won,
              "Reaching G wins even with the enemy one cell behind (the player moves first)");

        Game finished(tiny, aStar, medium, { tiny.getCell(1, 0), 0 });
        finished.playTurn(Direction::Right);
        finished.playTurn(Direction::Right);
        check(!finished.playTurn(Direction::Left) && finished.getTurn() == 2,
              "No more moves are accepted after the game is over");

        Maze corridor;
        corridor.loadFromLayout({ "#######", "#S   G#", "#######" }, error);
        check(Game::planEnemy(corridor, medium).start == nullptr,
              "A corridor maze (every open cell on the route) is reported as having no room for an enemy");
    }

    // Every random game must be fair (winnable by walking the shortest route)
    // but not free (a player who stalls gets caught), and the head start must
    // be as short as possible before the difficulty's extra turns are added.
    void testGameFairness(Difficulty difficulty)
    {
        MazeGenerator generator;
        AStarPathfinder aStar;
        const Game::Settings settings = Game::getSettings(difficulty);
        std::string error;

        int planned = 0, corridorsSkipped = 0, shortestRouteWins = 0, minimal = 0, stallingLoses = 0;
        for (unsigned int seed = 1; seed <= SeedsPerDifficulty; ++seed)
        {
            // Same path the game takes: skip corridor mazes with no room for
            // the enemy and use the next seed instead.
            Maze maze;
            Game::EnemyPlan plan;
            const unsigned int usedSeed = Game::generatePlayable(generator, difficulty, seed, maze, plan);
            if (usedSeed != seed)
                ++corridorsSkipped;
            if (plan.start == nullptr || plan.start->getType() != CellType::Open || plan.headStart < settings.extraHeadStart)
                continue;
            ++planned;

            if (Game::playShortestRoute(maze, aStar, settings, plan) == GameState::Won)
                ++shortestRouteWins;

            // One turn less than the fair minimum (ignoring the extra turns)
            // should let the enemy catch a player on the shortest route.
            Game::EnemyPlan tighter = plan;
            tighter.headStart = plan.headStart - settings.extraHeadStart - 1;
            if (tighter.headStart < 0 || Game::playShortestRoute(maze, aStar, settings, tighter) == GameState::Caught)
                ++minimal;

            // Step back and forth between S and its first neighbor. The S
            // corner always has a way out (it's where the maze was carved
            // from), so try each direction until one works.
            Game stall(maze, aStar, settings, plan);
            const Direction there[] = { Direction::Right, Direction::Down };
            const Direction back[] = { Direction::Left, Direction::Up };
            int way = stall.playTurn(there[0]) ? 0 : (stall.playTurn(there[1]) ? 1 : -1);
            const int turnLimit = 4 * maze.getRows() * maze.getCols();
            for (int turn = 0; way >= 0 && turn < turnLimit && stall.getState() == GameState::Playing; ++turn)
                stall.playTurn(turn % 2 == 0 ? back[way] : there[way]);

            if (stall.getState() == GameState::Caught)
                ++stallingLoses;
        }

        const MazeGenerator::Size size = MazeGenerator::getSize(difficulty);
        const std::string label = MazeGenerator::getName(difficulty) + " (" + std::to_string(size.width) + " x " +
            std::to_string(size.height) + ", enemy " + Game::describeEnemy(settings) + "), 100 seeds: ";
        auto outOf = [](int count) { return " (" + std::to_string(count) + "/" + std::to_string(SeedsPerDifficulty) + ")"; };
        check(planned == SeedsPerDifficulty, label + "a playable maze with an enemy lair is always found" + outOf(planned) +
              (corridorsSkipped > 0 ? ", skipping " + std::to_string(corridorsSkipped) + " corridor maze(s)" : ""));
        check(shortestRouteWins == SeedsPerDifficulty, label + "walking the shortest route always wins" + outOf(shortestRouteWins));
        check(minimal == SeedsPerDifficulty, label + "one turn less head start and the shortest route is caught" + outOf(minimal));
        check(stallingLoses == SeedsPerDifficulty, label + "a player who stalls near S always gets caught" + outOf(stallingLoses));
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

    std::cout << "\nPaths between any two cells:\n";
    testPathBetween();

    std::cout << "\nMaze layout validation:\n";
    testLayoutErrors();

    std::cout << "\nPlayer movement:\n";
    testPlayer();

    std::cout << "\nEnemy chasing:\n";
    testEnemy();

    std::cout << "\nPlay mode rules:\n";
    testGameRules();

    std::cout << "\nPlay mode fairness:\n";
    testGameFairness(Difficulty::Easy);
    testGameFairness(Difficulty::Medium);
    testGameFairness(Difficulty::Hard);

    std::cout << "\n" << g_passed << " passed, " << g_failed << " failed\n";
    return g_failed == 0;
}
