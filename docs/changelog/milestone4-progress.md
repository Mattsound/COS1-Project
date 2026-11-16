# Milestone 4 Progress Notes (in progress)

Week 4 focus: **File I/O (Save and Load)**, then testing, polish, and the final showcase.

## Week 4 Issues

| Issue | Work | Status |
|---|---|---|
| #11 | Save and Load (parent) | 🚧 In Progress |
| #25 | Save the current maze to a text file | ✅ Done |
| #26 | Load a maze from a text file | 📝 To Do |
| #27 | Save and load Play mode best scores | 📝 To Do |
| #28 | Test Save and Load, including bad files | 📥 Backlog |
| #29 | Final polish: How to play screen and consistent prompts | 📥 Backlog |

## Completed so far

- **`MazeFile` module (#25):** saves a maze as **plain text, one row per line**, using the same characters as the built-in mazes (`#`, `S`, `G`, spaces). Saved mazes can be opened, edited, or even drawn from scratch in Notepad, and loading (#26) will reuse the existing `Maze::loadFromLayout()` validation.
  - `isValidName()` rejects blank, too-long, Windows-reserved (`CON`, `NUL`, `COM1`...) and illegal-character names, with a reason.
  - `withExtension()` adds `.txt` when there's no extension.
  - `save()` creates the `mazes` folder if needed, and reports an error instead of crashing if the file can't be written.
- **`Maze::getLayout()`:** turns the grid back into text rows.
- **"4. Save this maze to a file"** in the What next? menu: asks for a name (0 cancels), asks before replacing an existing file, and shows the full path on success. Afterwards it returns to What next?, since the maze hasn't changed.
- **`InputHelper::readText()`** and `Menu::askText()` for typed text, following the same end-of-input pattern as the other prompts.

## Design decisions

- **Plain text over a custom format:** a maze file is just the maze, so it's readable, editable and easy to check by eye. The trade-off is that a random maze's seed isn't saved, which is fine because the full layout is.
- **A `mazes` folder next to where the program runs:** saved files are easy to find, the program shows the full path after saving, and the folder is in `.gitignore`, so files saved while testing in Visual Studio don't end up in the repository.

## Bugs & Challenges

| Problem | Fix |
|---|---|
| Error text "can't contain :." was confusing | Show the character in quotes: `can't contain ":"` |
| A test run could save a file into whatever folder it ran from | The test scripts now run in their own scratch folder, and the self-test uses a temp folder it deletes afterward |
