#pragma once

#include "IEntity.h"
#include "Vec2.h"

enum {
    PlayerPositionStatus_Ground,
    PlayerPositionStatus_Airial,
};

enum {
    PlayerMovementStatus_Left = 0,
    PlayerMovementStatus_Right,
    PlayerMovementStatus_Stop,
};

enum {
    PlayerDebuff_Stun = 1,
    PlayerDebuff_Neutralized = 2,
};

class Player : public IEntity{
public:
    Player(Simulator* simulator, int id, b2World* world, int hp, int movementForce, const simVec2& playerSize, const simVec2& spawningPoint, int collisionCode) : IEntity(simulator, id, world) {

        mHp = hp;
        mMovementForce = movementForce;
        mBody->SetFixedRotation(true);
        mBody->SetType(b2_dynamicBody);
        mBody->SetLinearDamping(0);
        mBody->SetAngularDamping(0.5);
        mBody->SetTransform(spawningPoint, 0);
        mBody->SetGravityScale(4);
        // playerBody->SetGravityScale(3);
        b2PolygonShape playerShape;
        playerShape.SetAsBox(playerSize.x, playerSize.y);
        
        b2FixtureDef fixtureDef;
        fixtureDef.density = 0.1;
        fixtureDef.friction = 0.0;
        fixtureDef.restitution = 0.0;
        fixtureDef.shape = &playerShape;
        fixtureDef.filter.categoryBits = EntityType::Player;
        fixtureDef.filter.maskBits = collisionCode; // EntityType::Map | EntityType::FieldItem | EntityType::PlayerGunShot | EntityType::Rockfall;
        mBody->CreateFixture(&fixtureDef);
    }

    virtual IEntityType getEntityType() const override {
        return EntityType::Player;
    }

    virtual void simulate(float delta) override;

    bool jump(const simVec2&);

    void setMovementStatus(int);
    
    int getCurrentDirection();
    
    int getMovementStatus();
    int getPositionStatus();

    void increaseHp(int hp);
    void decreaseHp(int hp);

    void setMovementForce(int force);
    
    void setLastShotTime();
    void setLastPingTime();

    int64_t getLastPingTime();

    int64_t getLastShotTime();
private:

    int mPositionStatus = PlayerPositionStatus_Ground;
    int mMovementStatus = PlayerMovementStatus_Stop;
    int mLookingDirection = PlayerMovementStatus_Left;

    int mLastYVelocity;
    
    int mMovementForce;

    int64_t mLastShotTime;
    int64_t mLastPingTime;
};
