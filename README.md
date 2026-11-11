# Maze Pathfinder

**Project and Portfolio I: Computer Science (COS1)**, a C++ console application by Matt Moreno

Maze Pathfinder draws mazes in the console as ASCII art and solves them with real pathfinding algorithms. The user picks a maze and watches the program find the shortest route from the start (`S`) to the goal (`G`). It can compare how efficiently different algorithms search, and later the user can race an AI enemy that uses the same pathfinding to chase them. The project models the problem behind GPS navigation, robot movement and video game AI: getting from A to B around obstacles.

```
  #########          #########
  #S  #   #          #S..#   #
  # # # # #          # #.# # #
  # #   # #   BFS    # #...# #
  # ### # #   --->   # ###.# #
  #   #  G#          #   #..G#
  #########          #########

  Path length: 10 moves   Cells explored: 21
```

## Core Features

| Feature | Status |
|---|---|
| Persistent menu with validated input and clear error messages | ✅ Done |
| **What next? options:** after results, solve the same maze with another algorithm, compare on it, or pick a new maze without going back to the main menu | ✅ Done |
| **Solve a maze:** pick a built-in maze and an algorithm (BFS or A*), then see the solved path, its length and the number of cells explored | ✅ Done |
| Detects and reports mazes with no possible path | ✅ Done |
| **Compare algorithms:** BFS vs. A* on the same maze, drawn side by side with a stats table | ✅ Done |
| **Random mazes:** Easy, Medium or Hard, generated with the recursive backtracker and shown with their seed | ✅ Done |
| **Explored-cells overlay:** see every cell each algorithm searched (`o`) under the path (`.`), side by side in Compare mode | ✅ Done |
| Automated self-test (`MazePathfinder.exe --selftest`): 27 checks covering 300 generated mazes, explored-cell lists, the no-path maze and layout errors | ✅ Done |
| **Play mode:** reach G with W/A/S/D before an enemy that chases you with A* catches you; Easy, Medium or Hard, always winnable | ✅ Done |
| Save and load mazes and scores to files | 📝 Milestone 4 |

## Class Plan

| Class | Responsibility | Status |
|---|---|---|
| `Menu` | Runs the main menu loop, reads choices, and decides what happens next in each mode | ✅ |
| `ResultView` | Prints Solve and Compare results: solved mazes, the explored overlay, side-by-side mazes and the stats table | ✅ |
| `Cell` | One square of the maze: position, type (open, wall, start, goal) and a parent pointer used to rebuild paths | ✅ |
| `Maze` | Owns the `vector<vector<Cell>>` grid. Builds it from a text layout, finds walkable neighbors and draws itself as ASCII | ✅ |
| `Pathfinder` | Abstract base class, so every search algorithm shares one interface | ✅ |
| `BfsPathfinder` | Breadth-First Search, which always finds the shortest path | ✅ |
| `AStarPathfinder` | A* search with a Manhattan-distance heuristic, which usually explores far fewer cells than BFS | ✅ |
| `MazeGenerator` | Builds random perfect mazes from a seed with the recursive backtracker (randomized DFS) | ✅ |
| `Player` | Play mode: moves one cell at a time with W/A/S/D, refusing walls and the grid edge | ✅ |
| `Enemy` | Play mode: re-plans with a `Pathfinder` every turn and moves toward the player (speed is adjustable) | ✅ |
| `Game` | Play mode rules (turns, win/lose, difficulty) and a fair enemy start with a calculated head start; no console I/O, so it can be self-tested | ✅ |

Helpers: `InputHelper` (validated console input), `MazePresets` (built-in maze layouts) and `SelfTest` (automated checks).

## Repository Structure

```
dev/       Visual Studio solution and C++ project (include/ = headers, src/ = source)
docs/      planning/, testing/, changelog/, and the Version Control reference
images/    screenshots
```

## Build & Run

1. Open `dev/MazePathfinder.sln` in Visual Studio 2022 or newer.
2. Select the **x64** platform and press **Ctrl + F5**.

