#include "InputHelper.h"

#include <cctype>
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
    const std::string range = std::to_string(min) + "-" + std::to_string(max);

    std::string line;
    while (true)
    {
        std::cout << prompt << " (" << range << "): ";
        if (!std::getline(std::cin, line))
            return false;

        const std::string text = trim(line);
        int value = 0;
        if (text.empty())
        {
            std::cout << "  Type a number from " << range << ", then press Enter.\n";
            continue;
        }

        if (!tryParseInt(text, value))
        {
            std::cout << "  \"" << text << "\" isn't a whole number. Please enter a number from " << range << ".\n";
            continue;
        }

        if (value < min || value > max)
        {
            std::cout << "  " << value << " isn't an option. Please enter a number from " << range << ".\n";
            continue;
        }

        result = value;
        return true;
    }
}

bool InputHelper::readYesNo(const std::string& prompt, bool& result)
{
    std::string line;
    while (true)
    {
        std::cout << prompt << " (y/n): ";
        if (!std::getline(std::cin, line))
            return false;

        std::string answer = trim(line);
        for (char& c : answer)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        if (answer == "y" || answer == "yes")
        {
            result = true;
            return true;
        }
        if (answer == "n" || answer == "no")
        {
            result = false;
            return true;
        }

        if (answer.empty())
            std::cout << "  Type y for yes or n for no, then press Enter.\n";
        else
            std::cout << "  \"" << trim(line) << "\" isn't a yes or no answer. Please type y or n.\n";
    }
}

bool InputHelper::waitForEnter(const std::string& prompt)
{
    std::cout << '\n' << prompt;
    std::string line;
    return static_cast<bool>(std::getline(std::cin, line));
}
