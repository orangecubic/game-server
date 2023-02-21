#pragma once

#include "box2d/box2d.h"
#include "Vec2.h"
#include <string>

class Simulator;

using IEntityType = int;


namespace EntityType {
    enum {
        Map = 1,
        Player = 2,
        Rockfall = 4,
        PlayerGunShot = 8,
        FieldItem = 16,
    };
}

class IEntity {
protected:
    const int mId;
    Simulator* mSimulator;
    b2World* const mWorld;
    b2Body* mBody;

    int mHp = 0;
    uint64_t mUserData;
public:
    IEntity(Simulator* simulator, int id, b2World* world) : mSimulator(simulator), mId(id), mWorld(world){
        b2BodyDef def;
        def.userData.pointer = reinterpret_cast<uintptr_t>(this);
        mBody = world->CreateBody(&def);
    }
    virtual ~IEntity() {
        mWorld->DestroyBody(mBody);
    }
    const simVec2& getPosition();
    const simVec2& getLinearVelocity();
    float getAngularVelocity();

    void setPosition(const simVec2& position);
    void setLinearVelocity(const simVec2& velocity);
    void setAngularVelocity(float angle);

    b2Body* getBody();
    int getEntityId() const;
    Simulator* getSimulator();
    void destroyEntity();
    void setUserData(uint64_t data);
    uint64_t getUserData();

    int getHp();

    virtual IEntityType getEntityType() const = 0;
    virtual void simulate(float delta) {}
};
