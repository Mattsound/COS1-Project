# Milestone 4 Test Log (in progress)

**Build:** Visual Studio 2026 (MSBuild 18, toolset v145), C++20, Debug | x64. Build succeeded, 0 warnings, 0 errors.

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
