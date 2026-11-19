# Milestone 4 Test Log

**Build:** Visual Studio 2026 (MSBuild 18, toolset v145), C++20, Debug | x64. Build succeeded, 0 warnings, 0 errors.

**Final result (end of Week 4):** the self-test passes **98 of 98** checks, and all 12 scripted menu sessions behave the same as before each change (the only differences were the new menu lines each Issue added). Issues #25-#29 are all Done.

## Saving a maze (#25)

### Self-test additions (now 81 checks, all passing)

```
Saving mazes:
  PASS  Ordinary file names (spaces, dashes, underscores, digits) are accepted
  PASS  Blank, too-long, reserved (CON, NUL, COM3) and illegal-character names are all rejected with a reason
  PASS  ".txt" is added only when the name has no extension
  PASS  Saving a 41 x 21 maze writes exactly its layout, one row per line (and creates the folder)
  PASS  Saving somewhere that can't be written reports an error instead of crashing

81 passed, 0 failed
```

The saving checks write to a private folder in the system temp directory and delete it afterward, so the self-test never touches the user's real `mazes` folder.

### Menu tests

These were run in a separate scratch folder, so test files never land in the repo.

| # | Input at "File name (or 0 to cancel)" | Expected | Actual | Pass |
|---|---|---|---|---|
| 1 | Blank line | Asks again | "Type something, then press Enter." | ✅ |
| 2 | `bad:name` | Rejected with the reason | `File names can't contain ":". Please avoid these characters: < > : " / \ \| ? *` | ✅ |
| 3 | `CON` | Rejected (reserved by Windows) | `"CON" is a name Windows reserves for devices. Please choose another name.` | ✅ |
| 4 | `small maze` | Saved as `small maze.txt` with the full path shown | `Saved to ...\mazes\small maze.txt`; the file is exactly the Small maze, 7 rows | ✅ |
| 5 | `small maze` again, then `n`, then `0` | Asks before replacing; "no" asks for another name; `0` cancels | `"small maze.txt" already exists. Replace it?` ... `Not saved.` | ✅ |
| 6 | `small maze.txt`, then `y` | Replaces the file (no second `.txt` added) | `Saved to ...\mazes\small maze.txt` | ✅ |
| 7 | Input ends at the file name prompt | Exits cleanly | "Thanks for playing. Goodbye!" | ✅ |

**Fixed while testing:** the first wording, `File names can't contain :.`, was hard to read, so the character is now shown in quotes.

**Regression:** all 12 scripted sessions from Milestone 3 still run cleanly. The only differences are the new "4. Save this maze to a file" option, plus one old script that typed `4` at What next? (which used to be invalid and now opens Save). The baseline was refreshed to include the new option.

## Loading a maze (#26)

### Self-test additions (now 88 checks, all passing)

```
Loading mazes:
  PASS  Save then load, 100 random mazes: the loaded maze is identical and valid
  PASS  A missing file reports "There's no file named ..."
  PASS  An empty file, or one with only blank lines, is reported as empty
  PASS  A Notepad-style file (byte order mark, CRLF line endings, trailing blank line) loads correctly
  PASS  Files with too many rows or too-wide rows are rejected as too big
  PASS  A folder named like a maze file is reported as a folder, not opened
  PASS  Listing shows only .txt files, sorted by name; a missing folder lists nothing

88 passed, 0 failed
```

**Why the Notepad test matters:** when a file is saved from Notepad it can start with three invisible bytes (the UTF-8 "byte order mark") and use Windows `\r\n` line endings. Without handling those, a perfectly good hand-drawn maze would fail with "Unknown character" on row 1, or with a stray `\r` at the end of every row. The test writes those exact bytes and checks the maze loads cleanly.

### Menu tests (in a scratch folder with hand-made maze files)

| # | File chosen | Expected | Actual | Pass |
|---|---|---|---|---|
| 1 | (no `mazes` folder yet) | Explains where to put mazes, then back to the picker | `There are no saved mazes in ...\mazes yet. Save one from the What next? menu, or put a maze .txt file in that folder.` | ✅ |
| 2 | `b broken.txt` (row 3 shorter) | Specific reason, then back to the picker | `"b broken.txt" isn't a valid maze: Row 3 is a different length than the first row.` | ✅ |
| 3 | `c empty.txt` | Reported as empty | `Couldn't load that file: "c empty.txt" is empty.` | ✅ |
| 4 | `d letters.txt` (contains `X`) | Shows where | `Unknown character 'X' at row 2, column 3.` | ✅ |
| 5 | `e noGoal.txt` | Explains what's missing | `A maze needs exactly one start (S) and one goal (G).` | ✅ |
| 6 | `9` at the file list | Rejected | `9 isn't an option. Please enter a number from 0-5.` | ✅ |
| 7 | `a good.txt` (hand-made, CRLF line endings) | Loads and solves | `Loaded "a good.txt" (7 x 5)`, BFS 6-move path | ✅ |
| 8 | Save it as `copy`, then Compare, then load `copy.txt` | A full save/load round trip through the menus | `Saved to ...\copy.txt`, then `Loaded "copy.txt" (7 x 5)` with the side-by-side comparison | ✅ |

**Regression:** all 12 scripted sessions still run. The only difference is the new "5. Load a maze from a file" line in the maze picker.

## Best scores (#27)

### Self-test additions (now 94 checks, all passing)

```
Best scores:
  PASS  Best scores: the first win counts, then only fewer turns beat it (ties and worse don't)
  PASS  Scores save as readable lines (Medium=62) and load back the same
  PASS  A missing scores file is treated as a first run, with no scores
  PASS  A corrupt scores file keeps the good lines and reports the bad ones (text, negative, unknown, too big)
  PASS  If a difficulty appears twice, the better score is kept
  PASS  Saving scores somewhere that can't be written reports an error

94 passed, 0 failed
```

