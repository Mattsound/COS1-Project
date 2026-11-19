#pragma once

#include <string>
#include <vector>

// Built-in mazes the user can pick from. Random mazes come from
// MazeGenerator, and saved mazes are loaded with MazeFile.
namespace MazePresets
{
    struct Preset
    {
        std::string name;
        std::vector<std::string> layout;
    };

    const std::vector<Preset>& getAll();
}
