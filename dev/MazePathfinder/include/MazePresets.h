#pragma once

#include <string>
#include <vector>

// Built-in mazes the user can pick from. Random generation and loading mazes
// from files come in later milestones.
namespace MazePresets
{
    struct Preset
    {
        std::string name;
        std::vector<std::string> layout;
    };

    const std::vector<Preset>& getAll();
}
