# Milestone 3 Progress Notes (in progress)

Week 3 focus: finishing **Play mode** (#10) and making the program **reliable** (refactoring and edge-case testing).

## Week 3 Issues

| Issue | Work | Status |
|---|---|---|
| #18 | `Player` class with W/A/S/D movement and wall collision | ✅ Done |
| #19 | `Enemy` class that chases the player using A* | ✅ Done |
| #20 | Play mode game loop with win/lose and difficulty | ✅ Done |
| #10 | Play mode (parent of #18-#20) | ✅ Done |
| #21 | Refactor: move result and Compare display out of `Menu` | 📥 Backlog |
| #22 | Edge-case tests: tiny mazes, start next to goal, mazes with loops | 📥 Backlog |

## Completed so far

- **`Player` class (#18):** tracks its current cell and move count. `move()` refuses walls *and* the grid edge without using up a move. `parseDirection()` maps W/A/S/D in either case.
- **`InputHelper::readKey()`:** accepts exactly one key from a set and explains anything else (blank, wrong key, or several keys at once).
- **`Maze::renderWithMarkers()`:** draws characters like `P` and `E` on top of the maze.
- **Pathfinder refactor (#19):** algorithms now override `findPathBetween(maze, start, goal)`, and `findPath(maze)` became a base-class wrapper that searches S to G. The enemy needs to path to the *player*, not to G. No existing caller had to change.
- **`Enemy` class (#19):** each turn it re-plans with its `Pathfinder` and moves up to `stepsPerTurn` cells toward its target, stopping on it. If no route exists it stays put. It takes a `Pathfinder&`, so it works with A* or BFS without knowing which.
- **`Game` class (#20):** the Play mode rules with **no console input or output**, so the self-test can play thousands of complete games. The player moves first, reaching G wins before the enemy moves, stepping onto the enemy (or being reached) loses, and blocked moves use no turn.
- **Play mode (#20):**
  - Choose Easy, Medium or Hard. A random maze is generated, and P and E are drawn each turn with a status line: E's wake-up countdown, then how many moves away it is.
  - The win screen shows your turns against the shortest route ("Perfect run" if they match). The loss screen marks where you were caught with an X and gives a tip.
  - After each game: play again, change difficulty, or go back to the main menu.
- **`--seed N` option:** makes random mazes repeatable, for sharing a maze or replaying it in a test.

## The big design problem: an unbeatable enemy

The first version placed the enemy as far from S as possible, and it was always chasing. Testing 300 random mazes showed that **a fair start existed in only 16-56% of mazes**. The reason is the maze generator. It makes "perfect" mazes, which have exactly **one** route from S to G. An enemy as fast as you can only be outrun if it starts on a side branch that's longer than your distance to where that branch joins your route, and most mazes don't have one. Hard (an enemy twice your speed) was almost never winnable.

**Fix: a sleeping enemy with a calculated head start.**
- The enemy starts in its **lair**, the open cell farthest from S that isn't on your route.
- It sleeps for a **head start**: the smallest number of turns that still lets a player on the shortest route escape. After turn *t* you stand on route cell *t*, and by then the enemy has made `(t − headStart) / movesEvery` moves. Its distance to that cell must be bigger. One breadth-first flood from the lair gives every distance at once.
- Each difficulty adds slack: **Easy** +6 turns and half speed, **Medium** +3 turns, **Hard** +0 turns, so a perfect run escapes by a single step.

The self-test proves both directions on 300 mazes: the shortest route **always wins**, and **one turn less** head start gets it **caught**. So the head start is exactly the minimum, and Hard truly has no room for mistakes. A player who stalls is always caught, so the enemy is never harmless.

## Bugs & Challenges

| Problem | Cause | Fix |
|---|---|---|
| A wall bump redrew the whole maze, pushing the message off-screen on big mazes | The loop always redrew after input | Only redraw after a successful move |
| "You left the maze after 1 moves" | The plural was hard-coded | `countText()` helper ("1 move", "2 turns") |
| How to test movement without a human pressing keys | Play mode reads from the keyboard | `Player` and `Game` don't read input, so the self-test drives them directly |
| Both enemy chase checks failed at first | A bug in the *test*: the loop limit used a distance that shrinks every turn | Fix the turn limit once before the loop, then mutation-test it |
| The first fairness self-test never finished | Every candidate start ran a full simulated game with A* each turn | Replaced with one distance flood per candidate and a direct check |
| Most games were unwinnable | One-route mazes (see above) | The sleeping enemy with a calculated head start |
| 3 of 100 small mazes had no room for an enemy | They were a single corridor | `Game::generatePlayable()` skips them and uses the next seed |
| Play mode can't be script-tested if every maze is random | Random seeds | `--seed N`, plus an end-to-end script that reads the maze off the screen, solves it, and replays the route |

## Design notes

- `Player` only knows how to move, `Enemy` only knows how to chase, and `Game` only knows the rules. None of them read input or draw anything, so the self-test can play complete games without a keyboard. `Menu` is the only place that talks to the console.
- `Enemy` takes any `Pathfinder&`. The game gives it A*, but the tests prove the rules hold no matter which algorithm drives it.
EOF
