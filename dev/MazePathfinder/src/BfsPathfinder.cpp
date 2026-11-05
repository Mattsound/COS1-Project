#include "BfsPathfinder.h"

#include "Maze.h"

#include <queue>

PathResult BfsPathfinder::findPath(Maze& maze)
{
    PathResult result;

    Cell* start = maze.getStart();
    Cell* goal = maze.getGoal();
    if (start == nullptr || goal == nullptr)
        return result;

    maze.clearParents();

    // One flag per cell so each cell is queued at most once.
    std::vector<bool> visited(maze.getCellCount(), false);

    std::queue<Cell*> frontier;
    frontier.push(start);
    visited[maze.indexOf(*start)] = true;

    while (!frontier.empty())
    {
        Cell* current = frontier.front();
        frontier.pop();
        ++result.cellsExplored;

        if (current == goal)
        {
            result.found = true;
            result.path = buildPath(goal);
            return result;
        }

        for (Cell* next : maze.getNeighbors(*current))
        {
            if (visited[maze.indexOf(*next)])
                continue;

            visited[maze.indexOf(*next)] = true;
            next->setParent(current);
            frontier.push(next);
        }
    }

    return result; // the frontier ran out before reaching the goal: no path
}
