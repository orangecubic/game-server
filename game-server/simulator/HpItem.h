#pragma once

#include "IFieldItem.h"
#include "Player.h"

class HpItem : public IFieldItem {
public:
    HpItem(Simulator* simulator, int id, b2World* world, const simVec2& spawningPoint, const simVec2& itemSize, int hpValue, int collisionCode) : IFieldItem(simulator, id, world) {
        mHpValue = hpValue;
        mBody->SetFixedRotation(true);
        mBody->SetType(b2_staticBody);
        mBody->SetLinearDamping(0);
        mBody->SetAngularDamping(0.5);
        mBody->SetTransform(spawningPoint, 0);
        // playerBody->SetGravityScale(3);
        b2PolygonShape playerShape;
        playerShape.SetAsBox(itemSize.x, itemSize.y);
        
        b2FixtureDef fixtureDef;
        fixtureDef.density = 0.3;
        fixtureDef.friction = 0.7;
        fixtureDef.restitution = 0.0;
        fixtureDef.shape = &playerShape;
        fixtureDef.filter.categoryBits = getEntityType();
        fixtureDef.filter.maskBits = collisionCode;
        mBody->CreateFixture(&fixtureDef);
    }

    int getHpAmount() {
        return 1;
    }

    virtual int itemCode() {
        return ItemCode_Hp;
    }

    virtual void getItem(Player* player) {
        player->increaseHp(getHpAmount());
    }

private:
    int mHpValue;
};
