# Milestone 2: Build Prototype

**COS1 - Project and Portfolio I: Computer Science**
**Matt Moreno**
**Maze Pathfinder** (C++ console application)

This document records how Maze Pathfinder changed during Week 2. The plan behind these changes is in [MMoreno-Milestone2-Planning.md](MMoreno-Milestone2-Planning.md). Test evidence is in [testing/milestone2-test-log.md](testing/milestone2-test-log.md).

---

## Where the project started this week (end of Milestone 1)

- A menu loop with validated number input
- `Cell`, `Maze`, `Pathfinder` (abstract base class) and `BfsPathfinder`
- **Solve a Maze:** 3 hand-made mazes, solved with BFS only
- "Play mode" and "Compare algorithms" were "coming soon" placeholders

## Where it is now

- Two algorithms: **BFS and A\***
- **Compare mode** shows both algorithms on the same maze, side by side
- **Unlimited random mazes** at three difficulty levels, plus the 3 built-in mazes
- An **explored-cells overlay** that shows *how* each algorithm searched
- A **"What next?" menu** after results, for working with the same maze without starting over
- An automated **self-test** with 27 checks

---

## ⚙️ Features Added

| Feature | GitHub Issue | What it does |
|---|---|---|
| A* pathfinder | #8 | Finds the shortest path like BFS, but uses a Manhattan-distance estimate to explore the most promising cells first. On the built-in Medium maze it explores 39 cells vs. BFS's 66 (40% fewer) and finds the same 30-move path. |
| Compare mode | #8 | Runs every algorithm on the same maze, draws the solved mazes side by side, prints a path length / cells explored table, and sums up the result, e.g. "A* explored 27 fewer cells than BFS (40% less)". This replaced the "coming soon" placeholder. |
| Algorithm choice in Solve a Maze | #8 | Solve a Maze now asks which algorithm to use instead of always using BFS. |
| Random maze generation | #9 | Builds a new "perfect" maze (every cell reachable, exactly one route between any two points) with the recursive backtracker. Easy is 11 x 7, Medium 21 x 11 and Hard 41 x 21. The program shows each maze's seed, e.g. `Generated random maze #60376 (Hard, 41 x 21)`. |
| Explored-cells overlay | #14 | After solving, the user can see every cell the search explored (`o`) under the final path (`.`). In Compare mode the two overlays appear side by side, so you can see BFS spreading into dead ends while A* stays near the path. |
| "What next?" options | #15 | After results: solve this maze with an algorithm, compare algorithms on it, pick a new maze, or go back to the main menu. |
| Automated self-test | #9, #14 | `MazePathfinder.exe --selftest` runs 27 checks and exits with code 1 if any fail (details below). |

## 🧱 Updates to Code Structure

**New classes and files**

| File(s) | What it is | Why |
|---|---|---|
| `AStarPathfinder.h/.cpp` | A second `Pathfinder` subclass | Because `Pathfinder` was already an abstract base class, adding A* didn't require changing BFS or the maze code. |
| `MazeGenerator.h/.cpp` | Builds random maze layouts from a seed | Keeps "how to build a maze" out of `Maze`, the same way `Pathfinder` keeps "how to solve a maze" out of it. It returns the same text layout format the built-in mazes use, so `Maze::loadFromLayout()` didn't need to change. |
| `SelfTest.h/.cpp` | Automated checks, started with `--selftest` | Random mazes are impossible to check by hand, so the program checks itself. |

**Changes to existing classes**

| Class | Change | Why |
|---|---|---|
| `Menu` | Stores the algorithms as `std::vector<std::unique_ptr<Pathfinder>>` | The menu runs any algorithm through the base class. Adding one is a single line in the constructor. |
| `Menu` | Solve and Compare now run as a session loop (`runMazeSession`) on the loaded maze, split into `solveCurrentMaze()` and `compareCurrentMaze()` | This is what makes "What next?" possible without repeating code. |
| `Menu` | `compareAlgorithms()` (about 80 lines) was split into `printSideBySide()`, `printStatsTable()`, `printComparisonSummary()` and `printLegend()` | Each part is easier to read, and the overlay reuses `printSideBySide()`. The Milestone 2 plan flagged this function as "getting messy". |
| `Maze` | Added `indexOf()` and `getCellCount()` | BFS and A* keep per-cell data (visited flags, best costs) in flat vectors using the same indexing. |
| `Maze` | Added `renderLines()`, and `render()` takes an optional explored list | `renderLines()` returns the drawing as text rows so two mazes can be placed side by side. The explored list draws the overlay. |
| `PathResult` | Replaced the `cellsExplored` counter with `std::vector<Cell*> explored` plus `getCellsExplored()` | The overlay needs to know *which* cells were explored, not just how many. Storing the list and counting it means the two can never disagree. |
| `Pathfinder` | Added `getShortName()` ("BFS", "A*") | The full name "Breadth-First Search (BFS)" is wider than a small maze, so it couldn't be used as a column label. |
| `InputHelper` | Added `readYesNo()` and `waitForEnter()`; `readInt()` shows the valid range and better errors | Reusable, validated input for the new prompts (see Usability). |

