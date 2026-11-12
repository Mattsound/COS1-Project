# Milestone 3: Refinement

**COS1 - Project and Portfolio I: Computer Science**
**Matt Moreno**
**Maze Pathfinder** (C++ console application)

This document records how Maze Pathfinder changed during Week 3. Test evidence is in [testing/milestone3-test-log.md](testing/milestone3-test-log.md), and day-to-day notes are in [changelog/milestone3-progress.md](changelog/milestone3-progress.md).

---

## Where the project started this week (end of Milestone 2)

- Solve a Maze and Compare mode with BFS and A*, random mazes, the explored-cells overlay, and "What next?" options
- **Play mode was still a "coming soon" message**
- `Menu.cpp` was 477 lines and did input, program flow *and* result formatting
- The self-test had 27 checks

## Where it is now

- **Play mode is complete:** race an AI enemy to the goal on Easy, Medium or Hard, and every game is winnable
- `Menu` only handles menus and flow; a new `ResultView` class does the printing
- **Duplicated code removed** in three places
- The self-test has **76 checks**, including maze and input edge cases
- One bug found by the new tests is fixed

---

## ⚙️ Features Added

| Feature | Issue | What it does |
|---|---|---|
| **Player movement** | #18 | The user moves `P` one cell per turn with W/A/S/D (either case). Walls and the edge of the grid are refused with a message, and a blocked move doesn't use up a turn. |
| **Enemy that chases you** | #19 | `E` re-plans the shortest path to the player with A* **every turn** and moves along it. |
| **Play mode game loop** | #20 | Pick Easy, Medium or Hard and a random maze is generated. Each turn shows the maze with `P` and `E` plus a status line (E's wake-up countdown, then how many moves away it is). **Win** by reaching G; you see your turn count against the shortest route, and "Perfect run" if they match. **Lose** if E reaches you; you see an `X` where you were caught and a tip. Afterwards: play again, change difficulty, or main menu. |
| **Fair difficulty** | #20 | The enemy sleeps in its lair for a calculated head start (see "What changed and why"). Easy: half speed, +6 spare turns. Medium: full speed, +3. Hard: full speed, +0, so a perfect run escapes by one step. |
| **`--seed N` option** | #20 | Running `MazePathfinder.exe --seed 12345` makes every random maze use that seed, so a maze can be shared or replayed exactly. A bad value is reported and ignored. |
| **Clearer number errors** | #22 | Typing a number too big to store (e.g. `99999999999`) now says "is too large" instead of wrongly saying "isn't a whole number". |

## 🧱 Updates to System Design

**New classes**

| Class | Responsibility |
|---|---|
| `Player` | Holds the player's current `Cell*` and move count. `move()` refuses walls and the grid edge, `parseDirection()` turns W/A/S/D into a direction, and `directionBetween()` turns two neighboring cells into a direction. |
| `Enemy` | Holds its current `Cell*` and a `Pathfinder&`. `takeTurn()` re-plans to the target and moves up to `stepsPerTurn` cells. It works with **any** pathfinder, because it only knows the base class. |
| `Game` | The Play mode **rules** (turns, win/lose, difficulty settings, where the enemy starts and how long it sleeps). It has **no console input or output**, which is what lets the self-test play thousands of complete games. |
| `ResultView` | Prints Solve and Compare results (see Refactoring). |

**Changes to existing classes**

| Class | Change | Why |
|---|---|---|
| `Pathfinder` | Algorithms now override `findPathBetween(maze, start, goal)`. `findPath(maze)` became a non-virtual base-class function that calls it with S and G. | The enemy has to find a path to the *player*, not to G. Making the search work between any two cells kept every existing caller unchanged. |
| `Maze` | Added `renderWithMarkers()`, which draws characters like `P`, `E` or `X` on top of the maze. | Play mode needs to draw people, not just paths. |
| `InputHelper` | Added `readKey()` (exactly one key from a set). `readInt()` now tells "too large" apart from "not a number". | Play mode reads single keys; the "too large" fix came from the edge-case tests. |
| `Menu` | Play mode screen; refactored (below). | |

**How the classes fit together in Play mode**

```
Menu (reads keys, draws the screen)
  └── Game (rules: turns, win/lose, head start)
        ├── Player (moves with W/A/S/D)
        └── Enemy  (chases using a Pathfinder&)
                      └── AStarPathfinder (via the Pathfinder base class)
```

`Player`, `Enemy` and `Game` never read input or print anything. Only `Menu` talks to the console, so every rule can be tested automatically.

## 🧹 Refactoring Improvements

Every refactoring step below was checked the same way. Before changing anything, I saved the exact output of **12 scripted menu sessions** (using fixed maze seeds) plus the self-test. After each step I re-ran them all and compared the output **byte for byte**. Every step matched exactly, so the code's structure changed while its behavior stayed the same.

