#include "ResultView.h"

#include "Maze.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>

namespace
{
    const int ColumnGap = 6; // spaces between mazes drawn side by side
}

ResultView::ResultView(const Maze& maze, const std::vector<std::unique_ptr<Pathfinder>>& pathfinders)
    : m_maze(maze), m_pathfinders(pathfinders)
{
}

void ResultView::showSolveExplored(const Pathfinder& pathfinder, const PathResult& result) const
{
    std::cout << "\nEvery cell " << pathfinder.getShortName() << " explored:\n\n";
    m_maze.render(result.path, result.explored);
    printLegend(true);
}

void ResultView::showComparison(const std::vector<PathResult>& results) const
{
    printSideBySide(results, false);
    printStatsTable(results);
    printComparisonSummary(results);
}

void ResultView::showComparisonExplored(const std::vector<PathResult>& results) const
{
    std::cout << "\nEvery cell each algorithm explored:\n";
    printSideBySide(results, true);
    printLegend(true);
}
void ResultView::printSideBySide(const std::vector<PathResult>& results, bool showExplored) const
{
    // Each maze is drawn under its algorithm's name.
    const int columnWidth = m_maze.getCols() + ColumnGap;

    std::cout << "\n  ";
    for (const auto& pathfinder : m_pathfinders)
        std::cout << std::left << std::setw(columnWidth) << pathfinder->getShortName();
    std::cout << "\n\n";

    std::vector<std::vector<std::string>> drawings;
    for (const PathResult& result : results)
        drawings.push_back(m_maze.renderLines(result.path, showExplored ? result.explored : std::vector<Cell*>{}));

    for (int row = 0; row < m_maze.getRows(); ++row)
    {
        std::cout << "  ";
        for (const auto& drawing : drawings)
            std::cout << std::left << std::setw(columnWidth) << drawing[row];
        std::cout << '\n';
    }
}

void ResultView::printStatsTable(const std::vector<PathResult>& results) const
{
    std::cout << "\n  " << std::left << std::setw(30) << "Algorithm"
              << std::right << std::setw(13) << "Path length"
              << std::setw(17) << "Cells explored" << '\n'
              << "  " << std::string(60, '-') << '\n';

    for (size_t i = 0; i < results.size(); ++i)
    {
        std::cout << "  " << std::left << std::setw(30) << m_pathfinders[i]->getName() << std::right;
        if (results[i].found)
            std::cout << std::setw(13) << results[i].getPathLength();
        else
            std::cout << std::setw(13) << "no path";
        std::cout << std::setw(17) << results[i].getCellsExplored() << '\n';
    }
}

void ResultView::printComparisonSummary(const std::vector<PathResult>& results) const
{
    std::cout << '\n';
    if (!results[0].found)
    {
        std::cout << "No path exists, so every algorithm had to search every cell it could reach.\n";
        return;
    }

    size_t fewest = 0;
    for (size_t i = 1; i < results.size(); ++i)
    {
        if (results[i].getCellsExplored() < results[fewest].getCellsExplored())
            fewest = i;
    }

    const int baseline = results[0].getCellsExplored();
    const int saved = baseline - results[fewest].getCellsExplored();
    if (fewest == 0 || saved == 0)
    {
        std::cout << "Every algorithm explored the same number of cells on this maze.\n";
    }
    else
    {
        std::cout << m_pathfinders[fewest]->getShortName() << " explored " << saved << " fewer cells than "
                  << m_pathfinders[0]->getShortName() << " (" << (saved * 100 / baseline) << "% less).\n";
    }

    bool sameLength = std::all_of(results.begin(), results.end(), [&](const PathResult& r)
    {
        return r.getPathLength() == results[0].getPathLength();
    });
    if (sameLength)
        std::cout << "All paths are the same length, so each algorithm found a shortest path.\n";
}

void ResultView::printLegend(bool withExplored) const
{
    std::cout << "\nLegend: S = start, G = goal, # = wall, . = path";
    if (withExplored)
        std::cout << ", o = explored but not on the path";
    std::cout << '\n';
}

void ResultView::showSolve(const Pathfinder& pathfinder, const PathResult& result) const
{
    std::cout << "\nSolving with " << pathfinder.getName() << "...\n\n";

    if (!result.found)
    {
        std::cout << "No path exists from S to G. Walls block every route.\n"
                  << "Cells explored: " << result.getCellsExplored() << '\n';
        return;
    }

    m_maze.render(result.path);
    printLegend(false);
    std::cout << "Path length:    " << result.getPathLength() << " moves\n"
              << "Cells explored: " << result.getCellsExplored() << '\n';
}
