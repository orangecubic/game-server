#include "IEntity.h"
#include "Simulator.h"

const simVec2& IEntity::getPosition() {
    return mBody->GetPosition();
}

const simVec2& IEntity::getLinearVelocity() {
    return mBody->GetLinearVelocity();
}

float IEntity::getAngularVelocity() {
    return mBody->GetAngularVelocity();
}


void IEntity::setPosition(const simVec2& position) {
    this->mBody->SetTransform(position, this->mBody->GetAngle());
}
void IEntity::setLinearVelocity(const simVec2& velocity) {
    this->mBody->SetLinearVelocity(velocity);
}
void IEntity::setAngularVelocity(float angle) {
    this->mBody->SetAngularVelocity(angle);
}


b2Body* IEntity::getBody() {
    return mBody;
}

int IEntity::getEntityId() const {
    return mId;
}

Simulator* IEntity::getSimulator() {
    return mSimulator;
}

void IEntity::destroyEntity() {
    mSimulator->removeEntity(mId);
}

void IEntity::setUserData(uint64_t data) {
    mUserData = data;
}

uint64_t IEntity::getUserData() {
    return mUserData;
}

int IEntity::getHp() {
    return mHp;
}
