#include "Game.h"

#include "BfsPathfinder.h"
#include "Maze.h"

#include <algorithm>
#include <queue>
#include <utility>
#include <vector>

namespace
{
    // Distance in moves from origin to every cell (-1 if unreachable), using
    // one breadth-first flood instead of one search per cell.
    std::vector<int> distancesFrom(Maze& maze, Cell* origin)
    {
        std::vector<int> distance(maze.getCellCount(), -1);
        std::queue<Cell*> frontier;
        distance[maze.indexOf(*origin)] = 0;
        frontier.push(origin);

        while (!frontier.empty())
        {
            Cell* current = frontier.front();
            frontier.pop();
            for (Cell* next : maze.getNeighbors(*current))
            {
                int& d = distance[maze.indexOf(*next)];
                if (d == -1)
                {
                    d = distance[maze.indexOf(*current)] + 1;
                    frontier.push(next);
                }
            }
        }
        return distance;
    }
}

Game::Settings Game::getSettings(Difficulty difficulty)
{
    switch (difficulty)
    {
    case Difficulty::Easy:   return { 1, 2, 6 }; // half your speed, 6 spare turns
    case Difficulty::Medium: return { 1, 1, 3 }; // your speed, 3 spare turns
    default:                 return { 1, 1, 0 }; // your speed, no spare turns
    }
}

std::string Game::describeEnemy(const Settings& settings)
{
    std::string text;
    if (settings.enemyMovesEvery > 1)
        text = "moves every other turn";
    else if (settings.enemyStepsPerTurn > 1)
        text = "moves " + std::to_string(settings.enemyStepsPerTurn) + " cells per turn";
    else
        text = "moves every turn";

    return text + (settings.extraHeadStart == 0 ? ", no room for mistakes" : "");
}

Game::Game(Maze& maze, Pathfinder& enemyPathfinder, Settings settings, EnemyPlan plan)
    : m_maze(maze),
      m_player(maze.getStart()),
      m_enemy(plan.start, enemyPathfinder, settings.enemyStepsPerTurn),
      m_settings(settings),
      m_headStart(plan.headStart)
{
}

bool Game::enemyMovesAfterTurn(int turn) const
{
    return turn > m_headStart && (turn - m_headStart) % m_settings.enemyMovesEvery == 0;
}

bool Game::playTurn(Direction direction)
{
    if (m_state != GameState::Playing)
        return false;

    if (!m_player.move(m_maze, direction))
        return false; // blocked: no turn used

    ++m_turn;

    if (m_player.getCell() == m_maze.getGoal())
    {
        m_state = GameState::Won;
        return true;
    }

    if (m_player.getCell() == m_enemy.getCell())
    {
        m_state = GameState::Caught; // walked right into it
        return true;
    }

    if (enemyMovesAfterTurn(m_turn) && m_enemy.takeTurn(m_maze, m_player.getCell()))
        m_state = GameState::Caught;

    return true;
}

GameState Game::playShortestRoute(Maze& maze, Pathfinder& enemyPathfinder, Settings settings, EnemyPlan plan)
{
    BfsPathfinder bfs;
    std::vector<Cell*> route = bfs.findPath(maze).path; // copy before the game re-plans

    Game game(maze, enemyPathfinder, settings, plan);
    for (size_t i = 1; i < route.size() && game.getState() == GameState::Playing; ++i)
        game.playTurn(Player::directionBetween(*route[i - 1], *route[i]));

    return game.getState();
}

Game::EnemyPlan Game::planEnemy(Maze& maze, Settings settings)
{
    EnemyPlan plan;

    BfsPathfinder bfs;
    const std::vector<Cell*> route = bfs.findPath(maze).path;
    if (route.empty())
        return plan; // no route to G, so no game

    std::vector<bool> onRoute(maze.getCellCount(), false);
    for (const Cell* cell : route)
        onRoute[maze.indexOf(*cell)] = true;

    // The lair: the open cell farthest from S that isn't on the route
    // (an enemy sleeping on the route would block it no matter what).
    const std::vector<int> fromStart = distancesFrom(maze, maze.getStart());
    int farthest = 0;
    for (int row = 0; row < maze.getRows(); ++row)
    {
        for (int col = 0; col < maze.getCols(); ++col)
        {
            Cell* cell = maze.getCell(row, col);
            const size_t index = maze.indexOf(*cell);
            if (cell->getType() == CellType::Open && !onRoute[index] && fromStart[index] > farthest)
            {
                farthest = fromStart[index];
                plan.start = cell;
            }
        }
    }
    if (plan.start == nullptr)
        return plan; // every open cell is on the route: no room for an enemy

    // The smallest head start that keeps a player on the shortest route safe.
    // After turn t the player stands on route[t]. By then the enemy has made
    // (t - headStart) / movesEvery moves of stepsPerTurn cells. It must still
    // be farther than that from route[t], even if it guessed the route
    // perfectly. The last turn is G, which the player reaches before the
    // enemy moves.
    const std::vector<int> fromLair = distancesFrom(maze, plan.start);
    const int lastTurn = static_cast<int>(route.size()) - 1;
    int minimum = 0;
    for (; minimum < lastTurn; ++minimum)
    {
        bool safe = true;
        for (int t = minimum + 1; t < lastTurn && safe; ++t)
        {
            int enemyReach = settings.enemyStepsPerTurn * ((t - minimum) / settings.enemyMovesEvery);
            if (fromLair[maze.indexOf(*route[t])] <= enemyReach)
                safe = false;
        }
        if (safe)
            break;
    }

    plan.headStart = minimum + settings.extraHeadStart;
    return plan;
}

unsigned int Game::generatePlayable(MazeGenerator& generator, Difficulty difficulty, unsigned int seed,
                                    Maze& maze, EnemyPlan& plan)
{
    const Settings settings = getSettings(difficulty);
    const int MaxTries = 100; // corridor mazes are rare, so this is plenty
    std::string error;

    plan = EnemyPlan();
    for (int attempt = 0; attempt < MaxTries; ++attempt, ++seed)
    {
        if (!maze.loadFromLayout(generator.generate(difficulty, seed), error))
            continue;

        plan = planEnemy(maze, settings);
        if (plan.start != nullptr)
            return seed;
    }
    return seed; // plan.start is still nullptr: the caller reports the failure
}
