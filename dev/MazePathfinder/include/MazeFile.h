#pragma once

#include <filesystem>
#include <string>
#include <vector>

// Saving and loading mazes as plain text files: one row per line, using the
// same characters as the built-in mazes ('#' wall, 'S' start, 'G' goal, ' '
// open), so a saved maze can be opened, edited, or even drawn by hand in
// Notepad.
namespace MazeFile
{
    // The "mazes" folder in the folder the program runs from.
    std::filesystem::path getFolder();

    // Checks a file name the user typed. Returns false and fills in reason if
    // it's blank, too long, uses a character Windows doesn't allow in file
    // names, or is a name Windows reserves (like CON or NUL).
    bool isValidName(const std::string& name, std::string& reason);

    // Adds ".txt" if the name doesn't already have an extension.
    std::string withExtension(const std::string& name);

    // Writes the layout to path, one row per line, creating the folder if
    // needed. Returns false and fills in error if the file can't be written.
    bool save(const std::vector<std::string>& layout, const std::filesystem::path& path, std::string& error);
}
