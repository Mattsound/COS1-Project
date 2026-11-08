#pragma once

#include "Pathfinder.h"

// A* search. Like BFS it finds a shortest path, but it uses a heuristic (an
// estimate of the distance left to the goal) to explore the most promising
// cells first, so it usually explores far fewer cells than BFS.
class AStarPathfinder : public Pathfinder
{
public:
    std::string getName() const override { return "A* Search"; }
    std::string getShortName() const override { return "A*"; }
    PathResult findPath(Maze& maze) override;

private:
    // Manhattan distance: rows apart plus columns apart. It never overestimates
    // the real distance when moves are only up, down, left and right, which is
    // what guarantees A* still finds a shortest path.
    static int estimateDistance(const Cell& from, const Cell& to);
};
