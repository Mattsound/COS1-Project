#pragma once

class Cell;
class Maze;
class Pathfinder;

// The chaser in Play mode. Each turn it asks its pathfinder for the shortest
// route to its target (the player) and moves along it.
class Enemy
{
public:
    // stepsPerTurn is how many cells it moves each turn; higher is harder.
    Enemy(Cell* start, Pathfinder& pathfinder, int stepsPerTurn = 1);

    Cell* getCell() const { return m_cell; }
    int getStepsPerTurn() const { return m_stepsPerTurn; }

    // Moves up to stepsPerTurn cells toward target, stopping early if it
    // reaches it. Stays put if no route connects them. Returns true if the
    // enemy is on the target afterward (it caught the player).
    bool takeTurn(Maze& maze, Cell* target);

private:
    Cell* m_cell;              // not owned; points into the Maze's grid
    Pathfinder* m_pathfinder;  // not owned; any algorithm works
    int m_stepsPerTurn;
};
