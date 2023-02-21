#pragma once

#include "IMap.h"
#include <vector>

class DefaultMap : public IMap {
public:
    DefaultMap(Simulator* simulator, int id, b2World* world, int collisionCode) : IMap(simulator, id, world) {

        // mBody->SetType(b2_dynamicBody);
        mBody->SetFixedRotation(true);
        // mBody->SetGravityScale(0);
        
        b2Vec2 groundVertics[] = {
            {0, 5},
            {0, 10.0f},
            {20, 10.0f},
            {20, 1.5f},
            {16, 1.5f},
            {16, 1.0f},
            {12, 1.0f},
            {12, 0.5f},
            {8, 0.5f},
            {8, 1.0f},
            {4, 1.0f},
            {4, 1.5f},
            {0, 1.5f}
        };
        
        b2FixtureDef mapFixtureDef;
        mapFixtureDef.density = 0;
        mapFixtureDef.filter.categoryBits = EntityType::Map;
        mapFixtureDef.filter.maskBits = collisionCode;
        

        b2ChainShape groundShape;
        groundShape.CreateLoop(groundVertics, 13);
        
        b2PolygonShape topShape;
        topShape.SetAsBox(3, 0.25f, b2Vec2(10, 5), 0);
        
        mapFixtureDef.shape = &groundShape;
        mBody->CreateFixture(&mapFixtureDef);

        mapFixtureDef.shape = &topShape;
        mBody->CreateFixture(&mapFixtureDef);
        
        
    }

    virtual const std::vector<simVec2>& getSpawnPoints() const override;
private:

};
