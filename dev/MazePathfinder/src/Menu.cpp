#include "Menu.h"

#include "AStarPathfinder.h"
#include "BfsPathfinder.h"
#include "Game.h"
#include "InputHelper.h"
#include "MazePresets.h"
#include "Player.h"

#include <iostream>
#include <iterator>
#include <random>

namespace
{
    enum MenuOption
    {
        SolveMaze = 1,
        PlayMode = 2,
        CompareAlgorithms = 3,
        Exit = 4
    };

    enum NextStep
    {
        BackToMainMenu = 0,
        SolveThisMaze = 1,
        CompareThisMaze = 2,
        NewMaze = 3
    };

    // "1 move", "2 turns"
    std::string countText(int count, const std::string& word)
    {
        return std::to_string(count) + " " + word + (count == 1 ? "" : "s");
    }
}

Menu::Menu()
    : m_view(m_maze, m_pathfinders)
{
    m_pathfinders.push_back(std::make_unique<BfsPathfinder>());
    m_pathfinders.push_back(std::make_unique<AStarPathfinder>());
}

// Each ask...() works like the InputHelper function it wraps, but if input
// ends (Ctrl+Z / end of file) it also stops the menu, so callers only have to
// check the return value instead of repeating that cleanup everywhere.
bool Menu::askInt(const std::string& prompt, int min, int max, int& result)
{
    if (InputHelper::readInt(prompt, min, max, result))
        return true;
    m_running = false;
    return false;
}

bool Menu::askKey(const std::string& prompt, const std::string& validKeys, const std::string& help, char& result)
{
    if (InputHelper::readKey(prompt, validKeys, help, result))
        return true;
    m_running = false;
    return false;
}

bool Menu::askYesNo(const std::string& prompt, bool& result)
{
    if (InputHelper::readYesNo(prompt, result))
        return true;
    m_running = false;
    return false;
}

unsigned int Menu::nextSeed() const
{
    // Keep seeds short so they're easy to read and note down.
    return m_hasFixedSeed ? m_fixedSeed : std::random_device{}() % 100000;
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
              << "     Pick a built-in or random maze and an algorithm, then watch it find the shortest path.\n"
              << "2. Play mode\n"
              << "     Reach G before the enemy catches you. It always knows the shortest way to you.\n"
              << "3. Compare algorithms\n"
              << "     Run BFS and A* on the same maze and see which explores fewer cells.\n"
              << "4. Exit\n";
}

int Menu::readChoice() const
{
    int choice = 0;
    if (!InputHelper::readInt("Choose an option", SolveMaze, Exit, choice))
        return Exit; // input ended, so leave instead of looping forever

    return choice;
}

void Menu::handleChoice(int choice)
{
    switch (choice)
    {
    case SolveMaze:
        runMazeSession(Mode::Solve);
        break;
    case PlayMode:
        playMaze();
        break;
    case CompareAlgorithms:
        runMazeSession(Mode::Compare);
        break;
    case Exit:
        m_running = false;
        break;
    }
}

void Menu::playMaze()
{
    Difficulty difficulty = Difficulty::Medium;
    if (!choosePlayDifficulty(difficulty))
        return;

    while (m_running)
    {
        playOneGame(difficulty);
        if (!m_running)
            return;

        std::cout << "\nWhat next?\n"
                  << "1. Play again (new " << MazeGenerator::getName(difficulty) << " maze)\n"
                  << "2. Change difficulty\n"
                  << "0. Back to main menu\n";

        int choice = 0;
        if (!askInt("Choose an option", 0, 2, choice))
            return;

        if (choice == 0)
            return;
        if (choice == 2 && !choosePlayDifficulty(difficulty))
            return;
    }
}

bool Menu::choosePlayDifficulty(Difficulty& difficulty)
{
    const Difficulty difficulties[] = { Difficulty::Easy, Difficulty::Medium, Difficulty::Hard };
    const int count = static_cast<int>(std::size(difficulties));

    std::cout << "\nChoose a difficulty:\n";
    for (int i = 0; i < count; ++i)
    {
        MazeGenerator::Size size = MazeGenerator::getSize(difficulties[i]);
        std::cout << i + 1 << ". " << MazeGenerator::getName(difficulties[i])
                  << " (" << size.width << " x " << size.height << ", enemy "
                  << Game::describeEnemy(Game::getSettings(difficulties[i])) << ")\n";
    }
    std::cout << "0. Back to main menu\n";

    int choice = 0;
    if (!askInt("Choose a difficulty", 0, count, choice))
        return false;
    if (choice == 0)
        return false;

    difficulty = difficulties[choice - 1];
    return true;
}

