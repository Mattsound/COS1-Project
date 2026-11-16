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

    // The biggest maze a file may hold, so a huge file can't flood the console.
    const int MaxRows = 101;
    const int MaxCols = 201;

    // Reads a maze file into layout, one row per line. Handles Windows line
    // endings and the invisible marker Notepad can put at the start of a
    // file. Blank lines at the end are ignored. Returns false and fills in
    // error if the file is missing, can't be read, is empty, or is too big.
    // (Whether the rows make a valid maze is checked by Maze::loadFromLayout.)
    bool load(const std::filesystem::path& path, std::vector<std::string>& layout, std::string& error);

    // Every .txt file in folder, sorted by name. Empty if the folder doesn't exist.
    std::vector<std::filesystem::path> listMazes(const std::filesystem::path& folder);
}
