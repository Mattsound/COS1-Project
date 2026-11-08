**COS1 - Project and Portfolio I: Computer Science**
**Matt Moreno**
**November 8, 2026**

> Use this worksheet to plan the next phase of your project **before you begin coding**
> Be clear, specific, and intentional—this will guide your development this week.

---
## 📌 Project Overview

**Project Name:**
→ Maze Pathfinder

**What does your program currently do? (1–3 sentences)**
→ Maze Pathfinder draws mazes in the console as ASCII art and solves them with real pathfinding algorithms: Breadth-First Search (BFS) and A*. The user can solve a maze with either algorithm and see the shortest path with its length and the number of cells explored. They can also run Compare mode, which solves the same maze with both algorithms side by side to show that A* explores far fewer cells.

---
## 🔍 Current Progress Check

**What is working right now?**
→
- A persistent main menu with validated input. Every prompt shows its valid range, errors explain what was wrong, and results stay on screen until Enter is pressed.
- **Solve a Maze:** pick one of 3 built-in mazes and an algorithm (BFS or A*), then see the solved path, its length and the cells explored.
- **Compare mode:** BFS and A* on the same maze, drawn side by side with a stats table. On the Medium maze A* explores 39 cells vs. BFS's 66 (40% fewer), and both find the same 30-move path.
- Mazes with no possible path are detected and reported instead of crashing.
- Classes: `Menu`, `Cell`, `Maze`, `Pathfinder` (abstract base), `BfsPathfinder` and `AStarPathfinder`, plus the `InputHelper` and `MazePresets` helpers.

**What is NOT working or incomplete?**
→
- There are only 3 hand-made mazes, so after a few runs there's nothing new to solve.
- **Play mode** is still a "coming soon" message.
- The program only shows the final path, not *how* each algorithm searched. You see a number like "66 cells explored" but not which cells those were.
- `Maze::loadFromLayout()` has error checks (uneven rows, unknown characters, missing S or G) that have never been tested, because the built-in mazes are all valid.

**What feels confusing or messy in your code?**
→
- `Menu::compareAlgorithms()` is getting long. It runs the searches, draws the mazes side by side, prints the table and writes the summary all in one function. I plan to split the drawing and the table into their own helper functions.
- `Menu` is starting to do too much (menus, input flow and result formatting). As features grow, the result display might belong in its own class.

---
## 🚀 Feature Planning

List the features you plan to add or improve this week.

