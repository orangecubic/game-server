#pragma once

#include "IEntity.h"

class Player;

class GunShot : public IEntity{
public:
    GunShot(Simulator* simulator, int id, b2World* world, const simVec2& gunshotSize, Player* shotPlayer, int damage, int collisionCode) : IEntity(simulator, id, world), mOwner(shotPlayer), mDamage(damage) {

        // initial value for right
        float angle = b2_pi*1.5;
        float velocity = 25;
        float anchorPoint = 0.8;
        if (shotPlayer->getCurrentDirection() != PlayerMovementStatus_Right) {
            angle = b2_pi/2;
            velocity = -25;
            anchorPoint = -0.8;
        }
        
        mBody->SetType(b2_dynamicBody);
        mBody->SetLinearDamping(0);
        mBody->SetAngularDamping(0.5);
        mBody->SetGravityScale(0.2);
        b2Vec2 spawningPoint;
        mBody->SetLinearVelocity(b2Vec2(velocity, 0));
        mBody->SetTransform(shotPlayer->getBody()->GetWorldPoint(b2Vec2(anchorPoint, 0)), angle);
        
        b2PolygonShape gunshotShape;

        b2Vec2 polygon[] = {
            {-gunshotSize.x/2, -gunshotSize.y/2},
            {0, gunshotSize.y/2},
            {gunshotSize.x/2, -gunshotSize.y/2},
        };

        gunshotShape.Set(polygon, 3);
        
        b2FixtureDef fixtureDef;
        fixtureDef.density = 1;
        fixtureDef.friction = 0.7;
        fixtureDef.restitution = 0.0;
        fixtureDef.shape = &gunshotShape;
        fixtureDef.filter.categoryBits = getEntityType();
        fixtureDef.filter.maskBits = collisionCode; // EntityType::Map | EntityType::PlayerGunShot | EntityType::Player | EntityType::Rockfall;

        mBody->CreateFixture(&fixtureDef);
     }

    Player* getOwner() {
        return mOwner;
    }
    
    virtual IEntityType getEntityType() const {
        return EntityType::PlayerGunShot;
    }
    
    int getDamage() {
        return mDamage;
    }
private:
    Player* mOwner;
    int mDamage;
};
