# Milestone 2 Test Log (in progress)

**Build:** Visual Studio 2026 (MSBuild 18, toolset v145), C++20, Debug | x64
**Result:** Build succeeded, 0 warnings, 0 errors (12 source files)

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

## Random maze generation (#9)

### Automated self-test

Run `MazePathfinder.exe --selftest`. It prints PASS/FAIL for each check and exits with code 0 only if all of them pass.

```
Random maze generation:
  PASS  Easy (11 x 7), 100 seeds: every maze is a valid layout
  PASS  Easy (11 x 7), 100 seeds: every room is connected
  PASS  Easy (11 x 7), 100 seeds: BFS and A* both find a path
  PASS  Easy (11 x 7), 100 seeds: BFS and A* paths are the same length
  PASS  Easy: the same seed always builds the same maze
  (same 5 checks for Medium 21 x 11 and Hard 41 x 21)
  PASS  Requested 4 x 6 is rounded up to 5 x 7 (sizes must be odd, at least 5)

Maze layout validation:
  PASS  Empty layout is rejected -> "The maze layout is empty."
  PASS  Rows of different lengths are rejected -> "Row 3 is a different length than the first row."
  PASS  Unknown character is rejected -> "Unknown character 'X' at row 2, column 3."
  PASS  Missing start is rejected -> "A maze needs exactly one start (S) and one goal (G)."
  PASS  Two goals are rejected -> "A maze needs exactly one start (S) and one goal (G)."

21 passed, 0 failed
```

**How "every room is connected" is checked:** rooms sit at odd rows and columns. A perfect maze joins R rooms with exactly R - 1 knocked-down walls, so it must have exactly 2R - 1 open cells. A different count means part of the maze was never reached.

**Testing the test:** I temporarily changed the generator so it stopped knocking down the walls between rooms. The self-test reported **9 failed** (rooms not connected, no path found) and exited with code 1. After the code was restored it went back to 21 passed.

This also covers the `Maze::loadFromLayout()` error cases that were listed as "not yet tested" in Milestone 1.

### Menu tests

| # | Test | Expected | Actual | Pass |
|---|---|---|---|---|
| 15 | Compare → Random → Hard | Generates a 41 x 21 maze, shows the seed, solves it side by side | "Generated random maze #60376 (Hard, 41 x 21)", BFS 168 moves / 397 cells, A* 168 moves / 353 cells | ✅ |
| 16 | Solve → Random → Easy → A* | Generates an 11 x 7 maze and solves it | "Generated random maze #72680 (Easy, 11 x 7)", 16-move path | ✅ |
| 17 | `0` at the difficulty picker | Back to the main menu with no pause | Main menu shown | ✅ |
| 18 | `x` then `9` at the difficulty picker | Both rejected with a clear message | `"x" isn't a whole number...`, `9 isn't an option...` | ✅ |
| 19 | Built-in Small maze after the random maze option was added | Still works (maze picker now 0-4) | 10-move path | ✅ |

## Explored-cells overlay (#14)

### Self-test additions (now 27 checks, all passing)

```
  PASS  Easy (11 x 7), 100 seeds: explored lists start at S, end at G, have no repeats, and cover the path
  PASS  Medium (21 x 11), 100 seeds: (same)
  PASS  Hard (41 x 21), 100 seeds: (same)

Maze with no path:
  PASS  Sealed Vault: both report no path
  PASS  Sealed Vault: both explore all 10 reachable cells before giving up
  PASS  Sealed Vault: explored lists start at S and have no repeats

27 passed, 0 failed
```

### Menu tests

| # | Input | Expected | Actual | Pass |
|---|---|---|---|---|
| 20 | Solve → Medium → BFS → `maybe` | Explains it isn't a yes/no answer | `"maybe" isn't a yes or no answer. Please type y or n.` | ✅ |
| 21 | Blank line at the y/n prompt | Explains what to type | `Type y for yes or n for no, then press Enter.` | ✅ |
| 22 | `YES` (capitals) | Accepted as yes | BFS overlay drawn with `o` for explored cells and `.` for the path | ✅ |
| 23 | Compare → Medium → `y` | Both overlays side by side | BFS fills most of the left half of the maze; A* explores only a few cells off the path | ✅ |
| 24 | Compare → Random Easy → `n` | No overlay; goes straight to the pause | "Press Enter to return to the main menu..." | ✅ |
| 25 | Solve → Sealed Vault → A* → `y` | Overlay shows the 10 cells it searched before giving up | All 10 cells on S's side marked `o`; the G side is untouched | ✅ |
| 26 | Input ends at the y/n prompt | Exits cleanly | "Thanks for playing. Goodbye!" | ✅ |

### Sample (Compare mode, Medium maze)

```
  BFS                        A*

  #####################      #####################
  #Soooo#o      #     #      #Soooo#       #     #
  #.###o#o##### # ### #      #.###o# ##### # ### #
  #.#ooo#ooooo# #   # #      #.# oo#     # #   # #
  #.#o#######o# ### # #      #.# ####### # ### # #
  #.#ooooooo#o#     # #      #.#       # #     # #
  #.#######o#o####### #      #.####### # ####### #
  #.......#.........# #      #.......#.........# #
  #######.#.#######.# #      #######.#.#######.# #
  #oooooo...#    oo..G#      #     o...#      ..G#
  #####################      #####################

Legend: S = start, G = goal, # = wall, . = path, o = explored but not on the path
```

## Quick options after results (#15)

After results, the program shows:

```
What next?
1. Solve this maze with an algorithm
2. Compare algorithms on this maze
3. Pick a new maze
0. Back to main menu
```

| # | Steps | Expected | Actual | Pass |
|---|---|---|---|---|
| 27 | Solve → Medium → BFS, then `1` → A* | Solves the *same* maze with A* without picking it again | "Solving with A* Search..." on the Medium maze | ✅ |
| 28 | Then `2` | Compares both algorithms on the same maze | Side-by-side BFS / A* on the Medium maze | ✅ |
| 29 | Then `3` → Small | New maze, staying in Compare mode | Side-by-side BFS / A* on the Small maze | ✅ |
| 30 | `0` at What next? | Back to the main menu | Main menu shown | ✅ |
| 31 | `5` at What next? | Rejected | `5 isn't an option. Please enter a number from 0-3.` | ✅ |
| 32 | `3`, then `0` at the maze picker | Back to the main menu | Main menu shown | ✅ |
| 33 | Solve → Small, then `0` at the algorithm picker | Back to the main menu | Main menu shown | ✅ |
| 34 | Play mode | Still shows the message and pauses | "Press Enter to return to the main menu..." | ✅ |
| 35 | Input ends at What next? | Exits cleanly | "Thanks for playing. Goodbye!" | ✅ |

The self-test still passes 27 of 27.
