# Milestone 2 Test Log (in progress)

**Build:** Visual Studio 2026 (MSBuild 18, toolset v145), C++20, Debug | x64
**Result:** Build succeeded, 0 warnings, 0 errors (10 source files)

Tests were run with scripted input redirected from a text file, the same way as in [Milestone 1](milestone1-test-log.md). All Milestone 1 cases were re-run and still pass.

## A* and Compare mode (#8)

| # | Test | Expected | Actual | Pass |
|---|---|---|---|---|
| 1 | Compare: Small maze | Both find a shortest path, and A* explores fewer cells | BFS 10 moves / 21 cells, A* 10 moves / 17 cells (19% fewer) | ✅ |
| 2 | Compare: Medium maze | Both find a shortest path, and A* explores fewer cells | BFS 30 moves / 66 cells, A* 30 moves / 39 cells (40% fewer) | ✅ |
| 3 | Compare: Sealed Vault (no path) | Both report no path without crashing | Both show "no path" with 10 cells explored, plus an explanation | ✅ |
| 4 | Solve a Maze with A* (Medium) | Draws a 30-move path | 30 moves, 39 cells explored | ✅ |
| 5 | Algorithm picker: `x`, `5` | Rejects both and asks again | Error message, then asks again | ✅ |
| 6 | Mazes drawn side by side | Columns line up with the BFS and A* labels above them | Aligned at every maze size | ✅ |

**Why A* can't return a longer path:** its Manhattan-distance estimate never overestimates the real distance (moves are only up, down, left and right), so the first time it reaches the goal it has a shortest path. Tests 1-2 confirm this: both algorithms always report the same path length.

## Usability (#13)

| # | Input | Expected | Actual | Pass |
|---|---|---|---|---|
| 7 | Blank line at the main menu | Explains what to type | "Type a number from 1-4, then press Enter." | ✅ |
| 8 | `abc` | Echoes the bad input | `"abc" isn't a whole number. Please enter a number from 1-4.` | ✅ |
| 9 | `9` | Says it isn't an option | `9 isn't an option. Please enter a number from 1-4.` | ✅ |
| 10 | Any prompt | Shows the valid range | "Choose an option (1-4):", "Choose a maze (0-3):", "Choose an algorithm (0-2):" | ✅ |
| 11 | After a solve or compare | Results stay until Enter is pressed | "Press Enter to return to the main menu..." | ✅ |
| 12 | `0` (Back) at the maze picker | Returns straight to the menu with no pause | Main menu shown immediately | ✅ |
| 13 | Play mode (`2`) | Explains it isn't available and suggests other options | Message plus suggestions, then the pause | ✅ |
| 14 | Input ends at the "Press Enter" pause | Exits cleanly | "Thanks for playing. Goodbye!" | ✅ |
