#pragma once

#include "Enemy.h"
#include "MazeGenerator.h"
#include "Player.h"

#include <string>

class Cell;
class Maze;
class Pathfinder;

enum class GameState
{
    Playing,
    Won,    // the player reached G
    Caught  // the enemy reached the player
};

// The rules of Play mode, with no console input or output, so the self-test
// can play whole games automatically. Each turn the player moves one cell,
// then the enemy takes its turn if it's awake and due.
class Game
{
public:
    struct Settings
    {
        int enemyStepsPerTurn; // cells the enemy moves when it moves
        int enemyMovesEvery;   // 1 = every turn, 2 = every other turn
        int extraHeadStart;    // turns of slack on top of the fair minimum
    };

    // Where the enemy starts and how many turns it sleeps before chasing.
    struct EnemyPlan
    {
        Cell* start = nullptr;
        int headStart = 0;
    };

    static Settings getSettings(Difficulty difficulty);
    static std::string describeEnemy(const Settings& settings);

    Game(Maze& maze, Pathfinder& enemyPathfinder, Settings settings, EnemyPlan plan);

    // Moves the player one cell. A blocked move returns false and uses up no
    // turn, so the enemy doesn't move either. After a real move: reaching G
    // wins (even with the enemy right behind you), stepping onto the enemy
    // loses, otherwise the enemy takes its turn (if awake) and may catch you.
    bool playTurn(Direction direction);

    GameState getState() const { return m_state; }
    int getTurn() const { return m_turn; }
    Cell* getPlayerCell() const { return m_player.getCell(); }
    Cell* getEnemyCell() const { return m_enemy.getCell(); }
    int getHeadStart() const { return m_headStart; }
    bool isEnemyAwake() const { return m_turn >= m_headStart; }
    bool enemyMovesAfterTurn(int turn) const;

    // Random mazes have only one route to G, so an enemy that is always
    // chasing could cut it off and make the game impossible. Instead the enemy
    // starts in its "lair" (the open cell farthest from S) and sleeps for a
    // head start: the smallest number of turns that still lets a player who
    // walks the shortest route escape, plus settings.extraHeadStart.
    // Returns a plan with start == nullptr if the maze has no route to G.
    static EnemyPlan planEnemy(Maze& maze, Settings settings);

    // Generates a random maze from seed and plans the enemy. A few small mazes
    // turn out to be one long corridor with nowhere off the route for the
    // enemy, so this tries seed, seed + 1, ... until one is playable. Returns
    // the seed it used, so the player is shown the maze they actually got.
    static unsigned int generatePlayable(MazeGenerator& generator, Difficulty difficulty, unsigned int seed,
                                         Maze& maze, EnemyPlan& plan);

    // Plays a whole game by walking the shortest route from S to G.
    static GameState playShortestRoute(Maze& maze, Pathfinder& enemyPathfinder, Settings settings, EnemyPlan plan);

private:
    Maze& m_maze;
    Player m_player;
    Enemy m_enemy;
    Settings m_settings;
    int m_headStart;
    int m_turn = 0;
    GameState m_state = GameState::Playing;
};
