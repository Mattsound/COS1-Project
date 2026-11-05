#include "AStarPathfinder.h"

#include "Maze.h"

#include <climits>
#include <cstdlib>
#include <queue>

namespace
{
    // One entry in the open list. The same cell can be added more than once if
    // a shorter route to it turns up later; the older entries are skipped.
    struct OpenEntry
    {
        int priority; // moves so far + estimated moves left (A*'s "f" score)
        int cost;     // moves so far from the start ("g" score)
        Cell* cell;
    };

    // std::priority_queue puts the "largest" item on top, so this ordering
    // makes the entry with the LOWEST priority come out first. On a tie, the
    // entry that has traveled further wins, because it is closer to the goal.
    struct ComesOutLater
    {
        bool operator()(const OpenEntry& a, const OpenEntry& b) const
        {
            if (a.priority != b.priority)
                return a.priority > b.priority;
            return a.cost < b.cost;
        }
    };
}

PathResult AStarPathfinder::findPath(Maze& maze)
{
    PathResult result;

    Cell* start = maze.getStart();
    Cell* goal = maze.getGoal();
    if (start == nullptr || goal == nullptr)
        return result;

    maze.clearParents();

    std::vector<int> bestCost(maze.getCellCount(), INT_MAX); // shortest known moves to each cell
    std::vector<bool> finished(maze.getCellCount(), false);  // cells already explored

    std::priority_queue<OpenEntry, std::vector<OpenEntry>, ComesOutLater> open;
    bestCost[maze.indexOf(*start)] = 0;
    open.push({ estimateDistance(*start, *goal), 0, start });

    while (!open.empty())
    {
        OpenEntry entry = open.top();
        open.pop();

        size_t index = maze.indexOf(*entry.cell);
        if (finished[index])
            continue; // an outdated entry; this cell was already explored by a shorter route

        finished[index] = true;
        ++result.cellsExplored;

        if (entry.cell == goal)
        {
            result.found = true;
            result.path = buildPath(goal);
            return result;
        }

        for (Cell* next : maze.getNeighbors(*entry.cell))
        {
            size_t nextIndex = maze.indexOf(*next);
            if (finished[nextIndex])
                continue;

            int newCost = entry.cost + 1;
            if (newCost < bestCost[nextIndex])
            {
                bestCost[nextIndex] = newCost;
                next->setParent(entry.cell);
                open.push({ newCost + estimateDistance(*next, *goal), newCost, next });
            }
        }
    }

    return result; // the open list ran out before reaching the goal: no path
}

int AStarPathfinder::estimateDistance(const Cell& from, const Cell& to)
{
    return std::abs(from.getRow() - to.getRow()) + std::abs(from.getCol() - to.getCol());
}
