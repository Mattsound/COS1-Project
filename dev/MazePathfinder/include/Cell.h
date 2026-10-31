#pragma once

enum class CellType
{
    Open,
    Wall,
    Start,
    Goal
};

// One square of the maze. The parent pointer lets a pathfinder walk back
// from the goal to the start to rebuild the path it found.
class Cell
{
public:
    Cell(int row, int col, CellType type = CellType::Open);

    int getRow() const { return m_row; }
    int getCol() const { return m_col; }

    CellType getType() const { return m_type; }
    void setType(CellType type) { m_type = type; }
    bool isWalkable() const { return m_type != CellType::Wall; }

    Cell* getParent() const { return m_parent; }
    void setParent(Cell* parent) { m_parent = parent; }

    // The character used to draw this cell in the console.
    char getSymbol() const;

private:
    int m_row;
    int m_col;
    CellType m_type;
    Cell* m_parent = nullptr; // not owned; points into the Maze's grid
};
