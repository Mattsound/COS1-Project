# Milestone 2 Progress Notes (in progress)

## Completed so far

- **`AStarPathfinder` (#8):** A* search using a `std::priority_queue` ordered by *moves so far + estimated moves left* (Manhattan distance). Ties go to the entry that has traveled further, which pushes the search toward the goal.
- **Shared indexing in `Maze`:** `indexOf()` and `getCellCount()` give every cell a number, so BFS and A* keep per-cell data (visited flags, best costs) in flat vectors instead of each doing the math itself.
- **Compare mode (#8):** runs every algorithm on the same maze, draws the solved mazes side by side (`Maze::renderLines()`), prints a path length / cells explored table, and sums it up, e.g. "A* explored 27 fewer cells than BFS (40% less)".
- **Algorithm choice in Solve a Maze:** `Menu` stores its algorithms as `std::vector<std::unique_ptr<Pathfinder>>`, so the menu works through the base class and adding a new algorithm is one line.
- **Usability (#13):** every prompt shows its valid range, errors say what was wrong, the main menu describes each option, and results stay on screen until Enter is pressed.
- **`MazeGenerator` (#9):** random "perfect" mazes built with the recursive backtracker (a randomized depth-first search using a `std::stack`) and a seeded `std::mt19937`. Difficulty sets the size: Easy 11 x 7, Medium 21 x 11, Hard 41 x 21.
- **Random mazes in the maze picker (#9):** pick a difficulty and the program generates a new maze and shows its seed, e.g. "Generated random maze #60376 (Hard, 41 x 21)". This works in both Solve a Maze and Compare mode.
- **Automated self-test:** `MazePathfinder.exe --selftest` runs 27 checks and exits with code 1 if any fail (see the test log).
- **Explored-cells overlay (#14):** after solving, the user can choose to see every cell the search explored (`o`) under the final path (`.`). In Compare mode the two overlays appear side by side, so you can *see* BFS flooding the maze while A* stays close to the goal. `PathResult` now stores the explored cells in order (`getCellsExplored()` returns the count, replacing the separate counter).
- **`InputHelper::readYesNo()`:** accepts y/yes/n/no in any capitalization and explains blank or unrecognized answers.
- **Refactor:** `compareAlgorithms()` went from about 80 lines to about 20 by moving the work into `printSideBySide()`, `printStatsTable()`, `printComparisonSummary()` and `printLegend()`. The overlay reuses `printSideBySide()`.

## Bugs & Challenges

| Problem | Cause | Fix |
|---|---|---|
| A* could explore the same cell twice | `std::priority_queue` can't update an entry that's already queued, so a cell gets pushed again when a shorter route to it turns up | Keep a `finished` flag per cell and skip outdated entries when they come off the queue |
| `priority_queue` returned the *worst* entry first | It's a max-heap by default | Wrote a comparator so the lowest score comes out first |
| "Breadth-First Search (BFS)" didn't fit as a column label above a 9-wide maze | The full name is wider than the maze | Added `getShortName()` to `Pathfinder` ("BFS", "A*") for column labels |
| Results scrolled away as soon as the menu reprinted | The menu printed straight after the results | Added `InputHelper::waitForEnter()`, shown only after results (going Back skips it) |
| How to prove a generated maze has no unreachable areas, without eyeballing hundreds of mazes | A missed room is hard to spot in a 41 x 21 maze | Math check: a perfect maze connects R rooms with exactly R - 1 knocked-down walls, so it must have exactly 2R - 1 open cells. The self-test checks this for 300 mazes. |
| Would the self-test actually catch a broken generator? | A test that always passes proves nothing | Temporarily broke the generator so it stopped knocking down walls. The self-test reported 9 failures and exit code 1, then went back to 21/21 once the code was restored. |
| Walls must sit *between* rooms | The backtracker moves 2 cells at a time | Rooms are at odd rows and columns, sizes are forced odd (and at least 5), and each step opens both the wall cell and the next room |

## Results

- On the built-in Medium maze, A* explores **39 cells vs. BFS's 66 (40% fewer)** and finds the same 30-move path.
- On random mazes A* still wins, e.g. **27% fewer** cells on a random Medium and **11% fewer** on a random Hard. The gap is smaller in generated mazes because their long winding corridors often force A* away from the goal, so the distance estimate helps less.

## Next

- Quick options after results (same maze / new maze / main menu)
