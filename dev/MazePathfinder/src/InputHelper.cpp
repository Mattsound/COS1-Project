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

    enum class ParseResult
    {
        Ok,
        NotANumber, // "abc", "3x", "2.5", ""
        TooLarge    // only digits, but too big (or small) to fit in an int
    };

    bool isWholeNumberText(const std::string& text)
    {
        size_t start = (!text.empty() && (text[0] == '+' || text[0] == '-')) ? 1 : 0;
        if (start == text.size())
            return false;
        for (size_t i = start; i < text.size(); ++i)
        {
            if (!std::isdigit(static_cast<unsigned char>(text[i])))
                return false;
        }
        return true;
    }

    // Converts the whole string to an int.
    ParseResult tryParseInt(const std::string& text, int& value)
    {
        if (!isWholeNumberText(text))
            return ParseResult::NotANumber;

        try
        {
            value = std::stoi(text);
            return ParseResult::Ok;
        }
        catch (const std::out_of_range&)
        {
            return ParseResult::TooLarge;
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

        const ParseResult parsed = tryParseInt(text, value);
        if (parsed == ParseResult::NotANumber)
        {
            std::cout << "  \"" << text << "\" isn't a whole number. Please enter a number from " << range << ".\n";
            continue;
        }
        if (parsed == ParseResult::TooLarge)
        {
            std::cout << "  " << text << " is too large. Please enter a number from " << range << ".\n";
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

bool InputHelper::readKey(const std::string& prompt, const std::string& validKeys,
                          const std::string& help, char& result)
{
    std::string line;
    while (true)
    {
        std::cout << prompt << ": ";
        if (!std::getline(std::cin, line))
            return false;

        const std::string text = trim(line);
        if (text.size() == 1)
        {
            char key = static_cast<char>(std::tolower(static_cast<unsigned char>(text[0])));
            if (validKeys.find(key) != std::string::npos)
            {
                result = key;
                return true;
            }
        }

        if (text.empty())
            std::cout << "  " << help << '\n';
        else
            std::cout << "  \"" << text << "\" isn't a valid key. " << help << '\n';
    }
}

bool InputHelper::waitForEnter(const std::string& prompt)
{
    std::cout << '\n' << prompt;
    std::string line;
    return static_cast<bool>(std::getline(std::cin, line));
}
