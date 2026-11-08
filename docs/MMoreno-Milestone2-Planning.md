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
→   
  
**What does this feature do?**   
→   
  
**Why is this feature important?**   
→   
  
---
### Feature 2  
**Name:**   
→   
  
**What does this feature do?**   
→   
  
**Why is this feature important?**   
→   
  
---
  
### Feature 3 (optional)  
**Name:**   
→   
  
**What does this feature do?**   
→   
  
**Why is this feature important?**   
→   
  
---
## 🧩 System Design Updates  
  
**Will you need to create any new classes? If so, which ones?**   
→   
  
**Will you modify any existing classes? How?**   
→   
  
**What data structures will you use (vectors, 2D vectors, etc.)?**   
→   
  
---
## 🔄 Program Flow  
  
**Describe how a user interacts with your program:**  
  
1. Program starts →   
2. User chooses →   
3. Program responds →   
4. Loop/next step →   
  
---
## 🎯 Usability Improvements  
  
How will you make your program easier to use this week?  
  
- Clearer prompts:   
→   
  
- Better error handling:   
→   
  
- Improved menu/navigation:   
→   
  
---
## ⚠️ Potential Challenges  
  
**What do you think will be the hardest part this week?**   
→   
  
**What is your plan if you get stuck?**   
→   
  
---
  
## 📈 Level Up Goal  
  
**What skill are you focusing on improving this week?**   
→   
  
**What will you do to improve it?**   
(e.g., tutorial, practice, debugging, office hours)   
→   
  
---
## 🗓️ Task Breakdown (GitHub Issues Planning)  
  
List the tasks you plan to create as GitHub Issues:  
  
- [ ]   
- [ ]   
- [ ]   
- [ ]   
  
---
  
## 🔥 Final Check  
  
Before you start coding, ask yourself:  
  
- [ ] Do I know what I’m building this week?   
- [ ] Do I know where to start?   
- [ ] Did I break my work into small tasks?   
  
If yes → start coding 🚀   
If no → refine your plan first   
  
---
## 😈 Final Thought  
  
> Plan it now… or debug it later.