**Command-line options** (run `dev\x64\Debug\MazePathfinder.exe` from a terminal):
- `--selftest`: runs the automated checks and exits with code 0 if all pass
- `--seed 12345`: every random maze uses this seed, so a maze can be shared or replayed

---

# Project Stand-Ups

## Week 1

### 🔎 Overview
I set up the repository (the `main` and `dev` branches, the folder structure and the Project board) and built the foundation of the program. That means a menu loop with validated input, the `Cell` and `Maze` classes, an abstract `Pathfinder` base class, and a working Breadth-First Search. **Solve a Maze** works from the menu: pick a maze, see it drawn, then see the shortest path marked with its length and how many cells were explored.

### 🧟 Challenges
- **Menu infinite loop:** typing a letter put `std::cin` into a failed state, so the menu reprinted forever. I fixed it by reading whole lines with `getline` and parsing them safely in a reusable `InputHelper`.
- **Pointer invalidation:** I first saved pointers to the start and goal cells while the grid's vector was still growing. Adding rows can move the cells in memory, which leaves those pointers pointing at the wrong place. Now `Maze` finds them only after the grid is complete, and copying a `Maze` is disabled.
- **Include errors after reorganizing:** moving headers into `include/` broke every `#include` until I added that folder to the project's include directories.

### 🏆 Accomplishments
I leveled up on designing with an abstract base class. `Pathfinder` means A*, Compare mode and the Play mode enemy can all plug into the same interface. I also started writing a real test log, which made me check the algorithm's results by hand instead of assuming they were right.

### 🔮 Next Steps
- Implement A* and the Compare mode (#8)
- Add random maze generation with difficulty levels (#9)
- Keep commits small and tied to Issues so the board and history stay accurate

## Week 2

### 🔎 Overview
This week I finished everything in my Milestone 2 plan and merged it into `main`:
- **A\* pathfinder:** uses a priority queue with a Manhattan-distance heuristic.
- **Compare mode:** runs BFS and A* on the same maze side by side, with a stats table.
- **Random maze generation:** a new `MazeGenerator` class using the recursive backtracker, with Easy, Medium and Hard sizes, and seeded so a maze can be recreated.
- **Explored-cells overlay:** shows every cell each algorithm searched, so you can *see* BFS flood the maze while A* heads straight for the goal.
- **"What next?" menu after results:** lets you try another algorithm on the same maze without going back through every menu.

I also improved usability: prompts show their valid range, error messages are clearer, and there's a yes/no helper. The full write-up is in [docs/MMoreno_Milestone2.md](docs/MMoreno_Milestone2.md).

### 🧟 Challenges
- **`std::priority_queue` is a max-heap by default,** so A* kept pulling out the *worst* cell first. I fixed it with a custom comparator. A* could also queue the same cell twice, so I added a "finished" flag that skips outdated entries.
- **The maze generator's grid math:** walls sit *between* rooms, so sizes have to be odd and every step moves two cells. Checking hundreds of mazes by eye wasn't realistic, so I wrote a `--selftest` mode. It uses the fact that a perfect maze with R rooms always has exactly 2R − 1 open cells to confirm 300 generated mazes are fully connected and solvable.
- **GitHub board:** linking a PR with "Closes #N" moved Issues I'd already closed back to In Progress. I fixed the statuses and now reference Issues without that keyword.

### 🏆 Accomplishments
I leveled up on **testing my own code**. The self-test now runs 27 automated checks: 300 random mazes, the explored-cell lists, a maze with no path, and every layout error case. To make sure the tests could actually catch a bug, I broke the generator on purpose. The self-test reported 9 failures, then went back to 27/27 once I restored it. I also got more comfortable with abstract base classes: because BFS and A* share a `Pathfinder` interface, Compare mode and the overlay didn't need any algorithm-specific code.

### 🔮 Next Steps
- **Play mode (#10):** the player moves through the maze with W/A/S/D while an AI enemy chases them using the same pathfinding code.
- **Save/Load (#11) in Week 4:** the maze seeds I'm already showing will make saving a maze simple.
