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

## Enemy and searching between any two cells (#19)

### Self-test additions (now 41 checks, all passing)

```
Paths between any two cells:
  PASS  Searching G to S on 100 random mazes gives a path from G to S the same length as S to G (BFS and A*)
  PASS  Searching from a cell to itself finds a 0-move path
  PASS  A missing start or goal cell reports no path instead of crashing

Enemy chasing:
  PASS  Enemy (1 step/turn) gets exactly 1 move closer each turn and catches a still target in d turns on 100 mazes
  PASS  Enemy (2 steps/turn) gets 2 moves closer each turn and catches a still target in d/2 turns (rounded up)
  PASS  An enemy already on the target reports a catch without moving
  PASS  Sealed Vault: an enemy walled off from the target stays put and never catches it
  PASS  Steps per turn below 1 are raised to 1

41 passed, 0 failed
```

**How "gets closer each turn" is measured:** after every enemy turn, a separate BFS measures the real distance to the target. With 1 step per turn it must drop by exactly 1, and with 2 steps by exactly 2 (or reach 0), and the enemy must always stand on an open cell.

**Bug found in the test itself:** the first version used `turn <= distance + 1` as the loop limit, but `distance` shrinks every turn, so the loop quit about halfway through the chase and both chase checks failed. The fix fixes the limit once, before the loop. The `Enemy` code was correct; the test was wrong.

**Testing the test:** I temporarily changed `Enemy` to always take 1 step, ignoring its speed setting. The 2-steps-per-turn check failed (40 passed, 1 failed, exit code 1), then everything went back to 41/41 once the code was restored.

### Regression

The refactor changed the `Pathfinder` interface that every mode uses, so I re-ran the Solve, Compare, overlay and Play menu scripts. The results were unchanged: Medium maze 30-move path, A* 40% fewer cells than BFS, perfect 10-move Play run.

## Play mode game loop (#20)

### Self-test additions (now 62 checks, all passing, about 24 seconds in a Debug build)

```
Play mode rules:
  PASS  Difficulty settings: Easy half speed +6 turns, Medium full speed +3 turns, Hard full speed +0 turns
  PASS  A blocked move uses no turn and the enemy doesn't move
  PASS  Walking onto the enemy's cell is a loss
  PASS  On Easy the enemy only moves after every second turn
  PASS  On Medium the enemy moves after every turn
  PASS  With a 3-turn head start the enemy sleeps for turns 1-3 and moves after turn 4
  PASS  Reaching G wins even with the enemy one cell behind (the player moves first)
  PASS  No more moves are accepted after the game is over
  PASS  A corridor maze (every open cell on the route) is reported as having no room for an enemy

Play mode fairness (Easy, Medium and Hard, 100 random mazes each):
  PASS  a playable maze with an enemy lair is always found (100/100; Easy skipped 3 corridor mazes)
  PASS  walking the shortest route always wins (100/100)
  PASS  one turn less head start and the shortest route is caught (100/100)
  PASS  a player who stalls near S always gets caught (100/100)

62 passed, 0 failed
```

The "one turn less" check proves the head start is the **exact minimum**. On Hard (0 extra turns), a perfect run escapes, but the enemy would catch it with one turn less.

### End-to-end tests (real program, real input)

These use `--seed 4242` so the maze is the same every run. The test script:
1. runs Play mode and quits immediately, to read the maze from the screen
2. finds the shortest route with its own BFS
3. replays that route, after a few bad inputs (`x`, a blank line, `ww`)
4. plays again while stepping back and forth next to S, to check the loss

| Difficulty | Head start | Route | Win run | Stall run |
|---|---|---|---|---|
| Easy (11 x 7) | 12 turns | 16 moves | "You escaped in 16 turns!", "Perfect run" ✅ | "Caught on turn 51!" ✅ |
| Medium (21 x 11) | 44 turns | 62 moves | "You escaped in 62 turns!", "Perfect run" ✅ | "Caught on turn 119!" ✅ |
| Hard (41 x 21) | 141 turns | 200 moves | "You escaped in 200 turns!", "Perfect run" ✅ | "Caught on turn 393!" ✅ |

In every run, all 3 bad inputs got a help message and the "*** E woke up! ***" alert appeared exactly once.

### Other checks

| Input | Expected | Actual | Pass |
|---|---|---|---|
| `--seed abc` | Warn and continue with random mazes | `Ignoring --seed: "abc" isn't a whole number. Using random mazes.` | ✅ |
| Solve, Compare and overlay scripts (regression) | Unchanged results | Medium maze 30-move path, A* 40% fewer cells | ✅ |

### Bugs found and fixed while building #20

| Problem | How it showed up | Fix |
|---|---|---|
| The first self-test never finished (still running after 10 minutes) | `chooseEnemyStart` simulated a whole game (with A* every turn) for every candidate cell, plus a separate BFS for every cell | One breadth-first flood per candidate, plus a direct distance check instead of a simulation |
| Most random games were impossible | A fair enemy start existed in only 16-56% of mazes | The sleeping-enemy head start (see the progress notes) |
| Hard (enemy at 2 cells per turn) was almost never winnable | Same analysis: in a one-route maze, a faster chaser always catches up | Hard is now full speed with 0 spare turns, which is tight but always winnable |
| 3 of 100 small mazes had nowhere to put the enemy | "Lair found" was 97/100 on Easy | `generatePlayable()` skips corridor mazes and uses the next seed |
| "E is asleep for 44 turns, then enemy moves every turn." | Seen while reading the screen | `describeEnemy()` no longer starts with "enemy"; the difficulty list adds it |