void Menu::playOneGame(Difficulty difficulty)
{
    const Game::Settings settings = Game::getSettings(difficulty);
    Game::EnemyPlan plan;
    const unsigned int seed = Game::generatePlayable(m_generator, difficulty,
                                                     nextSeed(), m_maze, plan);
    if (plan.start == nullptr)
    {
        std::cout << "\nCouldn't build a playable maze this time. Please try again.\n";
        return;
    }

    AStarPathfinder enemyBrain;
    Game game(m_maze, enemyBrain, settings, plan);
    BfsPathfinder ruler; // measures how far away the enemy is
    const int shortestRoute = ruler.findPath(m_maze).getPathLength();

    std::cout << "\nMaze #" << seed << " (" << MazeGenerator::getName(difficulty) << ", "
              << m_maze.getCols() << " x " << m_maze.getRows() << ")\n"
              << "You are P. Reach G before the enemy (E) catches you!\n"
              << "E is asleep for " << countText(game.getHeadStart(), "turn") << ", then it "
              << Game::describeEnemy(settings) << ".\n"
              << "It always knows the shortest way to you, so watch out for dead ends.\n";

    const std::string help = "Use W (up), A (left), S (down), D (right), or Q to quit.";
    bool redraw = true;
    bool wasAwake = game.isEnemyAwake();

    while (game.getState() == GameState::Playing)
    {
        if (redraw)
        {
            std::cout << '\n';
            m_maze.renderWithMarkers({ { game.getEnemyCell(), 'E' }, { game.getPlayerCell(), 'P' } });

            std::cout << "Turn " << game.getTurn() << " | ";
            if (!game.isEnemyAwake())
                std::cout << "E wakes up in " << countText(game.getHeadStart() - game.getTurn(), "turn") << '\n';
            else
                std::cout << "E is " << countText(ruler.findPathBetween(m_maze, game.getEnemyCell(), game.getPlayerCell()).getPathLength(), "move")
                          << " away\n";
        }

        char key = 0;
        if (!askKey("Your move (W/A/S/D, Q to quit)", "wasdq", help, key))
            return;

        if (key == 'q')
        {
            std::cout << "\nYou gave up on turn " << game.getTurn() << ".\n";
            return;
        }

        Direction direction = Direction::Up;
        Player::parseDirection(key, direction);
        if (!game.playTurn(direction))
        {
            // Don't redraw the whole maze just to say no.
            std::cout << "  That way is blocked by a wall. Try another direction.\n";
            redraw = false;
            continue;
        }
        redraw = true;

        if (!wasAwake && game.isEnemyAwake() && game.getState() == GameState::Playing)
            std::cout << "\n*** E woke up! ***\n";
        wasAwake = game.isEnemyAwake();
    }

    std::cout << '\n';
    if (game.getState() == GameState::Won)
    {
        m_maze.renderWithMarkers({ { game.getEnemyCell(), 'E' }, { game.getPlayerCell(), 'P' } });
        std::cout << "\nYou escaped in " << countText(game.getTurn(), "turn") << "! "
                  << "The shortest route was " << countText(shortestRoute, "move") << ".\n";
        if (game.getTurn() == shortestRoute)
            std::cout << "Perfect run: you matched the pathfinder!\n";
    }
    else
    {
        m_maze.renderWithMarkers({ { game.getPlayerCell(), 'X' } });
        std::cout << "\nCaught on turn " << game.getTurn() << "! (X marks the spot.)\n"
                  << "Tip: the enemy always takes the shortest way to you, so a wrong turn into a dead end costs you.\n";
    }
}

void Menu::runMazeSession(Mode mode)
{
    if (!chooseMaze())
        return;

    while (m_running)
    {
        if (mode == Mode::Solve)
        {
            if (!solveCurrentMaze())
                return; // went back from the algorithm picker
        }
        else
        {
            compareCurrentMaze();
        }

        if (!m_running)
            return;

        // The results stay on screen while the user decides what to do next.
        switch (askWhatNext())
        {
        case SolveThisMaze:
            mode = Mode::Solve;
            break;
        case CompareThisMaze:
            mode = Mode::Compare;
            break;
        case NewMaze:
            if (!chooseMaze())
                return;
            break;
        default:
            return; // back to the main menu
        }
    }
}

