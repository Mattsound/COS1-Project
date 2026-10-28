#pragma once

#include <string>

// Reusable, validated console input. Reads whole lines so bad input can never
// leave std::cin stuck in a failed state.
namespace InputHelper
{
    // Keeps asking until the user enters a whole number between min and max.
    // Returns false only if input has ended (Ctrl+Z / end of file).
    bool readInt(const std::string& prompt, int min, int max, int& result);
}
