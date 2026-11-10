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
    std::vector<Cell*> path;     // start to goal, both included; empty if no path
    std::vector<Cell*> explored; // every cell the search took off its frontier, in order

    // Number of moves from start to goal.
    int getPathLength() const { return path.empty() ? 0 : static_cast<int>(path.size()) - 1; }

    int getCellsExplored() const { return static_cast<int>(explored.size()); }
};

// Base class for every search algorithm. The menu, Compare mode and the Play
// mode enemy all work through this interface, so a new algorithm only has to
// override findPathBetween() and the two name functions.
class Pathfinder
{
public:
    virtual ~Pathfinder() = default;

    virtual std::string getName() const = 0;
    virtual std::string getShortName() const = 0; // fits above a narrow maze

    // Shortest path from the maze's start (S) to its goal (G).
    PathResult findPath(Maze& maze);

    // Shortest path between any two cells, e.g. from the enemy to the player.
    // Returns a result with found == false if either cell is missing or no
    // route connects them.
    virtual PathResult findPathBetween(Maze& maze, Cell* start, Cell* goal) = 0;

protected:
    // Follows parent pointers from the goal back to the start, then reverses
    // the list so it runs start to goal.
    static std::vector<Cell*> buildPath(Cell* goal);
};