| What changed | Before | After | Why |
|---|---|---|---|
| **Moved result printing out of `Menu` into a new `ResultView` class** | `Menu` had `showResult`, `printSideBySide`, `printStatsTable`, `printComparisonSummary` and `printLegend`, about 110 lines of formatting mixed in with menu flow | `Menu::solveCurrentMaze()` just calls `m_view.showSolve(...)`, and `compareCurrentMaze()` calls `m_view.showComparison(...)` | Each class now has one job: `Menu` decides *what* to show and *when*, `ResultView` decides *how it looks*. Changing the look of the stats table no longer means touching menu logic. |
| **Removed 8 copies of the "input ended" cleanup** | Every prompt in `Menu` repeated: if `InputHelper::readInt(...)` fails, set `m_running = false` and return | New `askInt()`, `askKey()` and `askYesNo()` wrap the `InputHelper` functions and stop the menu themselves, so callers write `if (!askInt(...)) return false;` | The same 4-line block was copied 8 times. Now there's one place to change if end-of-input handling ever changes, and each menu function is shorter. |
| **Merged two copies of the Easy/Medium/Hard menu** | `chooseRandomMaze()` and Play mode's `choosePlayDifficulty()` built nearly the same list | One `chooseDifficulty(difficulty, describeEnemy)`, used by both. The flag adds the enemy description only for Play mode | Duplicated code. If a difficulty is added, it now only has to be added once. |
| **Removed a duplicated helper** | `directionBetween()` (which way to step from one cell to the next) existed in both `Game.cpp` and `SelfTest.cpp` | It's now the static `Player::directionBetween()`, next to `parseDirection()` | It describes player movement, so it belongs with `Player`, and it was copied in two places. |
| **Made pathfinding work between any two cells** | Each algorithm's `findPath(maze)` searched only from S to G | Algorithms implement `findPathBetween(maze, start, goal)`; `findPath(maze)` is a base-class wrapper | The enemy needed it, and no caller had to change. The self-test confirms G→S matches S→G on 100 random mazes. |
| **Replaced a slow simulation with a direct calculation** | Choosing the enemy's start simulated a whole game (with A* every turn) for every candidate cell; the self-test never finished | One breadth-first "distance flood" from the enemy's lair, then a simple check of whether it can reach each route cell in time | Same answer, a tiny fraction of the work. The self-test went from never finishing to about 24 seconds. |

**Result:** `Menu.cpp` went from **477 lines to 364**, with three duplications removed and no change in behavior.

**A C++ detail I had to get right:** `ResultView` keeps *references* to `Menu`'s maze and algorithm list. C++ builds class members in the order they're **declared**, so `m_view` has to be declared after `m_maze` and `m_pathfinders`. Otherwise it would hold references to objects that don't exist yet.

## 🧪 Testing and Debugging

- **Self-test: 62 → 76 checks**, all passing. New this week:
  - Player, Enemy and Game rules
  - **Play mode fairness on 300 random mazes:** walking the shortest route always wins, *one turn less* head start gets caught (so the head start is the exact minimum), and stalling always gets caught
  - maze edge cases: the smallest 5×5 maze, start next to goal, a goal walled in, an open room, and mazes with loops
  - input edge cases: the self-test points `std::cin` and `std::cout` at strings so it can "type" at a prompt
- **End-to-end Play mode tests:** with `--seed 4242`, a script reads the maze off the screen, solves it with its own BFS, replays the route (a perfect run on all three difficulties), then stalls to check the loss.
- **Testing the tests:** I broke the `Enemy` on purpose (ignoring its speed), and the self-test caught it.

**Bugs found and fixed this week**

| Bug | Cause | Fix |
|---|---|---|
| Most Play mode games were impossible to win | Random mazes have one route; an always-chasing enemy as fast as you could usually cut it off (a fair start existed in only 16-56% of mazes) | The sleeping-enemy head start (below) |
| The fairness self-test never finished | It simulated a full game per candidate cell | The direct distance calculation (see Refactoring) |
| Both enemy-chase checks failed at first | A bug in the **test**: its loop limit used a distance that shrinks every turn | Fix the limit once before the loop |
| 3 of 100 small mazes had nowhere to put the enemy | They were a single corridor | `Game::generatePlayable()` skips them and uses the next seed |
| `99999999999` was called "not a whole number" | `std::stoi` throws `out_of_range` for it, and every exception was treated as bad text | Check the text is digits first, then report "too large" separately |
| "You left the maze after 1 moves" | Hard-coded plural | `countText()` helper |

## 💭 What I Changed and Why

- **Why Play mode needed a head start.** My first version placed the enemy as far away as possible and had it chase from turn 1. Testing showed that most games couldn't be won, because the maze generator makes "perfect" mazes with exactly **one** route to G. An enemy as fast as you can only be outrun if it starts on a side branch longer than your distance to where that branch joins your route, and most mazes don't have one. So the enemy now **sleeps in its lair** (the farthest open cell that isn't on your route) for a head start. The game calculates the *smallest* number of turns that still lets a perfect run escape, then adds spare turns by difficulty. That makes every game winnable, and makes "Hard" mean something precise: no room for mistakes.
- **Why "Hard" is no longer a faster enemy.** I originally planned Hard as an enemy that moves 2 cells per turn. The same analysis showed it was almost never beatable in one-route mazes. Hard is now "full speed, zero spare turns". The 2-cell speed is still supported and tested in `Enemy`, just not used by any difficulty.
- **Why the rules live in `Game` with no input or output.** It let the self-test play complete games thousands of times, which is how the head-start problem was found and how the fix is proven.
- **Why I refactored after adding features, not before.** Play mode added a lot to `Menu`, which made its problems obvious: mixed jobs and copied code. Refactoring with a saved baseline let me clean it up without breaking anything.
- **Why so many edge-case tests.** Week 3's goal was "it works reliably". The new tests found one real bug (the "too large" message) and showed the maze code was already solid on unusual mazes.

## 📋 Project Management

- **New Issues this week** (created Tuesday 11/10, on the board): **#18** Player, **#19** Enemy, **#20** Play mode game loop, **#21** Refactor `Menu`, **#22** Edge-case tests. **#10** (Play mode) was the parent feature.
- Each Issue moved To Do → In Progress → Done as it was worked on.
- **Pull Request #23** merged this week's development into `main` and is on the board, along with a final PR for this document.
- **Next (Week 4):** #11 Save and Load.
