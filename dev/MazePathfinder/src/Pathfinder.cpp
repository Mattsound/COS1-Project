#include "Pathfinder.h"

#include "Cell.h"

#include <algorithm>

std::vector<Cell*> Pathfinder::buildPath(Cell* goal)
{
    std::vector<Cell*> path;
    for (Cell* cell = goal; cell != nullptr; cell = cell->getParent())
        path.push_back(cell);

    std::reverse(path.begin(), path.end());
    return path;
}
