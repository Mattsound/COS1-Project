#pragma once

#include "Cell.h"

#include <string>
#include <vector>

// The grid of cells that the pathfinders search. Built from a text layout where
// '#' is a wall, 'S' is the start, 'G' is the goal, and a space is open floor.
class Maze
{
public:
    Maze() = default;

    // Cells hold pointers into m_grid, so copying a Maze would leave the copy
    // pointing at the original's cells.
    Maze(const Maze&) = delete;
    Maze& operator=(const Maze&) = delete;

    // Builds the grid from the layout. Returns false and fills in error if the
    // layout is empty, has rows of different lengths, uses an unknown
    // character, or doesn't have exactly one start and one goal.
    bool loadFromLayout(const std::vector<std::string>& layout, std::string& error);

    int getRows() const { return static_cast<int>(m_grid.size()); }
    int getCols() const { return m_grid.empty() ? 0 : static_cast<int>(m_grid[0].size()); }
    bool isLoaded() const { return !m_grid.empty(); }
    size_t getCellCount() const { return static_cast<size_t>(getRows()) * getCols(); }

    // A unique number from 0 to getCellCount() - 1 for each cell, so
    // pathfinders can keep per-cell data in a flat vector.
    size_t indexOf(const Cell& cell) const
    {
        return static_cast<size_t>(cell.getRow()) * getCols() + cell.getCol();
    }

    // Returns nullptr if the position is outside the grid.
    Cell* getCell(int row, int col);

    Cell* getStart() const { return m_start; }
    Cell* getGoal() const { return m_goal; }

    // The walkable cells directly up, down, left and right of the given cell.
    std::vector<Cell*> getNeighbors(const Cell& cell);

    // Clears every cell's parent pointer so a new search starts fresh.
    void clearParents();

    // Draws the maze in the console. Cells on the path are drawn as '.' and
    // other explored cells as 'o'; the start and goal keep their letters.
    void render(const std::vector<Cell*>& path = {}, const std::vector<Cell*>& explored = {}) const;

    // The same drawing as render(), returned as text rows so callers can
    // place two mazes side by side.
    std::vector<std::string> renderLines(const std::vector<Cell*>& path = {},
                                         const std::vector<Cell*>& explored = {}) const;

    // A character drawn on top of one cell, like 'P' for the player.
    struct Marker
    {
        const Cell* cell;
        char symbol;
    };

    // Draws the maze with markers on top. Later markers win if two share a cell.
    void renderWithMarkers(const std::vector<Marker>& markers) const;

private:
    void clear();

    std::vector<std::vector<Cell>> m_grid;
    Cell* m_start = nullptr; // not owned; points into m_grid
    Cell* m_goal = nullptr;  // not owned; points into m_grid
};
