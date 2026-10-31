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
| Persistent menu with validated input (no crashes or infinite loops on bad input) | ✅ Done |
| **Solve a maze:** pick a built-in maze and see it solved with Breadth-First Search, with the path, its length and the number of cells explored | ✅ Done |
| Detects and reports mazes with no possible path | ✅ Done |
| **Compare algorithms:** BFS vs. A* on the same maze, side by side | 📝 Milestone 2 |
| Random maze generation with difficulty levels | 📝 Milestone 2 |
| **Play mode:** move with W/A/S/D and reach the goal before the AI enemy catches you | 📝 Milestone 3 |
| Save and load mazes and scores to files | 📝 Milestone 4 |

## Class Plan

| Class | Responsibility | Status |
|---|---|---|
| `Menu` | Runs the main menu loop and sends each choice to the right feature | ✅ |
| `Cell` | One square of the maze: position, type (open, wall, start, goal) and a parent pointer used to rebuild paths | ✅ |
| `Maze` | Owns the `vector<vector<Cell>>` grid. Builds it from a text layout, finds walkable neighbors and draws itself as ASCII | ✅ |
| `Pathfinder` | Abstract base class, so every search algorithm shares one interface | ✅ |
| `BfsPathfinder` | Breadth-First Search, which always finds the shortest path | ✅ |
| `AStarPathfinder` | A* search with a Manhattan-distance heuristic | 📝 |
| `Player` / `Enemy` | Play mode: the user moves the player, and the enemy chases using a `Pathfinder` | 📝 |

Helpers: `InputHelper` (validated console input) and `MazePresets` (built-in maze layouts).

## Repository Structure

```
dev/       Visual Studio solution and C++ project (include/ = headers, src/ = source)
docs/      planning/, testing/, changelog/, and the Version Control reference
images/    screenshots
```

## Build & Run

1. Open `dev/MazePathfinder.sln` in Visual Studio 2022 or newer.
2. Select the **x64** platform and press **Ctrl + F5**.

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
