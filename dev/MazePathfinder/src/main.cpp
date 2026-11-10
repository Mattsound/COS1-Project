#include "Menu.h"
#include "SelfTest.h"

#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    // "MazePathfinder.exe --selftest" runs the automated checks instead of the menu.
    if (argc > 1 && std::string(argv[1]) == "--selftest")
        return SelfTest::runAll() ? 0 : 1;

    Menu menu;

    // "MazePathfinder.exe --seed 12345" makes every random maze use that seed,
    // so a maze can be shared or replayed.
    if (argc > 2 && std::string(argv[1]) == "--seed")
    {
        try
        {
            size_t used = 0;
            unsigned long seed = std::stoul(argv[2], &used);
            if (used != std::string(argv[2]).size())
                throw std::invalid_argument("extra characters");
            menu.setFixedSeed(static_cast<unsigned int>(seed));
        }
        catch (const std::exception&)
        {
            std::cout << "Ignoring --seed: \"" << argv[2] << "\" isn't a whole number. Using random mazes.\n";
        }
    }

    menu.run();
    return 0;
}
