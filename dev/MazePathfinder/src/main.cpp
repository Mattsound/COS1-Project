#include "Menu.h"
#include "SelfTest.h"

#include <string>

int main(int argc, char* argv[])
{
    // "MazePathfinder.exe --selftest" runs the automated checks instead of the menu.
    if (argc > 1 && std::string(argv[1]) == "--selftest")
        return SelfTest::runAll() ? 0 : 1;

    Menu menu;
    menu.run();
    return 0;
}