### Menu tests: five runs in a row in the same scratch folder

All five use `--seed 4242` on Easy and replay the perfect 16-move route.

| Run | Scores file before | Expected | Actual | Pass |
|---|---|---|---|---|
| A | (none) | First win is the best so far | `No best score on Easy yet. Win to set one!` ... `That's your first win on Easy, so it's your best so far!`; the file is now `Easy=16` | ✅ |
| B | `Easy=16` | A tie doesn't count as a new best | `Your best on Easy: 16 turns.` ... `Your best on Easy is still 16 turns.` | ✅ |
| C | `Easy=20` (edited by hand) | Beats the old score | `New best on Easy! (Your old best was 20 turns.)`; the file is now `Easy=16` | ✅ |
| D | `Easy=abc`, `Medium=5` | The bad line is reported and reset; Medium is kept | `Note: some saved best scores couldn't be read and were reset: line 1 ("Easy=abc").`; the file afterward is `Easy=16`, `Medium=5` | ✅ |
| E | `Easy=30`, **read-only** | A failed save is reported, and the game carries on | `Your score couldn't be saved: Couldn't write "...\scores.txt".` | ✅ |

**Regression:** all 12 scripted sessions still match. The only differences are the new best-score lines in the Play mode sessions.

## Save and Load tests, including bad files (#28)

Most of #28's list was already covered while building #25 to #27 (round trips, missing and empty files, unwritable paths, scores). This pass added the remaining cases: files that are readable text but not valid mazes, binary junk, overwriting, and every built-in maze.

### Self-test additions (now 98 checks, all passing)

```
Loading mazes:
  ...
  PASS  Every built-in maze (including the one with no path) saves and loads back identically
  PASS  Saving over a bigger maze file replaces it completely (no leftover rows)
  PASS  Garbage text, uneven rows, a missing goal and two starts are each rejected with a clear reason
  PASS  A binary file or a tab gives a readable message ("character code 0", "a tab"), not raw bytes
  ...

98 passed, 0 failed
```

The "not a maze" checks also confirm that a rejected file leaves the maze **empty**, so a half-loaded maze can never be solved or played by accident.

**Bug found and fixed:** a binary file (or one with a tab) used to print the raw character in the error, e.g. `Unknown character '' at row 1`. A NUL byte or a tab is invisible, and other bytes can garble the console. Now printable characters still show in quotes (`'X'`), and anything else is described: `Unknown character (character code 0) at row 1, column 1.` or `Unknown character (a tab) at row 2, column 3.`

### End-to-end menu test (one session, scratch folder)

| Step | Input | Expected | Actual | Pass |
|---|---|---|---|---|
| 1 | Solve, Small maze, BFS | Solves | `Path length: 10 moves` | ✅ |
| 2 | What next? 4, name `roundtrip` | Saved | `Saved to ...\mazes\roundtrip.txt` | ✅ |
| 3 | Solve, 5 (load from file), pick `binary.txt` (bytes `00 FF 01 23`) | Readable error, then the picker comes back | `"binary.txt" isn't a valid maze: Unknown character (character code 0) at row 1, column 1.` | ✅ |
| 4 | 5, pick `roundtrip.txt`, BFS | Loads the same maze, same answer | `Loaded "roundtrip.txt" (9 x 7)`, `Path length: 10 moves` | ✅ |
| 5 | Back to the main menu, quit | Exits cleanly | `Thanks for playing. Goodbye!` | ✅ |

**Regression:** all 12 scripted sessions are byte-for-byte identical to the #27 baseline.

## Final polish: How to play and consistent prompts (#29)

### What changed (wording and layout only; no feature works differently)

- **"4. How to play"** on the main menu: what each mode does, the difficulty settings, every symbol (S, G, #, space, ., o, P, E, X), and the full paths of the saved mazes folder and the scores file. The difficulty lines are built from `Game::getSettings()`, so they can't go out of date.
- **"0. Exit"** instead of "4. Exit". Every other menu already used 0 to leave, so 0 now means "back/leave" everywhere.
- "Back to main menu" is now "Back to **the** main menu" in all five places, matching "Back to the maze list".
- Correct plurals in results: `Path length: 1 move` (was "1 moves") and "explored 1 fewer cell". `countText()` moved from `Menu` into `ResultView` so both use it.
- "Could not load that maze" became "  Couldn't load that maze", with the same indent and contraction as every other error.
- The Solve menu description now mentions saved mazes, and an outdated comment in `MazePresets.h` was fixed.

### Menu tests

| # | Input | Expected | Actual | Pass |
|---|---|---|---|---|
| 1 | `4` | How to play screen, then waits | Full screen shown, ending with `Press Enter to return to the main menu...` | ✅ |
| 2 | Enter | Back to the main menu | Main menu shown with `4. How to play` and `0. Exit` | ✅ |
| 3 | `5` | Rejected with the new range | `5 isn't an option. Please enter a number from 0-4.` | ✅ |
| 4 | `0` | Exits | `Thanks for playing. Goodbye!` | ✅ |
| 5 | `4`, then input ends at "Press Enter" | Exits cleanly, no loop | `Thanks for playing. Goodbye!` | ✅ |

**Regression:** the 9 scripts that exited with `4` now exit with `0`. Then all 12 sessions were compared with the #28 baseline. After replacing only the expected wording (new menu lines, `(0-4)` range, "the main menu"), all 12 are **identical**, so nothing about how the features behave changed. The self-test still passes 98 of 98. The baseline was refreshed afterward.
