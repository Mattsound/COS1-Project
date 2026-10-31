#include "Menu.h"

#include "InputHelper.h"

#include <iostream>

namespace
{
    enum MenuOption
    {
        SolveMaze = 1,
        PlayMode = 2,
        CompareAlgorithms = 3,
        Exit = 4
    };
}

void Menu::run()
{
    while (m_running)
    {
        showMainMenu();
        handleChoice(readChoice());
    }

    std::cout << "\nThanks for playing. Goodbye!\n";
}

void Menu::showMainMenu() const
{
    std::cout << "\n==============================\n"
              << "        MAZE PATHFINDER\n"
              << "==============================\n"
              << "1. Solve a maze\n"
              << "2. Play mode\n"
              << "3. Compare algorithms\n"
              << "4. Exit\n";
}

int Menu::readChoice() const
{
    int choice = 0;
    if (!InputHelper::readInt("Choose an option: ", SolveMaze, Exit, choice))
        return Exit; // input ended, so leave instead of looping forever

    return choice;
}

void Menu::handleChoice(int choice)
{
    switch (choice)
    {
    case SolveMaze:
        std::cout << "\n[Solve a maze] Coming soon: BFS and A* solving.\n";
        break;
    case PlayMode:
        std::cout << "\n[Play mode] Coming soon: race the AI to the goal.\n";
        break;
    case CompareAlgorithms:
        std::cout << "\n[Compare algorithms] Coming soon: BFS vs. A* stats.\n";
        break;
    case Exit:
        m_running = false;
        break;
    }
}
