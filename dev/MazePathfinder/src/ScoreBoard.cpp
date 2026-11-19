#include "ScoreBoard.h"

#include <fstream>
#include <system_error>
#include <utility>

namespace
{
    const Difficulty AllDifficulties[] = { Difficulty::Easy, Difficulty::Medium, Difficulty::Hard };
    const int MaxTurns = 1000000; // anything bigger can't be a real game

    bool parseDifficulty(const std::string& name, Difficulty& difficulty)
    {
        for (Difficulty d : AllDifficulties)
        {
            if (MazeGenerator::getName(d) == name)
            {
                difficulty = d;
                return true;
            }
        }
        return false;
    }

    // A whole positive number of turns, with nothing else on the line.
    bool parseTurns(const std::string& text, int& turns)
    {
        if (text.empty() || text.size() > 7)
            return false;
        for (char c : text)
        {
            if (c < '0' || c > '9')
                return false;
        }
        turns = std::stoi(text); // safe: at most 7 digits
        return turns >= 1 && turns <= MaxTurns;
    }
}

ScoreBoard::ScoreBoard(std::filesystem::path path)
    : m_path(std::move(path))
{
}

ScoreBoard::LoadResult ScoreBoard::load(std::string& problems)
{
    m_best.clear();
    problems.clear();

    std::error_code ignored;
    if (!std::filesystem::exists(m_path, ignored))
        return LoadResult::NoFile;

    std::ifstream file(m_path);
    if (!file)
    {
        problems = "couldn't open the file";
        return LoadResult::Repaired;
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line))
    {
        ++lineNumber;
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (line.empty())
            continue;

        const size_t equals = line.find('=');
        Difficulty difficulty;
        int turns = 0;
        if (equals == std::string::npos || !parseDifficulty(line.substr(0, equals), difficulty) ||
            !parseTurns(line.substr(equals + 1), turns))
        {
            problems += (problems.empty() ? "" : ", ") + std::string("line ") + std::to_string(lineNumber) +
                        " (\"" + line + "\")";
            continue;
        }

        // If a difficulty appears twice, keep the better score.
        auto existing = m_best.find(difficulty);
        if (existing == m_best.end() || turns < existing->second)
            m_best[difficulty] = turns;
    }

    return problems.empty() ? LoadResult::Loaded : LoadResult::Repaired;
}

bool ScoreBoard::save(std::string& error) const
{
    std::ofstream file(m_path);
    if (!file)
    {
        error = "Couldn't write \"" + m_path.string() + "\".";
        return false;
    }

    for (Difficulty d : AllDifficulties)
    {
        auto entry = m_best.find(d);
        if (entry != m_best.end())
            file << MazeGenerator::getName(d) << '=' << entry->second << '\n';
    }

    file.close();
    if (file.fail())
    {
        error = "Something went wrong while writing \"" + m_path.string() + "\".";
        return false;
    }
    return true;
}

int ScoreBoard::getBest(Difficulty difficulty) const
{
    auto entry = m_best.find(difficulty);
    return entry == m_best.end() ? 0 : entry->second;
}

bool ScoreBoard::recordWin(Difficulty difficulty, int turns)
{
    if (turns < 1)
        return false;

    const int best = getBest(difficulty);
    if (best != 0 && turns >= best)
        return false;

    m_best[difficulty] = turns;
    return true;
}
