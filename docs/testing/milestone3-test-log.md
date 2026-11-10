# Milestone 3 Test Log (in progress)

**Build:** Visual Studio 2026 (MSBuild 18, toolset v145), C++20, Debug | x64. Build succeeded, 0 warnings, 0 errors.

All Milestone 1 and 2 tests were re-run and still pass.

## Player movement (#18)

### Self-test additions (now 33 checks, all passing)

```
Player movement:
  PASS  W/A/S/D map to up/left/down/right in either case
  PASS  Other keys are not treated as moves
  PASS  Moving into a wall is refused, the player stays put, and no move is counted
  PASS  Moving into an open cell works and counts as one move
  PASS  Moving off the edge of the grid is refused
  PASS  Walking BFS's path on 100 random mazes reaches G in exactly the path length

33 passed, 0 failed
```

The edge test uses a one-row maze with no border walls (`"S G"`), so "up" really would leave the grid. That checks `Maze::getCell()` returning `nullptr` rather than relying on border walls.

### Play mode menu tests

| # | Input | Expected | Actual | Pass |
|---|---|---|---|---|
| 1 | Play → Small | Maze drawn with `P` on the start, plus controls | `P` at row 1, column 1; "Moves: 0" | ✅ |
| 2 | `w` at the start (wall above) | Refused, with no move counted and no redraw | "That way is blocked by a wall. Try another direction." | ✅ |
| 3 | `x` | Rejected with help | `"x" isn't a valid key. Use W (up), A (left), S (down), D (right), or Q to quit.` | ✅ |
| 4 | Blank line | Help shown | `Use W (up), A (left), S (down), D (right), or Q to quit.` | ✅ |
| 5 | `ww` | Rejected (only one key per move) | `"ww" isn't a valid key...` | ✅ |
| 6 | `D` (capital) | Accepted | `P` moves right; "Moves: 1" | ✅ |
| 7 | The shortest 10-move route to G | Win message compared to BFS | "You reached the goal in 10 moves! The shortest possible route is 10 moves." and "Perfect run" | ✅ |
| 8 | Play → Sealed Vault | Warns that there's no route | "Heads up: walls block every route to G in this maze..." | ✅ |
| 9 | `d` then `q` | Quits with the move count, in correct grammar | "You left the maze after 1 move." | ✅ |
| 10 | Play → `0` at the maze picker | Back to the main menu | Main menu shown | ✅ |
| 11 | Input ends mid-game | Exits cleanly | "Thanks for playing. Goodbye!" | ✅ |

**Bug found and fixed while testing:** the quit message said "after 1 moves". A `movesText()` helper now picks "move" or "moves".
