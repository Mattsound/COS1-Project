#pragma once

class Maze;

// The Save and Load screens. Menu decides *when* to show them; this class
// asks for file names, lists the saved mazes and explains any problem.
// The file reading and writing itself is done by MazeFile.
class MazeFileMenu
{
public:
    enum class LoadResult
    {
        Loaded,     // a saved maze is now in the maze
        NotLoaded,  // the user went back, or the file couldn't be used
        InputEnded  // input ended (Ctrl+Z / end of file), so the program should stop
    };

    // Keeps a reference, so the maze must outlive this object (both are
    // members of Menu, and the maze is declared first).
    explicit MazeFileMenu(Maze& maze);

    // Asks for a file name and saves the maze to the mazes folder, asking
    // before replacing a file that already exists. Returns false only if
    // input ended.
    bool saveMaze();

    // Lists the saved maze files and loads the one the user picks into the
    // maze. When it returns NotLoaded, it has already explained why.
    LoadResult chooseAndLoad();

private:
    Maze& m_maze;
};