int Menu::askWhatNext()
{
    std::cout << "\nWhat next?\n"
              << SolveThisMaze << ". Solve this maze with an algorithm\n"
              << CompareThisMaze << ". Compare algorithms on this maze\n"
              << NewMaze << ". Pick a new maze\n"
              << BackToMainMenu << ". Back to main menu\n";

    int choice = BackToMainMenu;
    if (!askInt("Choose an option", BackToMainMenu, NewMaze, choice))
        return BackToMainMenu;
    return choice;
}

bool Menu::solveCurrentMaze()
{
    Pathfinder* pathfinder = choosePathfinder();
    if (pathfinder == nullptr)
        return false;

    std::cout << "\nHere's the maze:\n\n";
    m_maze.render();

    PathResult result = pathfinder->findPath(m_maze);
    m_view.showSolve(*pathfinder, result);

    if (askToShowExplored())
        m_view.showSolveExplored(*pathfinder, result);

    return true;
}

void Menu::compareCurrentMaze()
{
    std::vector<PathResult> results;
    for (const auto& pathfinder : m_pathfinders)
        results.push_back(pathfinder->findPath(m_maze));

    m_view.showComparison(results);

    if (askToShowExplored())
        m_view.showComparisonExplored(results);
}

bool Menu::askToShowExplored()
{
    bool show = false;
    return askYesNo("\nShow every cell the search explored?", show) && show;
}

bool Menu::chooseMaze()
{
    const auto& presets = MazePresets::getAll();
    const int count = static_cast<int>(presets.size());

    const int randomOption = count + 1;

    std::cout << "\nChoose a maze:\n";
    for (int i = 0; i < count; ++i)
        std::cout << i + 1 << ". " << presets[i].name << '\n';
    std::cout << randomOption << ". Random maze (choose a difficulty)\n"
              << "0. Back to main menu\n";

    int choice = 0;
    if (!askInt("Choose a maze", 0, randomOption, choice))
        return false;

    if (choice == 0)
        return false;

    if (choice == randomOption)
        return chooseRandomMaze();

    return loadLayout(presets[choice - 1].layout);
}

bool Menu::chooseRandomMaze()
{
    const Difficulty difficulties[] = { Difficulty::Easy, Difficulty::Medium, Difficulty::Hard };
    const int count = static_cast<int>(std::size(difficulties));

    std::cout << "\nChoose a difficulty:\n";
    for (int i = 0; i < count; ++i)
    {
        MazeGenerator::Size size = MazeGenerator::getSize(difficulties[i]);
        std::cout << i + 1 << ". " << MazeGenerator::getName(difficulties[i])
                  << " (" << size.width << " x " << size.height << ")\n";
    }
    std::cout << "0. Back to main menu\n";

    int choice = 0;
    if (!askInt("Choose a difficulty", 0, count, choice))
        return false;

    if (choice == 0)
        return false;

    const unsigned int seed = nextSeed();
    const Difficulty difficulty = difficulties[choice - 1];
    if (!loadLayout(m_generator.generate(difficulty, seed)))
        return false;

    std::cout << "\nGenerated random maze #" << seed << " (" << MazeGenerator::getName(difficulty)
              << ", " << m_maze.getCols() << " x " << m_maze.getRows() << ")\n";
    return true;
}

bool Menu::loadLayout(const std::vector<std::string>& layout)
{
    std::string error;
    if (!m_maze.loadFromLayout(layout, error))
    {
        std::cout << "Could not load that maze: " << error << '\n';
        return false;
    }
    return true;
}

Pathfinder* Menu::choosePathfinder()
{
    const int count = static_cast<int>(m_pathfinders.size());

    std::cout << "\nChoose an algorithm:\n";
    for (int i = 0; i < count; ++i)
        std::cout << i + 1 << ". " << m_pathfinders[i]->getName() << '\n';
    std::cout << "0. Back to main menu\n";

    int choice = 0;
    if (!askInt("Choose an algorithm", 0, count, choice))
        return nullptr;

    if (choice == 0)
        return nullptr;

    return m_pathfinders[choice - 1].get();
}

