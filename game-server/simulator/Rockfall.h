#pragma once

#include "IEntity.h"

class Rockfall : public IEntity{
public:
    Rockfall(Simulator* simulator, int id, b2World* world, const simVec2& rockfallSize, const simVec2& spawningPoint, int damage, int collisionCode) : IEntity(simulator, id, world), mDamage(damage)  {

        mBody->SetType(b2_dynamicBody);
        mBody->SetLinearDamping(0);
        mBody->SetAngularDamping(0.5);
        mBody->SetTransform(spawningPoint, b2_pi);
        // playerBody->SetGravityScale(3);
        b2PolygonShape rockfallShape;

        b2Vec2 polygon[] = {
            {-rockfallSize.x/2, -rockfallSize.y/2},
            {0, rockfallSize.y/2},
            {rockfallSize.x/2, -rockfallSize.y/2},
        };

        rockfallShape.Set(polygon, 3);
        
        b2FixtureDef fixtureDef;
        fixtureDef.density = 0.3;
        fixtureDef.friction = 0.7;
        fixtureDef.restitution = 0.5;
        fixtureDef.shape = &rockfallShape;
        fixtureDef.filter.categoryBits = getEntityType();
        fixtureDef.filter.maskBits = collisionCode; // EntityType::Map | EntityType::PlayerGunShot | EntityType::Player;

        mBody->CreateFixture(&fixtureDef);
     }

    virtual IEntityType getEntityType() const {
        return EntityType::Rockfall;
    }
    
    int getDamage() {
        return mDamage;
    }
private:
    int mDamage;
};
