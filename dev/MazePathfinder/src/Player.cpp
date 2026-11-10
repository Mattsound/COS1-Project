#include "Player.h"

#include "Maze.h"

#include <cctype>

Player::Player(Cell* start)
    : m_cell(start)
{
}

bool Player::move(Maze& maze, Direction direction)
{
    int row = m_cell->getRow();
    int col = m_cell->getCol();

    switch (direction)
    {
    case Direction::Up:    --row; break;
    case Direction::Down:  ++row; break;
    case Direction::Left:  --col; break;
    case Direction::Right: ++col; break;
    }

    Cell* next = maze.getCell(row, col); // nullptr if off the grid
    if (next == nullptr || !next->isWalkable())
        return false;

    m_cell = next;
    ++m_moveCount;
    return true;
}

bool Player::parseDirection(char key, Direction& direction)
{
    switch (std::tolower(static_cast<unsigned char>(key)))
    {
    case 'w': direction = Direction::Up;    return true;
    case 's': direction = Direction::Down;  return true;
    case 'a': direction = Direction::Left;  return true;
    case 'd': direction = Direction::Right; return true;
    default:  return false;
    }
}
