#include "MazeFile.h"

#include <cctype>
#include <fstream>
#include <system_error>

namespace
{
    const size_t MaxNameLength = 100;

    std::string toUpper(std::string text)
    {
        for (char& c : text)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        return text;
    }

    // Windows won't create a file with these names, whatever the extension.
    bool isReservedName(const std::string& name)
    {
        std::string base = toUpper(name.substr(0, name.find('.')));
        if (base == "CON" || base == "PRN" || base == "AUX" || base == "NUL")
            return true;

        bool numberedDevice = base.size() == 4 && (base.compare(0, 3, "COM") == 0 || base.compare(0, 3, "LPT") == 0) &&
                              base[3] >= '1' && base[3] <= '9';
        return numberedDevice;
    }
}

std::filesystem::path MazeFile::getFolder()
{
    return std::filesystem::current_path() / "mazes";
}

bool MazeFile::isValidName(const std::string& name, std::string& reason)
{
    if (name.empty())
    {
        reason = "The file name can't be blank.";
        return false;
    }

    if (name.size() > MaxNameLength)
    {
        reason = "That file name is too long. Please use " + std::to_string(MaxNameLength) + " characters or fewer.";
        return false;
    }

    const std::string notAllowed = "<>:\"/\\|?*";
    for (char c : name)
    {
        if (notAllowed.find(c) != std::string::npos || static_cast<unsigned char>(c) < 32)
        {
            reason = (static_cast<unsigned char>(c) < 32 ? std::string("File names can't contain control characters")
                                                          : "File names can't contain \"" + std::string(1, c) + "\"") +
                     ". Please avoid these characters: < > : \" / \\ | ? *";
            return false;
        }
    }

    if (name.back() == '.' || name.back() == ' ')
    {
        reason = "File names can't end with a period or a space.";
        return false;
    }

    if (isReservedName(name))
    {
        reason = "\"" + name + "\" is a name Windows reserves for devices. Please choose another name.";
        return false;
    }

    return true;
}

std::string MazeFile::withExtension(const std::string& name)
{
    return std::filesystem::path(name).has_extension() ? name : name + ".txt";
}

bool MazeFile::save(const std::vector<std::string>& layout, const std::filesystem::path& path, std::string& error)
{
    std::error_code ignored;
    std::filesystem::create_directories(path.parent_path(), ignored); // fails harmlessly if it already exists

    std::ofstream file(path);
    if (!file)
    {
        error = "Couldn't create \"" + path.string() + "\". Check that the folder exists and isn't read-only.";
        return false;
    }

    for (const std::string& row : layout)
        file << row << '\n';

    file.close();
    if (file.fail())
    {
        error = "Something went wrong while writing \"" + path.string() + "\". The file may be incomplete.";
        return false;
    }

    return true;
}
