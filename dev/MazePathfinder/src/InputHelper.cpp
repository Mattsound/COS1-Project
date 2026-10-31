#include "InputHelper.h"

#include <iostream>
#include <stdexcept>

namespace
{
    std::string trim(const std::string& text)
    {
        const char* whitespace = " \t\r\n";
        size_t first = text.find_first_not_of(whitespace);
        if (first == std::string::npos)
            return "";

        size_t last = text.find_last_not_of(whitespace);
        return text.substr(first, last - first + 1);
    }

    // Converts the whole string to an int. Rejects "abc", "3x", "", and
    // numbers too large to fit in an int.
    bool tryParseInt(const std::string& text, int& value)
    {
        if (text.empty())
            return false;

        try
        {
            size_t charsUsed = 0;
            value = std::stoi(text, &charsUsed);
            return charsUsed == text.size();
        }
        catch (const std::exception&)
        {
            return false;
        }
    }
}

bool InputHelper::readInt(const std::string& prompt, int min, int max, int& result)
{
    std::string line;
    while (true)
    {
        std::cout << prompt;
        if (!std::getline(std::cin, line))
            return false;

        int value = 0;
        if (!tryParseInt(trim(line), value))
        {
            std::cout << "Please enter a whole number.\n";
            continue;
        }

        if (value < min || value > max)
        {
            std::cout << "Please enter a number from " << min << " to " << max << ".\n";
            continue;
        }

        result = value;
        return true;
    }
}
