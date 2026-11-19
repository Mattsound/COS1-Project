# Milestone 4 Progress Notes (in progress)

Week 4 focus: **File I/O (Save and Load)**, then testing, polish, and the final showcase.

## Week 4 Issues

| Issue | Work | Status |
|---|---|---|
| #11 | Save and Load (parent) | ✅ Done |
| #25 | Save the current maze to a text file | ✅ Done |
| #26 | Load a maze from a text file | ✅ Done |
| #27 | Save and load Play mode best scores | ✅ Done |
| #28 | Test Save and Load, including bad files | ✅ Done |
| #29 | Final polish: How to play screen and consistent prompts | 📋 To Do |

## Completed so far

- **`MazeFile` module (#25):** saves a maze as **plain text, one row per line**, using the same characters as the built-in mazes (`#`, `S`, `G`, spaces). Saved mazes can be opened, edited, or even drawn from scratch in Notepad, and loading (#26) will reuse the existing `Maze::loadFromLayout()` validation.
  - `isValidName()` rejects blank, too-long, Windows-reserved (`CON`, `NUL`, `COM1`...) and illegal-character names, with a reason.
  - `withExtension()` adds `.txt` when there's no extension.
  - `save()` creates the `mazes` folder if needed, and reports an error instead of crashing if the file can't be written.
- **`Maze::getLayout()`:** turns the grid back into text rows.
- **"4. Save this maze to a file"** in the What next? menu: asks for a name (0 cancels), asks before replacing an existing file, and shows the full path on success. Afterwards it returns to What next?, since the maze hasn't changed.
- **`InputHelper::readText()`** and `Menu::askText()` for typed text, following the same end-of-input pattern as the other prompts.
- **Loading (#26):** `MazeFile::load()` reads a file line by line, handles Notepad's byte order mark, Windows line endings and trailing blank lines, and rejects missing, empty, too-big or folder paths with a clear message. `MazeFile::listMazes()` lists the saved `.txt` files. In the maze picker, **"5. Load a maze from a file"** shows them by number. A file that isn't a valid maze gets the same specific reasons as the built-in checks (e.g. *"Row 3 is a different length than the first row"*), and the picker comes back instead of leaving the user stuck.
- **The File I/O requirement (Save AND Load, handling missing files) is now met.**
- **Best scores (#27):** a new `ScoreBoard` class keeps the fewest turns to win on each difficulty in `scores.txt`, as readable lines like `Medium=62`. It loads at startup and saves after every win. Play mode shows your best before each game and announces a first win or a new best afterward. A **missing** file just means no wins yet. A **damaged** file keeps every line it can read and reports the rest once at startup, so one bad line never wipes the other scores. A failed save is reported and the game carries on.
- **Save/Load tests (#28):** 4 more self-test checks (98 total) for every built-in maze round trip, overwriting a bigger file, garbage text, uneven rows, missing goal, two starts, and binary files. Error messages for unprintable characters are now readable (`(character code 0)`, `(a tab)`) instead of printing raw bytes.

## Design decisions

- **Plain text over a custom format:** a maze file is just the maze, so it's readable, editable and easy to check by eye. The trade-off is that a random maze's seed isn't saved, which is fine because the full layout is.
- **A `mazes` folder next to where the program runs:** saved files are easy to find, the program shows the full path after saving, and the folder is in `.gitignore`, so files saved while testing in Visual Studio don't end up in the repository.

## Bugs & Challenges

| Problem | Fix |
|---|---|
| Error text "can't contain :." was confusing | Show the character in quotes: `can't contain ":"` |
| A test run could save a file into whatever folder it ran from | The test scripts now run in their own scratch folder, and the self-test uses a temp folder it deletes afterward |
| A good maze drawn in Notepad could fail to load | Notepad can add an invisible byte order mark and Windows (`\r\n`) line endings; `load()` strips both |
| A binary file printed raw, invisible bytes in its error message | `Maze` now describes unprintable characters: `(character code 0)`, `(a tab)` |

## Possible future improvement

Best scores count **turns**, but random mazes vary in length, so a lucky short maze makes an easier record. A fairer score would be the **extra moves** compared with the shortest route, where 0 means a perfect run. I kept turns because that's what Issue #27 specified, and it's easy to understand.
