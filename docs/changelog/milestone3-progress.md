# Milestone 3 Progress Notes (in progress)

Week 3 focus: finishing **Play mode** (#10) and making the program **reliable** (refactoring and edge-case testing).

## Week 3 Issues

| Issue | Work | Status |
|---|---|---|
| #18 | `Player` class with W/A/S/D movement and wall collision | ✅ Done |
| #19 | `Enemy` class that chases the player using A* | ✅ Done |
| #20 | Play mode game loop with win/lose and difficulty | 📝 To Do |
| #21 | Refactor: move result and Compare display out of `Menu` | 📥 Backlog |
| #22 | Edge-case tests: tiny mazes, start next to goal, mazes with loops | 📥 Backlog |

## Completed so far

- **`Player` class (#18):** tracks its current cell and move count. `move()` refuses walls *and* the grid edge without using up a move. `parseDirection()` maps W/A/S/D in either case.
- **`InputHelper::readKey()`:** accepts exactly one key from a set and explains anything else (blank, wrong key, or several keys at once).
- **`Maze::renderWithMarkers()`:** draws characters like `P` (and soon `E`) on top of the maze.
- **Play mode is playable:** it replaced the "coming soon" message. Pick any maze (built-in or random) and walk from S to G. Reaching G compares your move count with BFS's shortest route ("Perfect run" if they match). Q quits, and the no-path maze warns you up front.
- **Pathfinder refactor (#19):** algorithms now override `findPathBetween(maze, start, goal)`, and `findPath(maze)` became a base-class wrapper that searches S to G. The enemy needs to path to the *player*, not to G. No existing caller had to change.
- **`Enemy` class (#19):** each turn it re-plans with its `Pathfinder` and moves up to `stepsPerTurn` cells toward its target, stopping on it. If no route exists it stays put. It takes a `Pathfinder&`, so it works with A* or BFS without knowing which.

## Bugs & Challenges

| Problem | Cause | Fix |
|---|---|---|
| A wall bump redrew the whole maze, pushing the message off-screen on big mazes | The loop always redrew after input | Only redraw after a successful move |
| "You left the maze after 1 moves" | The plural was hard-coded | `movesText()` helper |
| How to test movement without a human pressing keys | Play mode reads from the keyboard | `Player` doesn't read input itself, so the self-test can drive it directly, e.g. walking BFS's path on 100 random mazes |
| Both enemy chase checks failed at first | A bug in the *test*: the loop limit used a distance that shrinks every turn, so the chase stopped halfway | Fix the turn limit once before the loop. A mutation test (forcing the enemy to 1 step) then confirmed the checks really catch speed bugs |

## Design note

`Player` only knows how to move. It doesn't read input or draw anything. That made it testable on its own, and it means the `Enemy` (#19) can share the same `Cell*`-based position idea while choosing its moves with A* instead of the keyboard.
