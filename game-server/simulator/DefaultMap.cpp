#include "DefaultMap.h"

static std::vector<simVec2> DefaultMapSpawnPoints = {
    {1.5, 4.1},
    {18.5, 4.1},
};

const std::vector<simVec2>& DefaultMap::getSpawnPoints() const {
    return DefaultMapSpawnPoints;
}
