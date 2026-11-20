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
| Persistent menu with validated input and clear error messages | ✅ Done |
| **What next? options:** after results, solve the same maze with another algorithm, compare on it, or pick a new maze without going back to the main menu | ✅ Done |
| **Solve a maze:** pick a built-in maze and an algorithm (BFS or A*), then see the solved path, its length and the number of cells explored | ✅ Done |
| Detects and reports mazes with no possible path | ✅ Done |
| **Compare algorithms:** BFS vs. A* on the same maze, drawn side by side with a stats table | ✅ Done |
| **Random mazes:** Easy, Medium or Hard, generated with the recursive backtracker and shown with their seed | ✅ Done |
| **Explored-cells overlay:** see every cell each algorithm searched (`o`) under the path (`.`), side by side in Compare mode | ✅ Done |
| Automated self-test (`MazePathfinder.exe --selftest`): 98 checks covering random mazes, every algorithm, Play mode rules and fairness, maze edge cases, tricky input, and saving and loading files (including bad and binary files) | ✅ Done |
| **Play mode:** reach G with W/A/S/D before an enemy that chases you with A* catches you; Easy, Medium or Hard, always winnable | ✅ Done |
| **Save a maze** to a plain-text file you can open in Notepad (from the What next? menu) | ✅ Done |
| **Load a maze** from a file (saved ones, or your own drawn in Notepad), with clear messages for missing or broken files | ✅ Done |
| **Best scores:** fewest turns to win on each difficulty, saved between runs | ✅ Done |
| **How to play** screen (main menu option 4): every mode, the symbols, and where files are saved | ✅ Done |

## Class Plan

| Class | Responsibility | Status |
|---|---|---|
| `Menu` | Runs the main menu loop, reads choices, and decides what happens next in each mode | ✅ |
| `ResultView` | Prints Solve and Compare results: solved mazes, the explored overlay, side-by-side mazes and the stats table | ✅ |
| `Cell` | One square of the maze: position, type (open, wall, start, goal) and a parent pointer used to rebuild paths | ✅ |
| `Maze` | Owns the `vector<vector<Cell>>` grid. Builds it from a text layout, finds walkable neighbors and draws itself as ASCII | ✅ |
| `Pathfinder` | Abstract base class, so every search algorithm shares one interface | ✅ |
| `BfsPathfinder` | Breadth-First Search, which always finds the shortest path | ✅ |
| `AStarPathfinder` | A* search with a Manhattan-distance heuristic, which usually explores far fewer cells than BFS | ✅ |
| `MazeGenerator` | Builds random perfect mazes from a seed with the recursive backtracker (randomized DFS) | ✅ |
| `MazeFile` | Saves and loads mazes as plain-text files, checks file names, lists saved mazes | ✅ |
| `MazeFileMenu` | The Save and Load screens: asks for a file name, lists saved mazes and explains problems; holds a reference to `Menu`'s maze | ✅ |
| `ScoreBoard` | Play mode best scores, saved to and loaded from `scores.txt` | ✅ |
| `Player` | Play mode: moves one cell at a time with W/A/S/D, refusing walls and the grid edge | ✅ |
| `Enemy` | Play mode: re-plans with a `Pathfinder` every turn and moves toward the player (speed is adjustable) | ✅ |
| `Game` | Play mode rules (turns, win/lose, difficulty) and a fair enemy start with a calculated head start; no console I/O, so it can be self-tested | ✅ |

Helpers: `InputHelper` (validated console input), `MazePresets` (built-in maze layouts) and `SelfTest` (automated checks).

## Repository Structure

```
dev/       Visual Studio solution and C++ project (include/ = headers, src/ = source)
docs/      planning/, testing/, changelog/, and the Version Control reference
images/    screenshots
```

## Build & Run

1. Open `dev/MazePathfinder.sln` in Visual Studio 2022 or newer.
2. Select the **x64** platform and press **Ctrl + F5**.
3. New here? Choose **4. How to play** on the main menu. Choose **0** to exit.

**Command-line options** (run `dev\x64\Debug\MazePathfinder.exe` from a terminal):
- `--selftest`: runs the automated checks and exits with code 0 if all pass
- `--seed 12345`: every random maze uses this seed, so a maze can be shared or replayed

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

## Week 2

### 🔎 Overview
This week I finished everything in my Milestone 2 plan and merged it into `main`:
- **A\* pathfinder:** uses a priority queue with a Manhattan-distance heuristic.
- **Compare mode:** runs BFS and A* on the same maze side by side, with a stats table.
- **Random maze generation:** a new `MazeGenerator` class using the recursive backtracker, with Easy, Medium and Hard sizes, and seeded so a maze can be recreated.
- **Explored-cells overlay:** shows every cell each algorithm searched, so you can *see* BFS flood the maze while A* heads straight for the goal.
- **"What next?" menu after results:** lets you try another algorithm on the same maze without going back through every menu.

