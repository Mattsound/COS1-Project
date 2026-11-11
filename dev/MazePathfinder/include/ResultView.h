#pragma once

#include "Pathfinder.h"

#include <memory>
#include <vector>

class Maze;

// Prints Solve and Compare results. Menu decides *what* to show and when;
// ResultView decides how it looks on screen.
class ResultView
{
public:
    // Keeps references, so maze and pathfinders must outlive the view (both
    // are members of Menu, declared before its ResultView).
    ResultView(const Maze& maze, const std::vector<std::unique_ptr<Pathfinder>>& pathfinders);

    // Solve a maze: the solved maze with its path and stats.
    void showSolve(const Pathfinder& pathfinder, const PathResult& result) const;
    void showSolveExplored(const Pathfinder& pathfinder, const PathResult& result) const;

    // Compare mode: every algorithm side by side, a stats table and a summary.
    void showComparison(const std::vector<PathResult>& results) const;
    void showComparisonExplored(const std::vector<PathResult>& results) const;

private:
    void printSideBySide(const std::vector<PathResult>& results, bool showExplored) const;
    void printStatsTable(const std::vector<PathResult>& results) const;
    void printComparisonSummary(const std::vector<PathResult>& results) const;
    void printLegend(bool withExplored) const;

    const Maze& m_maze;
    const std::vector<std::unique_ptr<Pathfinder>>& m_pathfinders;
};
