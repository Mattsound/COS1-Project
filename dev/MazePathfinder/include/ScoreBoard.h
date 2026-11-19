#pragma once

#include "MazeGenerator.h"

#include <filesystem>
#include <map>
#include <string>

// The best (fewest turns) Play mode win on each difficulty, kept in a small
// text file so it survives between runs. The file is one line per difficulty,
// like "Medium=62", so it's easy to read or reset by hand.
class ScoreBoard
{
public:
    explicit ScoreBoard(std::filesystem::path path);

    enum class LoadResult
    {
        Loaded,   // every line was read
        NoFile,   // first run: nothing saved yet
        Repaired  // some lines couldn't be read and were skipped
    };

    // Reads the scores file. A missing file is normal (NoFile). Lines that
    // can't be understood are skipped and listed in problems, so one bad line
    // never throws away the other scores.
    LoadResult load(std::string& problems);

    // Writes every best score. Returns false and fills in error if the file
    // can't be written.
    bool save(std::string& error) const;

    // Fewest turns to win on this difficulty, or 0 if there's no win yet.
    int getBest(Difficulty difficulty) const;

    // Records a win. Returns true if it beat (or set) the best for that
    // difficulty. Turn counts below 1 are ignored.
    bool recordWin(Difficulty difficulty, int turns);

    const std::filesystem::path& getPath() const { return m_path; }

private:
    std::filesystem::path m_path;
    std::map<Difficulty, int> m_best;
};
