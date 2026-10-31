# Milestone 1 Progress Notes

## Completed

- **Repository setup:** `main` and `dev` branches, the required folder structure, a Visual Studio `.gitignore`, and the instructor added as a collaborator and to the Project board.
- **Project structure:** a Visual Studio C++20 console project with headers in `include/` and source files in `src/`. The project adds `include/` to the include path, and a `.filters` file shows the same groups in Solution Explorer.
- **Menu loop (`Menu`):** a persistent numbered menu with an Exit option.
- **Input validation (`InputHelper`):** reads whole lines and rejects non-numbers, partial numbers like `3x`, blank lines and out-of-range values. It exits cleanly when input ends.
- **`Cell` class:** position, type (open, wall, start, goal), its display symbol, and a parent pointer used to rebuild paths.
- **`Maze` class:** builds a `vector<vector<Cell>>` grid from a text layout, checks the layout is valid, finds walkable neighbors and draws itself as ASCII with an optional path overlay.
- **`MazePresets`:** three built-in mazes (Small, Medium, and an unsolvable "Sealed Vault" for testing).
- **`Pathfinder` base class and `BfsPathfinder`:** an abstract interface plus a working Breadth-First Search that returns the path, its length and the number of cells explored.
- **Solve a maze:** fully working from the menu. Pick a maze, see it drawn, then see it solved with stats.

## Bugs & Challenges

| Problem | Cause | Fix |
|---|---|---|
| Typing a letter at the menu reprinted the menu forever | `std::cin >> int` failed, and `cin` stayed in a failed state, so every later read failed instantly | Wrote `InputHelper::readInt`, which reads a full line with `getline` and parses it with `std::stoi` in a try/catch. Bad input can't break `cin` anymore. |
| End of input (Ctrl+Z) would also loop forever | `getline` keeps failing at end of file | `readInt` returns `false` at end of input, and the menu treats that as Exit. |
| Start and goal pointers could point at the wrong memory | Taking `&cell` while rows were still being added to the vector. Adding rows can move the cells in memory and invalidate the pointers. | `Maze` finds the start and goal only after the grid is fully built, and copying a `Maze` is disabled so pointers never refer to another maze's cells. |
| Moving files into `src/` and `include/` broke `#include "Cell.h"` | The compiler only searched the source file's own folder | Added `$(ProjectDir)include` to `AdditionalIncludeDirectories` for both Debug and Release. |

## Design Decisions

- **Abstract `Pathfinder` base class:** A* (Milestone 2) and the Play mode enemy can both plug into the same interface, and Compare mode can run any algorithm without special-case code.
- **Parent pointers on `Cell`:** BFS records how it reached each cell, and the path is rebuilt by following the pointers back from the goal. This avoids storing a whole path for every cell.
- **Layouts stored as `vector<string>`:** easy to type, easy to read, and the same format will work for random generation and for Save/Load in Week 4.
- **An unsolvable maze among the presets:** it proves the "no path" case is handled instead of crashing or hanging.

## Next (Milestone 2)

- #8 `AStarPathfinder` and Compare mode (BFS vs. A* side by side)
- #9 Random maze generation and difficulty levels
- Test `Maze::loadFromLayout`'s error handling once mazes come from more than the built-in presets
