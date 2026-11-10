#pragma once

class Cell;
class Maze;

enum class Direction
{
    Up,
    Down,
    Left,
    Right
};

// The person playing Play mode. Moves one cell at a time and can't walk
// through walls or off the edge of the maze.
class Player
{
public:
    explicit Player(Cell* start);

    Cell* getCell() const { return m_cell; }
    int getMoveCount() const { return m_moveCount; }

    // Moves one cell in the given direction. Returns false and stays put if a
    // wall or the edge of the maze is in the way, so a blocked move doesn't
    // count as a move.
    bool move(Maze& maze, Direction direction);

    // Turns W/A/S/D (any capitalization) into a direction. Returns false for
    // any other key.
    static bool parseDirection(char key, Direction& direction);

private:
    Cell* m_cell;         // not owned; points into the Maze's grid
    int m_moveCount = 0;
};
