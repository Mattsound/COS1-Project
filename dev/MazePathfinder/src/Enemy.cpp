#include "Enemy.h"

#include "Pathfinder.h"

#include <algorithm>

Enemy::Enemy(Cell* start, Pathfinder& pathfinder, int stepsPerTurn)
    : m_cell(start), m_pathfinder(&pathfinder), m_stepsPerTurn(std::max(1, stepsPerTurn))
{
}

bool Enemy::takeTurn(Maze& maze, Cell* target)
{
    if (m_cell == target)
        return true;

    // Re-plan every turn, because the player moves between turns.
    PathResult result = m_pathfinder->findPathBetween(maze, m_cell, target);
    if (!result.found)
        return false; // walled off from the player, so it can't reach them

    // path[0] is where the enemy stands now; step forward from there.
    const int steps = std::min(m_stepsPerTurn, result.getPathLength());
    m_cell = result.path[steps];

    return m_cell == target;
}
