#pragma once

#include "IEntity.h"
#include "Player.h"

enum {
    ItemCode_Hp = 1,
};

class IFieldItem : public IEntity{
public:
    IFieldItem(Simulator* simulator, int id, b2World* world) : IEntity(simulator, id, world) {}

    virtual IEntityType getEntityType() const {
        return EntityType::FieldItem;
    }

    virtual int itemCode() = 0;
    virtual void getItem(Player*) = 0;
private:
};
