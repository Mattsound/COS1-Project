#pragma once

#include <random>
#include <string>
#include <vector>

enum class Difficulty
{
    Easy,
    Medium,
    Hard
};

// Builds random mazes with the recursive backtracker (a randomized depth-first
// search). The result is a "perfect" maze: every open cell can be reached and
// there is exactly one route between any two of them.
class MazeGenerator
{
public:
    struct Size
    {
        int width;
        int height;
    };

    static Size getSize(Difficulty difficulty);
    static std::string getName(Difficulty difficulty);

    // Returns a layout in the same format as the built-in mazes, with the
    // start in the top-left corner and the goal in the bottom-right. The same
    // seed and size always produce the same maze. Even sizes are rounded up to
    // the next odd number and anything smaller than 5 becomes 5, because walls
    // sit between the open "rooms", which are at odd rows and columns.
    std::vector<std::string> generate(int width, int height, unsigned int seed);

    std::vector<std::string> generate(Difficulty difficulty, unsigned int seed);

private:
    std::mt19937 m_random;
};
