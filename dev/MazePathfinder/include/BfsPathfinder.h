#pragma once

#include "Pathfinder.h"

// Breadth-First Search. It explores the maze in rings outward from the start,
// so the first time it reaches the goal it has found a shortest path.
class BfsPathfinder : public Pathfinder
{
public:
    std::string getName() const override { return "Breadth-First Search (BFS)"; }
    PathResult findPath(Maze& maze) override;
};