**New data structures used this week**

- `std::priority_queue` with a custom comparator: A*'s open list
- `std::stack<std::pair<int,int>>`: the maze generator's backtracking path
- `std::mt19937` with `std::uniform_int_distribution`: seeded random numbers, so the same seed rebuilds the same maze
- `std::unique_ptr`: the menu owns its algorithms through base-class pointers

## ✨ Usability Improvements

All of these change what the *user* sees, not just the code:

| Before | After | Issue |
|---|---|---|
| `Choose an option:` | `Choose an option (1-4):`. Every prompt shows its valid range. | #13 |
| `Please enter a whole number.` for anything wrong | Specific messages: `"abc" isn't a whole number...`, `9 isn't an option...`, or for a blank line, `Type a number from 1-4, then press Enter.` | #13 |
| A bare list of menu options | Each main menu option has a one-line description of what it does | #13 |
| Results scrolled off-screen as soon as the menu reprinted | Results stay on screen until the user picks what to do next | #13, #15 |
| Play mode printed "[Play mode] Coming soon" | It explains that the feature isn't available yet and suggests what to try instead | #13 |
| Trying A* on a maze you just solved with BFS took 3 menus | One choice in "What next?" | #15 |
| Only 3 mazes to choose from | Pick "Random maze", then a difficulty with its size shown, e.g. `2. Medium (21 x 11)` | #9 |
| A yes/no question would only accept an exact answer | `readYesNo()` accepts y/yes/n/no in any capitalization and explains anything else | #14 |

## 🧪 Testing This Week

- **Self-test (27 checks, all passing):**
  - 100 random mazes per difficulty: every room is connected, both algorithms find a path, the path lengths match, the explored lists are valid, and the same seed rebuilds the same maze
  - the no-path maze: both searches explore all 10 reachable cells before giving up
  - all 5 `Maze::loadFromLayout()` error cases. These were "not yet tested" at the end of Milestone 1.
- **Testing the tests:** I broke the generator on purpose. The self-test reported 9 failures and exit code 1, then went back to all passing once I restored the code.
- **Menu tests:** 35 scripted cases in the test log, all passing. They cover bad input at every prompt, every Back option, and input ending at every kind of prompt.

## 💭 What Changed and Why

- **Why A* and Compare mode first:** comparing algorithms is the core idea of the project, and it was a "coming soon" placeholder. Building it proved the `Pathfinder` design from Milestone 1: the second algorithm plugged in without touching the first.
- **Why random mazes next:** with only 3 mazes, the program got repetitive and the comparison only had 3 data points. Random mazes added variety and showed something interesting: A*'s advantage shrinks in generated mazes (11-27% fewer cells instead of 40%), because their long winding corridors often force it away from the goal.
- **Why the explored-cells overlay:** "A* explored 40% fewer cells" was just a number. The overlay shows the reason, which is the heuristic.
- **Why a self-test:** I couldn't check hundreds of random mazes by eye. Writing tests that check the generator mathematically (2R − 1 open cells for R rooms) was more reliable than looking at a few.
- **What changed from the plan:** the plan said `Maze` might need changes for random mazes, but making `MazeGenerator` output the same text layout as the presets meant `Maze` didn't change at all. I also planned to keep the "Press Enter" pause, but the "What next?" menu does the same job better, so the pause is now used only for Play mode.

## 📋 Project Management

- Issues completed this week: **#8, #9, #13, #14, #15**. Each was moved To Do → In Progress → Done on the board as it was worked on.
- Remaining in Backlog: **#10 Play mode** (Milestone 3) and **#11 Save/Load** (Week 4).
- Pull Requests: **#16** merged this week's development into `main`, and a final PR adds this document and the Week 2 stand-up.
