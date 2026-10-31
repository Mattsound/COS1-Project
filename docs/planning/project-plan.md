# Project Plan: Maze Pathfinder

## What problem or system does the program model?

The program models **pathfinding**: finding the shortest route from a start point to a goal through a grid with obstacles. That's the same problem GPS navigation, robot movement and video game AI solve. The program shows a maze in the console as ASCII art, solves it with a search algorithm, and draws the path it found. Later milestones add A* so the two algorithms can be compared, and a Play mode where the user races an AI enemy that uses pathfinding to chase them.

## What classes are needed?

| Class | Responsibility |
|---|---|
| `Menu` | Runs the main menu loop and sends each choice to the right feature. |
| `Cell` | One square of the maze: its position, type (open, wall, start, goal) and a parent pointer used to rebuild a path. |
| `Maze` | Owns the grid of `Cell` objects. Builds it from a text layout, answers questions like "which neighbors can I walk to?" and draws itself in the console. |
| `Pathfinder` | Abstract base class for search algorithms. Every algorithm takes a `Maze` and returns a `PathResult`. |
| `BfsPathfinder` | Breadth-First Search. It always finds the shortest path in an unweighted grid. |
| `AStarPathfinder` *(Milestone 2)* | A* search with a Manhattan-distance heuristic. It usually explores fewer cells than BFS. |
| `Player` / `Enemy` *(Milestone 2-3)* | Play mode: the user moves the player, and the enemy uses a `Pathfinder` to chase them. |
| `InputHelper` (namespace) | Reusable, validated console input. |
| `MazePresets` (namespace) | The built-in maze layouts. |

## What data structures are used?

- `std::vector<std::vector<Cell>>`: the maze grid (rows of cells).
- `std::vector<std::string>`: maze layouts as text, one string per row.
- `std::queue<Cell*>`: the BFS frontier.
- `std::vector<bool>`: which cells BFS has already visited.
- `std::vector<Cell*>`: the finished path from start to goal.
- **Pointers:** each `Cell` has a `Cell* m_parent` that points back into the maze grid, and the path is built by following those pointers from the goal back to the start.
- *(Later)* `std::priority_queue` for A*, and file streams (`std::ifstream`/`std::ofstream`) for Save/Load in Week 4.

## How does the user interact with the program?

Through a numbered console menu that keeps coming back until the user picks Exit:

1. **Solve a maze**: pick a built-in maze, see it drawn, then see it solved with the path marked and stats (path length, cells explored).
2. **Play mode** *(Milestone 2-3)*: move with W/A/S/D and reach the goal before the enemy catches you.
3. **Compare algorithms** *(Milestone 2)*: run BFS and A* on the same maze and compare cells explored and path length.
4. **Exit**

Every input is validated, so letters, blank lines, out-of-range numbers and end-of-input never crash the program or loop forever.

## What is the minimum version that must work first?

1. The menu loop with validated input and a clean exit.
2. A `Maze` built from a text layout, made of `Cell` objects, that draws itself as ASCII.
3. **One working pathfinder (BFS)** that finds the shortest path, draws it on the maze and reports when there is no path.

Everything else (A*, Compare mode, Play mode, random mazes, Save/Load) builds on these three pieces.

## Milestone roadmap

| Milestone | Focus | GitHub Issues |
|---|---|---|
| 1. Foundation | Project structure, menu loop, input validation, `Cell`, `Maze`, BFS | #1-#7 |
| 2. Core Development | A* and Compare mode, random maze generation | #8, #9 |
| 3. Expansion & Refinement | Play mode with an enemy chase, edge cases, refactoring | #10 |
| 4. File I/O & Final | Save/Load mazes (handling missing files), documentation, final PR | #11 |
