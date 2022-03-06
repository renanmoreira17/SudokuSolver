#ifndef __GAMES_H__
#define __GAMES_H__

#include <string>

enum class GameDifficulty
{
    Any = 0,
    Simple = 1,
    Easy = 2,
    Intermediate = 3,
    Expert = 4
};

const std::string& getGameOfDifficulty(GameDifficulty difficulty);

#endif // __GAMES_H__