I also improved usability: prompts show their valid range, error messages are clearer, and there's a yes/no helper. The full write-up is in [docs/MMoreno_Milestone2.md](docs/MMoreno_Milestone2.md).

### 🧟 Challenges
- **`std::priority_queue` is a max-heap by default,** so A* kept pulling out the *worst* cell first. I fixed it with a custom comparator. A* could also queue the same cell twice, so I added a "finished" flag that skips outdated entries.
- **The maze generator's grid math:** walls sit *between* rooms, so sizes have to be odd and every step moves two cells. Checking hundreds of mazes by eye wasn't realistic, so I wrote a `--selftest` mode. It uses the fact that a perfect maze with R rooms always has exactly 2R − 1 open cells to confirm 300 generated mazes are fully connected and solvable.
- **GitHub board:** linking a PR with "Closes #N" moved Issues I'd already closed back to In Progress. I fixed the statuses and now reference Issues without that keyword.

### 🏆 Accomplishments
I leveled up on **testing my own code**. The self-test now runs 27 automated checks: 300 random mazes, the explored-cell lists, a maze with no path, and every layout error case. To make sure the tests could actually catch a bug, I broke the generator on purpose. The self-test reported 9 failures, then went back to 27/27 once I restored it. I also got more comfortable with abstract base classes: because BFS and A* share a `Pathfinder` interface, Compare mode and the overlay didn't need any algorithm-specific code.

