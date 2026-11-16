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
