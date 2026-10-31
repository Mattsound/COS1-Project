#include "Maze.h"

#include <iostream>
#include <utility>

namespace
{
    const char PathSymbol = '.';
}

bool Maze::loadFromLayout(const std::vector<std::string>& layout, std::string& error)
{
    clear();

    if (layout.empty() || layout[0].empty())
    {
        error = "The maze layout is empty.";
        return false;
    }

    const size_t width = layout[0].size();
    int startCount = 0;
    int goalCount = 0;

    for (size_t row = 0; row < layout.size(); ++row)
    {
        if (layout[row].size() != width)
        {
            error = "Row " + std::to_string(row + 1) + " is a different length than the first row.";
            clear();
            return false;
        }

        std::vector<Cell> cells;
        cells.reserve(width);
        for (size_t col = 0; col < width; ++col)
        {
            CellType type = CellType::Open;
            switch (layout[row][col])
            {
            case '#': type = CellType::Wall; break;
            case 'S': type = CellType::Start; ++startCount; break;
            case 'G': type = CellType::Goal; ++goalCount; break;
            case ' ': type = CellType::Open; break;
            default:
                error = "Unknown character '" + std::string(1, layout[row][col]) +
                        "' at row " + std::to_string(row + 1) + ", column " + std::to_string(col + 1) + ".";
                clear();
                return false;
            }
            cells.emplace_back(static_cast<int>(row), static_cast<int>(col), type);
        }
        m_grid.push_back(std::move(cells));
    }

    if (startCount != 1 || goalCount != 1)
    {
        error = "A maze needs exactly one start (S) and one goal (G).";
        clear();
        return false;
    }

    // Find the start and goal only after the grid is complete, because adding
    // rows can move the cells in memory and break any pointers taken earlier.
    for (auto& row : m_grid)
    {
        for (Cell& cell : row)
        {
            if (cell.getType() == CellType::Start)
                m_start = &cell;
            else if (cell.getType() == CellType::Goal)
                m_goal = &cell;
        }
    }

    return true;
}

Cell* Maze::getCell(int row, int col)
{
    if (row < 0 || row >= getRows() || col < 0 || col >= getCols())
        return nullptr;

    return &m_grid[row][col];
}

std::vector<Cell*> Maze::getNeighbors(const Cell& cell)
{
    // Up, down, left, right. No diagonal moves.
    const int rowOffsets[] = { -1, 1, 0, 0 };
    const int colOffsets[] = { 0, 0, -1, 1 };

    std::vector<Cell*> neighbors;
    for (int i = 0; i < 4; ++i)
    {
        Cell* next = getCell(cell.getRow() + rowOffsets[i], cell.getCol() + colOffsets[i]);
        if (next != nullptr && next->isWalkable())
            neighbors.push_back(next);
    }
    return neighbors;
}

void Maze::clearParents()
{
    for (auto& row : m_grid)
        for (Cell& cell : row)
            cell.setParent(nullptr);
}

void Maze::render(const std::vector<Cell*>& path) const
{
    // Draw into text rows first so the path can be laid over the maze.
    std::vector<std::string> canvas;
    canvas.reserve(m_grid.size());
    for (const auto& row : m_grid)
    {
        std::string line;
        line.reserve(row.size());
        for (const Cell& cell : row)
            line += cell.getSymbol();
        canvas.push_back(line);
    }

    for (const Cell* cell : path)
    {
        if (cell->getType() == CellType::Open)
            canvas[cell->getRow()][cell->getCol()] = PathSymbol;
    }

    for (const std::string& line : canvas)
        std::cout << "  " << line << '\n';
}

void Maze::clear()
{
    m_grid.clear();
    m_start = nullptr;
    m_goal = nullptr;
}
