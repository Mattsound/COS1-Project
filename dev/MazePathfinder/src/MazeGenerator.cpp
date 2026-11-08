#include "MazeGenerator.h"

#include <stack>
#include <utility>

namespace
{
    const char Wall = '#';
    const char Open = ' ';

    int makeOddAndAtLeastFive(int value)
    {
        if (value < 5)
            return 5;
        return (value % 2 == 0) ? value + 1 : value;
    }
}

MazeGenerator::Size MazeGenerator::getSize(Difficulty difficulty)
{
    switch (difficulty)
    {
    case Difficulty::Easy:   return { 11, 7 };
    case Difficulty::Medium: return { 21, 11 };
    default:                 return { 41, 21 };
    }
}

std::string MazeGenerator::getName(Difficulty difficulty)
{
    switch (difficulty)
    {
    case Difficulty::Easy:   return "Easy";
    case Difficulty::Medium: return "Medium";
    default:                 return "Hard";
    }
}

std::vector<std::string> MazeGenerator::generate(Difficulty difficulty, unsigned int seed)
{
    Size size = getSize(difficulty);
    return generate(size.width, size.height, seed);
}

std::vector<std::string> MazeGenerator::generate(int width, int height, unsigned int seed)
{
    width = makeOddAndAtLeastFive(width);
    height = makeOddAndAtLeastFive(height);
    m_random.seed(seed);

    // Start solid. Rooms live at odd (row, col) positions; the cells between
    // two rooms are the walls that get knocked down to connect them.
    std::vector<std::string> layout(height, std::string(width, Wall));

    // Each step moves two cells: from one room to the next.
    const int rowSteps[] = { -2, 2, 0, 0 };
    const int colSteps[] = { 0, 0, -2, 2 };

    std::stack<std::pair<int, int>> path; // rooms we can backtrack to
    layout[1][1] = Open;
    path.push({ 1, 1 });

    while (!path.empty())
    {
        auto [row, col] = path.top();

        // Rooms next to this one that haven't been carved yet.
        std::vector<int> choices;
        for (int i = 0; i < 4; ++i)
        {
            int nextRow = row + rowSteps[i];
            int nextCol = col + colSteps[i];
            bool inside = nextRow > 0 && nextRow < height - 1 && nextCol > 0 && nextCol < width - 1;
            if (inside && layout[nextRow][nextCol] == Wall)
                choices.push_back(i);
        }

        if (choices.empty())
        {
            path.pop(); // dead end: back up and try from an earlier room
            continue;
        }

        std::uniform_int_distribution<size_t> pick(0, choices.size() - 1);
        int direction = choices[pick(m_random)];
        int nextRow = row + rowSteps[direction];
        int nextCol = col + colSteps[direction];

        layout[row + rowSteps[direction] / 2][col + colSteps[direction] / 2] = Open; // the wall between
        layout[nextRow][nextCol] = Open;
        path.push({ nextRow, nextCol });
    }

    layout[1][1] = 'S';
    layout[height - 2][width - 2] = 'G';
    return layout;
}
