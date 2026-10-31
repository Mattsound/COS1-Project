# Milestone 1 Test Log

**Build:** Visual Studio 2026 (MSBuild 18, toolset v145), C++20, Debug | x64
**Result:** Build succeeded, 0 warnings, 0 errors (all 8 source files compiled)

Screenshots: `images/milestone1-core-structure*.png`

## How the tests were run

The program was run with scripted input redirected from a text file, so the exact same test can be repeated after every change. Each line of the file is one thing typed at a prompt.

## Test cases

| # | Feature | Input | Expected | Actual | Pass |
|---|---|---|---|---|---|
| 1 | Main menu | `1` | Opens the maze picker | Maze picker shown | ✅ |
| 2 | Validation: not a number | `abc` | "Please enter a whole number." and ask again | Same | ✅ |
| 3 | Validation: out of range | `9` | "Please enter a number from 0 to 3." and ask again | Same | ✅ |
| 4 | Solve Small maze (BFS) | `1` | Maze drawn, then solved with a shortest path | 10-move path, 21 cells explored | ✅ |
| 5 | Solve Medium maze (BFS) | `2` | Maze drawn, then solved with a shortest path | 30-move path, 66 cells explored | ✅ |
| 6 | Unsolvable maze | `3` | Reports that no path exists, without crashing | "No path exists from S to G", 10 cells explored | ✅ |
| 7 | Back option | `0` | Returns to the main menu | Main menu shown | ✅ |
| 8 | Exit | `4` | Prints goodbye and closes | "Thanks for playing. Goodbye!" | ✅ |
| 9 | Input ends mid-menu (Ctrl+Z / end of file) | `1` then EOF | Closes cleanly, with no infinite loop | Goodbye printed, program exited | ✅ |
| 10 | Validation from earlier work: blank line, `3x`, `0`, `99`, `"  2  "` at the main menu | | Rejects bad input and accepts the padded number | Same (tested with the InputHelper commit) | ✅ |

### Checking the results by hand

- **Small maze:** counting the path by hand gives S → (1,2) → (1,3) → (2,3) → (3,3) → (3,4) → (3,5) → (4,5) → (5,5) → (5,6) → (5,7) → G, which is 10 moves. No shorter route exists because the wall at column 4 forces the detour.
- **Sealed Vault:** the start's area has exactly 10 open cells (4 + 2 + 4). BFS explored all 10, then correctly reported that no path exists.

## Sample output (Small maze)

```
Here's the maze:

  #########
  #S  #   #
  # # # # #
  # #   # #
  # ### # #
  #   #  G#
  #########

Solving with Breadth-First Search (BFS)...

  #########
  #S..#   #
  # #.# # #
  # #...# #
  # ###.# #
  #   #..G#
  #########

Legend: S = start, G = goal, # = wall, . = path
Path length:    10 moves
Cells explored: 21
```

## Sample output (no path)

```
Solving with Breadth-First Search (BFS)...

No path exists from S to G. Walls block every route.
Cells explored: 10
```

## Not yet tested

- `Maze::loadFromLayout` error messages (uneven rows, unknown characters, missing S or G). The built-in mazes are all valid, so these checks can't be reached from the menu yet. They'll be tested in Milestone 2, when random generation is added, and again in Week 4, when mazes can be loaded from files.
