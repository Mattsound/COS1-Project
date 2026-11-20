#include "MazeFileMenu.h"

#include "InputHelper.h"
#include "Maze.h"
#include "MazeFile.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

MazeFileMenu::MazeFileMenu(Maze& maze)
    : m_maze(maze)
{
}

bool MazeFileMenu::saveMaze()
{
    const std::filesystem::path folder = MazeFile::getFolder();
    std::cout << "\nSave this maze as a text file in " << folder.string() << "\n"
              << "You can open it in Notepad, and load it again later from the maze picker.\n";

    while (true)
    {
        std::string name;
        if (!InputHelper::readText("File name (or 0 to cancel)", name))
            return false;
        if (name == "0")
        {
            std::cout << "  Not saved.\n";
            return true;
        }

        std::string reason;
        if (!MazeFile::isValidName(name, reason))
        {
            std::cout << "  " << reason << '\n';
            continue;
        }

        const std::string fileName = MazeFile::withExtension(name);
        const std::filesystem::path path = folder / fileName;

        std::error_code ignored;
        if (std::filesystem::exists(path, ignored))
        {
            bool replace = false;
            if (!InputHelper::readYesNo("  \"" + fileName + "\" already exists. Replace it?", replace))
                return false;
            if (!replace)
                continue; // ask for a different name
        }

        std::string error;
        if (MazeFile::save(m_maze.getLayout(), path, error))
            std::cout << "  Saved to " << path.string() << '\n';
        else
            std::cout << "  " << error << '\n';
        return true;
    }
}

MazeFileMenu::LoadResult MazeFileMenu::chooseAndLoad()
{
    const std::filesystem::path folder = MazeFile::getFolder();
    const std::vector<std::filesystem::path> files = MazeFile::listMazes(folder);
    if (files.empty())
    {
        std::cout << "\nThere are no saved mazes in " << folder.string() << " yet.\n"
                  << "Save one from the What next? menu, or put a maze .txt file in that folder.\n";
        return LoadResult::NotLoaded;
    }

    std::cout << "\nSaved mazes in " << folder.string() << ":\n";
    const int count = static_cast<int>(files.size());
    for (int i = 0; i < count; ++i)
        std::cout << i + 1 << ". " << files[i].filename().string() << '\n';
    std::cout << "0. Back to the maze list\n";

    int choice = 0;
    if (!InputHelper::readInt("Choose a file", 0, count, choice))
        return LoadResult::InputEnded;
    if (choice == 0)
        return LoadResult::NotLoaded;

    const std::filesystem::path& path = files[choice - 1];
    const std::string name = path.filename().string();

    std::vector<std::string> layout;
    std::string error;
    if (!MazeFile::load(path, layout, error))
    {
        std::cout << "  Couldn't load that file: " << error << '\n';
        return LoadResult::NotLoaded;
    }

    if (!m_maze.loadFromLayout(layout, error))
    {
        std::cout << "  \"" << name << "\" isn't a valid maze: " << error << '\n';
        return LoadResult::NotLoaded;
    }

    std::cout << "\nLoaded \"" << name << "\" (" << m_maze.getCols() << " x " << m_maze.getRows() << ")\n";
    return LoadResult::Loaded;
}
