#pragma once

#include "IEntity.h"
#include "Vec2.h"
#include <vector>

class IMap : public IEntity {
public:
    IMap(Simulator* simulator, int id, b2World* world) : IEntity(simulator, id, world) {
        
    }

    virtual IEntityType getEntityType() const {
        return EntityType::Map;
    }

    virtual const std::vector<simVec2>& getSpawnPoints() const = 0;
private:

};
