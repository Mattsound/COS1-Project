#include "Cell.h"

Cell::Cell(int row, int col, CellType type)
    : m_row(row), m_col(col), m_type(type)
{
}

char Cell::getSymbol() const
{
    switch (m_type)
    {
    case CellType::Wall:  return '#';
    case CellType::Start: return 'S';
    case CellType::Goal:  return 'G';
    default:              return ' ';
    }
}
