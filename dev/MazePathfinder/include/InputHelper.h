#pragma once

#include <string>

// Reusable, validated console input. Reads whole lines so bad input can never
// leave std::cin stuck in a failed state.
namespace InputHelper
{
    // Keeps asking until the user enters a whole number between min and max.
    // The valid range is added to the prompt, e.g. "Choose an option (1-4): ".
    // Returns false only if input has ended (Ctrl+Z / end of file).
    bool readInt(const std::string& prompt, int min, int max, int& result);

    // Waits for the user to press Enter so results stay on screen until
    // they're done reading. Returns false if input has ended.
    bool waitForEnter(const std::string& prompt = "Press Enter to return to the main menu...");
}
