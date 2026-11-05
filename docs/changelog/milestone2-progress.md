# Milestone 2 Progress Notes (in progress)

## Completed so far

- **`AStarPathfinder` (#8):** A* search using a `std::priority_queue` ordered by *moves so far + estimated moves left* (Manhattan distance). Ties go to the entry that has traveled further, which pushes the search toward the goal.
- **Shared indexing in `Maze`:** `indexOf()` and `getCellCount()` give every cell a number, so BFS and A* keep per-cell data (visited flags, best costs) in flat vectors instead of each doing the math itself.
- **Compare mode (#8):** runs every algorithm on the same maze, draws the solved mazes side by side (`Maze::renderLines()`), prints a path length / cells explored table, and sums it up, e.g. "A* explored 27 fewer cells than BFS (40% less)".
- **Algorithm choice in Solve a Maze:** `Menu` stores its algorithms as `std::vector<std::unique_ptr<Pathfinder>>`, so the menu works through the base class and adding a new algorithm is one line.
- **Usability (#13):** every prompt shows its valid range, errors say what was wrong, the main menu describes each option, and results stay on screen until Enter is pressed.

## Bugs & Challenges

| Problem | Cause | Fix |
|---|---|---|
| A* could explore the same cell twice | `std::priority_queue` can't update an entry that's already queued, so a cell gets pushed again when a shorter route to it turns up | Keep a `finished` flag per cell and skip outdated entries when they come off the queue |
| `priority_queue` returned the *worst* entry first | It's a max-heap by default | Wrote a comparator so the lowest score comes out first |
| "Breadth-First Search (BFS)" didn't fit as a column label above a 9-wide maze | The full name is wider than the maze | Added `getShortName()` to `Pathfinder` ("BFS", "A*") for column labels |
| Results scrolled away as soon as the menu reprinted | The menu printed straight after the results | Added `InputHelper::waitForEnter()`, shown only after results (going Back skips it) |

## Results

On the Medium maze, A* explores **39 cells vs. BFS's 66 (40% fewer)** and finds the same 30-move path.

## Next

- #9 Random maze generation with difficulty levels
- More test cases for `Maze::loadFromLayout`'s error handling once random mazes exist
