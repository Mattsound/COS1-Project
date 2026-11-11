#include "Pathfinder.h"

#include "Cell.h"
#include "Maze.h"

#include <algorithm>

PathResult Pathfinder::findPath(Maze& maze)
{
    return findPathBetween(maze, maze.getStart(), maze.getGoal());
}

std::vector<Cell*> Pathfinder::buildPath(Cell* goal)
{
    std::vector<Cell*> path;
    for (Cell* cell = goal; cell != nullptr; cell = cell->getParent())
        path.push_back(cell);

    std::reverse(path.begin(), path.end());
    return path;
}
