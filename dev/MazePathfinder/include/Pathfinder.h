#pragma once

#include <string>
#include <vector>

class Cell;
class Maze;

// What a search produced, so results from different algorithms can be
// displayed and compared the same way.
struct PathResult
{
    bool found = false;
    std::vector<Cell*> path; // start to goal, both included; empty if no path
    int cellsExplored = 0;   // how many cells the search took off its frontier

    // Number of moves from start to goal.
    int getPathLength() const { return path.empty() ? 0 : static_cast<int>(path.size()) - 1; }
};

// Base class for every search algorithm. The menu (and Compare mode later)
// works through this interface, so a new algorithm only has to override
// findPath() and getName().
class Pathfinder
{
public:
    virtual ~Pathfinder() = default;

    virtual std::string getName() const = 0;
    virtual PathResult findPath(Maze& maze) = 0;

protected:
    // Follows parent pointers from the goal back to the start, then reverses
    // the list so it runs start to goal.
    static std::vector<Cell*> buildPath(Cell* goal);
};