### Feature 1
**Name:**
→ Random maze generation with difficulty levels (GitHub Issue #9)

**What does this feature do?**
→ It generates a brand-new maze every time using the **recursive backtracker** algorithm, a randomized depth-first search. The generator starts with a grid that is all walls, carves a path from cell to cell in a random order, and backtracks with a stack when it hits a dead end. The result is a "perfect" maze: every open cell is reachable, and there's exactly one route between any two points. The user picks a difficulty:
- **Easy:** 11 x 7
- **Medium:** 21 x 11
- **Hard:** 41 x 21

The maze's **seed** number is shown so the same maze can be recreated later.

**Why is this feature important?**
→ It's the biggest thing missing. With only 3 built-in mazes, the program gets repetitive fast. Random mazes give unlimited replay value, and they make Compare mode much more interesting because the results change every time. It's also a second real algorithm (maze *generation*) on top of the pathfinding ones. The seed sets up Week 4's Save/Load, since saving a maze can be as simple as saving its seed and size.

---
### Feature 2
**Name:**
→ Explored cells overlay ("how did it search?")

**What does this feature do?**
→ After solving, the user can choose to see every cell the algorithm explored, marked with `o`, with the final path still shown as `.`. In Compare mode the two overlays appear side by side, so you can *see* BFS spreading out in every direction while A* heads toward the goal.

**Why is this feature important?**
→ It explains the stats table visually. "A* explored 40% fewer cells" is just a number until you can see which cells each algorithm skipped. That's the main educational point of the program: the heuristic is what makes A* smarter.

---

### Feature 3 (optional)
**Name:**
→ Quick options after results

**What does this feature do?**
→ After a maze is solved, instead of always going straight back to the main menu, the user gets:
`1. Same maze, different algorithm` / `2. New maze` / `0. Main menu`

**Why is this feature important?**
→ Right now, trying A* on a maze you just solved with BFS means going back through 3 menus. Quick options make experimenting fast, which is the whole point of a comparison tool.

---
## 🧩 System Design Updates

**Will you need to create any new classes? If so, which ones?**
→ Yes, **`MazeGenerator`**. It creates the random mazes, which keeps the "how to build a maze" logic out of `Maze`, the same way `Pathfinder` keeps "how to solve a maze" out of it. It will:
- hold a `std::mt19937` random number generator
- have a `generate(width, height, seed)` function that returns a text layout (`std::vector<std::string>`) which `Maze::loadFromLayout()` already knows how to read

**Will you modify any existing classes? How?**
→
- **`PathResult` (in `Pathfinder.h`):** add `std::vector<Cell*> explored`, the cells in the order the search visited them.
- **`BfsPathfinder` / `AStarPathfinder`:** record each cell in `explored` as it comes off the queue (one line each).
- **`Maze`:** `renderLines()` takes an optional explored list and draws those cells as `o`.
- **`Menu`:**
  - a "Choose a maze" step that offers the presets or a random maze at a chosen difficulty
  - the post-result quick options
  - split `compareAlgorithms()` into smaller helper functions

**What data structures will you use (vectors, 2D vectors, etc.)?**
→
- `std::vector<std::string>`: the generated layout, the same format the presets use
- `std::stack<std::pair<int,int>>`: the backtracker's path, so it can step back from dead ends
- `std::vector<std::vector<bool>>`: which cells the generator has already carved
- `std::vector<Cell*>`: the explored-cells list in `PathResult`
- `std::mt19937` and `std::uniform_int_distribution`: random numbers, seeded so a maze can be recreated
- Already in use: `std::vector<std::vector<Cell>>` (the grid), `std::queue` (BFS), `std::priority_queue` (A*), and `std::vector<std::unique_ptr<Pathfinder>>` (the algorithms)

---
## 🔄 Program Flow

**Describe how a user interacts with your program:**

1. Program starts → the main menu shows Solve a maze / Play mode / Compare algorithms / Exit, each with a one-line description.
2. User chooses → Solve or Compare, then picks a maze source: one of the 3 built-in mazes, or a **random maze** at Easy, Medium or Hard. For Solve, they also pick BFS or A*.
3. Program responds →
   - generates or loads the maze (showing the seed if it's random) and draws it
   - runs the algorithm(s) and draws the solved maze with the path
   - prints the stats: path length, cells explored, and a comparison summary
   - offers "Show explored cells? (y/n)"
4. Loop/next step → the quick options appear: same maze with a different algorithm, a new maze, or back to the main menu. The program keeps looping until the user picks Exit.

---
## 🎯 Usability Improvements

How will you make your program easier to use this week?

- Clearer prompts:
→ The difficulty menu shows the maze size next to each level, e.g. "1. Easy (11 x 7)", so users know what they're picking. When a random maze is made, the program prints "Generated maze #48213 (Medium, 21 x 11)" so the user knows it's new and can mention the seed.

- Better error handling:
→ Add a yes/no input helper (`InputHelper::readYesNo`) that accepts `y`, `yes`, `n`, `no` in any capitalization and re-asks on anything else, with the same clear error style as the number prompts. Also test `Maze::loadFromLayout()`'s error messages with broken layouts so they work before Week 4, when users will load mazes from files.

- Improved menu/navigation:
→ The post-result quick options mean fewer steps back through menus. Every submenu keeps a "0. Back" option, so the user is never stuck.

---
## ⚠️ Potential Challenges

**What do you think will be the hardest part this week?**
→ Getting the maze generator's grid math right. The recursive backtracker works on a grid where walls sit *between* cells. That means maze dimensions must be odd, the "rooms" are at odd coordinates, and carving from one room to the next also has to open the wall cell between them. Off-by-one errors here would leave walls missing on the border or make parts of the maze unreachable. I'll also need to make sure the start and goal always land on open cells.

**What is my plan if I get stuck?**
→
1. Test on the smallest size first (5 x 5) and print the grid after every carving step to watch what happens.
2. Use the existing BFS as an automatic checker. A correctly generated maze must always have a path from S to G, so I'll run BFS on 100 random seeds and confirm every one is solvable.
3. Step through the code in the Visual Studio debugger, watching the stack.
4. Review the maze generation articles in my research, and ask in `#cos1-questions-chat` or at office hours if I'm still stuck.

---

## 📈 Level Up Goal

**What skill are you focusing on improving this week?**
→ Debugging algorithms with the Visual Studio debugger (breakpoints, watch windows, stepping), and writing code that tests itself.

**What will you do to improve it?**
(e.g., tutorial, practice, debugging, office hours)
→
- Use conditional breakpoints in the generator, e.g. "break when the stack size is 0", to watch the backtracking happen.
- Watch the `std::stack` and the carved grid in the Watch window instead of adding `cout` statements.
- Write the 100-seed "every maze is solvable" check, so the program can catch generator bugs by itself.
- Review Microsoft's "First look at the debugger" documentation for the features I haven't used yet.

---
## 🗓️ Task Breakdown (GitHub Issues Planning)

List the tasks you plan to create as GitHub Issues:

- [ ] **#9 (existing):** Create `MazeGenerator` with the recursive backtracker and Easy/Medium/Hard difficulty
- [ ] Add random mazes to the maze picker (show the seed, keep the presets available)
- [ ] Record explored cells in BFS/A* and add the explored-cells overlay
- [ ] Add quick options after results (same maze / new maze / main menu)
- [ ] Add a yes/no input helper and refactor `compareAlgorithms()` into smaller functions
- [ ] Test `Maze::loadFromLayout()` error handling and verify that 100 random seeds are all solvable

---

## 🔥 Final Check

Before you start coding, ask yourself:

- [x] Do I know what I'm building this week?
- [x] Do I know where to start?
- [x] Did I break my work into small tasks?

If yes → start coding 🚀
If no → refine your plan first

**Where I'm starting:** `MazeGenerator` (#9), tested on its own with the 100-seed BFS check before it's added to the menu. Everything else this week builds on having random mazes.

---
## 😈 Final Thought

> Plan it now… or debug it later.

---

## 🔎 Research & Inspiration

How similar programs solve these problems, and what I'm taking from each:

- **Pathfinding visualizers** (like the well-known browser-based "Pathfinding Visualizer" projects) show the explored cells in one color and the final path in another. That's the inspiration for Feature 2, with `o` for explored and `.` for the path.
- **Roguelike games** (like *NetHack* and *Brogue*) generate a new ASCII map every run, often from a seed. That's where the idea of showing the seed came from.
- **Jamis Buck's maze generation articles** compare algorithms like recursive backtracker, Prim's and Kruskal's. The recursive backtracker creates long, winding corridors, which make BFS vs. A* differences more dramatic.
- **Red Blob Games' A* introduction** explains why the heuristic matters, with visual examples of the frontier spreading. It helped me explain Compare mode's results.

**Links**
- [Jamis Buck: Maze Generation - Recursive Backtracking](https://weblog.jamisbuck.org/2010/12/27/maze-generation-recursive-backtracking)
- [Red Blob Games: Introduction to A*](https://www.redblobgames.com/pathfinding/a-star/introduction.html)
- [cppreference: std::mersenne_twister_engine (std::mt19937)](https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine)
- [Microsoft Learn: First look at the Visual Studio debugger](https://learn.microsoft.com/en-us/visualstudio/debugger/debugger-feature-tour)