### 🔮 Next Steps
- **Play mode (#10):** the player moves through the maze with W/A/S/D while an AI enemy chases them using the same pathfinding code.
- **Save/Load (#11) in Week 4:** the maze seeds I'm already showing will make saving a maze simple.

## Week 3

### 🔎 Overview
This week I finished **Play mode** and spent the rest of the week on **reliability**:
- **Play mode (#10):** you move P with W/A/S/D and try to reach G before an enemy catches you. It's built from three new classes:
  - `Player`: movement, refusing walls and the grid edge
  - `Enemy`: re-plans with A* every turn
  - `Game`: the rules, with no console input or output, so the self-test can play thousands of games
- **Pathfinder refactor (#19):** the enemy needs to chase the *player*, not G, so the algorithms can now search between any two cells.
- **Refactor (#21):** `Menu.cpp` went from 477 to 364 lines. A new `ResultView` class prints all the results, and three pieces of duplicated code are gone.
- **Edge-case tests (#22):** the self-test went from 27 checks at the start of Week 2 to **76**.

### 🧹 Improvements
- **`ResultView` class:** moved all result printing (solved mazes, the explored overlay, side-by-side comparison, the stats table) out of `Menu`. Now `Menu` decides *what* to show and `ResultView` decides *how it looks*.
- **Less duplicated code:**
  - `askInt()`, `askKey()` and `askYesNo()` replaced 8 copies of the "if input ended, stop the menu" block
  - the two Easy/Medium/Hard menus became one `chooseDifficulty()`
  - a helper that existed in two files is now `Player::directionBetween()`
- **Pathfinding between any two cells:** algorithms implement `findPathBetween()`, and `findPath()` became a simple wrapper. The enemy needed it, and no existing code had to change.
- **Faster fairness check:** replaced a full-game simulation per cell with one distance calculation.
- **Clearer error message:** a number too big to store now says "too large".
- **Result:** `Menu.cpp` went from 477 to 364 lines, and the program's output stayed byte-for-byte identical through every refactoring step.

### 🧟 Challenges
- **Most games were impossible to win.** Random mazes have exactly one route to G, so an enemy as fast as you could usually cut you off. A fair start existed in only 16–56% of mazes. My fix: the enemy sleeps in its lair, and the game calculates the *smallest* head start that still lets a perfect run escape, then adds spare turns by difficulty (Easy +6, Medium +3, Hard +0). The self-test proves it on 300 mazes: the shortest route always wins, and one turn less gets you caught.
- **A self-test that never finished.** My first fairness check simulated a whole game for every possible enemy start. I replaced it with one distance calculation per cell, and it now takes seconds.
- **Bugs I found by testing:**
  - a test whose loop limit shrank every turn
  - a few tiny mazes that were just one corridor with nowhere to put the enemy
  - typing `99999999999` said "isn't a whole number" (it's now "too large")

### 🏆 Accomplishments
I leveled up on **refactoring safely**. Before changing anything, I saved the exact output of 12 scripted sessions plus the self-test, using fixed maze seeds so random mazes repeat exactly. After each refactoring step I compared everything **byte for byte**, and it stayed identical every time, which proves the structure changed without changing the behavior. I also learned that C++ builds class members in the order they're declared. That mattered, because `ResultView` holds references to other `Menu` members.

### 🔮 Next Steps
- **Week 4: Save and Load (#11).** Save mazes (seed and size, or the full layout for custom mazes), and handle missing or broken files without crashing.
- Final documentation, the showcase, and the final Pull Request.

## Week 4

### 🔎 Overview
This was the final week. I added **File I/O**, tested it against every bad file I could think of, and polished the program:
- **Save a maze (#25):** "Save this maze to a file" in the What next? menu writes the maze as plain text, so it opens in Notepad.
- **Load a maze (#26):** "Load a maze from a file" in the maze picker lists every saved maze, plus any maze drawn by hand in Notepad.
- **Best scores (#27):** a new `ScoreBoard` class saves the fewest turns to win on each difficulty to `scores.txt` and loads it when the program starts.
- **Save/Load tests (#28):** the self-test went from 76 to **98 checks**, including garbage, uneven, binary and overwritten files.
- **Final polish (#29):** a **How to play** screen, **0** to exit like every other menu, and consistent wording everywhere.

### 🧹 Improvements
- **Readable error messages for bad files:** a broken maze file says exactly what's wrong and where (`Row 3 is a different length than the first row.`), and a binary file says `(character code 0)` instead of printing invisible bytes.
- **Notepad-friendly loading:** Notepad's invisible byte order mark, Windows line endings and trailing blank lines are all handled.
- **A damaged scores file is repaired, not wiped:** good lines are kept, and bad ones are reported once.
- **Consistent menus:** 0 always means back or exit, "Back to the main menu" is worded the same everywhere, and plurals are correct ("1 move").
- **Shared code:** `countText()` moved into `ResultView` so `Menu` and the results use one helper.
- **`MazeFileMenu` class (#32):** moved the Save and Load screens out of `Menu`, which went from 640 to 550 lines. The new class holds a reference to the maze and returns a result (`Loaded`, `NotLoaded` or `InputEnded`) instead of reaching into `Menu`. The output stayed byte-for-byte identical.

### 🧟 Challenges
- **Files are messier than keyboard input.** A maze I drew in Notepad failed to load with "Unknown character" on row 1, even though it looked perfect. Notepad had added three invisible bytes (a byte order mark) at the start and `\r` at the end of every line. `MazeFile::load()` now strips both, and a self-test writes those exact bytes to prove it.
- **Testing without making a mess.** My first test runs could have saved files into whatever folder they ran from. The self-test now uses a temp folder it deletes afterward, and my scripted tests run in their own scratch folder.
- **Making sure scores really persist.** One run can't prove that. I ran the program five separate times in a row: a first win, a tie, beating an older score, a corrupt file, and a read-only file.

### 🏆 Accomplishments
I leveled up on **File I/O and defensive programming**. I learned to treat every file as untrusted input: it might be missing, empty, a folder, too big, edited by hand or not text at all. Every case now gives a clear message instead of a crash. I also kept `ScoreBoard` free of console output, just like `Game` in Week 3, so the self-test could check each of those cases automatically.

### 🔮 Next Steps
- **Showcase presentation** and the final Pull Request into `main`.
- Future ideas (in Backlog): score by *extra moves* over the shortest route instead of turns (#31), and save a Play mode game in progress (#33).

## 🏁 Final Project Stand-Up

### ⚙️ Overview
This week I added **File I/O** and finished the project:
- **Save and load mazes** as plain-text files (you can even draw your own in Notepad)
- **Best scores** for Play mode, saved between runs
- The final PRs merged into `main`, and every planned Issue on my board is Done

### 🛠️ Project Wrap-Up
- Tested Save/Load against every bad file I could think of: missing, empty, uneven, binary, and read-only. Each one gets a clear message instead of a crash.
- Added a **How to play** screen and made every menu consistent (0 always goes back or exits).
- Moved the Save and Load screens out of `Menu` into a new `MazeFileMenu` class (#32), so `Menu.cpp` went from 640 to 550 lines with no change in behavior.
- The self-test grew to **98 automated checks**.
- Finished the docs: Milestone 4 write-up, test logs, final screenshots, and a requirements checklist.

### 🌵 Challenges
- A maze I drew in Notepad wouldn't load even though it looked perfect. Notepad had added invisible bytes and Windows line endings, so my loader now strips both.
- Earlier, my first Play mode made most games unwinnable. My automated tests caught it, and I fixed it by calculating a fair head start for the enemy.

### 📚 Biggest Takeaway
**Test your own code automatically.** I came in worried about pointers and debugging. A `--selftest` mode that plays hundreds of games found bugs I never would have caught by hand. It also let me refactor safely, because I could prove the output didn't change. Breaking work into small GitHub Issues kept a big project manageable.

### 🔮 Future Development
- Score by *extra moves* over the shortest route instead of turns (#31)
- Save and resume a game in progress (#33)
- Long term: add weighted search (Dijkstra), since my degree is in AI and pathfinding is a core AI topic
