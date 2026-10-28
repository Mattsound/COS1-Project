#pragma once

// Runs the main menu loop and sends each choice to the right feature.
class Menu
{
public:
    void run();

private:
    void showMainMenu() const;
    int readChoice() const;
    void handleChoice(int choice);

    bool m_running = true;
};
