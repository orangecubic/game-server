#include "Player.h"
#include "Simulator.h"

void Player::simulate(float delta) {
    auto velocity = mBody->GetLinearVelocity();
    float movementForce = mMovementForce;
    /*
    
    if (mMovementStatus == PlayerMovementStatus_Left) {
        if (velocity.x > movementForce*-0.66) {
            mBody->ApplyForceToCenter(b2Vec2(-movementForce, 0), true);
        }
    } else if (mMovementStatus == PlayerMovementStatus_Right) {
        if (velocity.x < movementForce*0.66) {
            mBody->ApplyForceToCenter(b2Vec2(movementForce, 0), true);
        }
    } else {
        mBody->SetLinearVelocity(b2Vec2(velocity.x/3, velocity.y));
    }
*/
    if (mMovementStatus == PlayerMovementStatus_Left) {
        
        mBody->SetLinearVelocity(b2Vec2(-movementForce*0.66, getLinearVelocity().y));
        
    } else if (mMovementStatus == PlayerMovementStatus_Right) {
        mBody->SetLinearVelocity(b2Vec2(movementForce*0.66, getLinearVelocity().y));
    } else {
        mBody->SetLinearVelocity(b2Vec2(0, velocity.y));
    }
    
    if (abs(velocity.y) > 0.1f && abs(mLastYVelocity) > 0.1f) {
        mPositionStatus = PlayerPositionStatus_Airial;
    } else {
        mPositionStatus = PlayerPositionStatus_Ground;
    }

    mLastYVelocity = velocity.y;
}

bool Player::jump(const simVec2& velocity) {
    if (mPositionStatus != PlayerPositionStatus_Ground) {
        return false;
    }
    mBody->SetLinearVelocity(velocity);
    mPositionStatus = PlayerPositionStatus_Airial;

    return true;
}

void Player::setMovementStatus(int status) {
    if (status != PlayerMovementStatus_Stop) {
        mLookingDirection = status;
    }
    
    mMovementStatus = status;
    
}

int Player::getCurrentDirection() {
    return mLookingDirection;
}

int Player::getMovementStatus() {
    return mMovementStatus;
}

int Player::getPositionStatus() {
    return mPositionStatus;
}

void Player::increaseHp(int hp) {
    mHp += hp;
}

void Player::decreaseHp(int hp) {
    mHp -= hp;
}

void Player::setMovementForce(int force) {
    mMovementForce = force;
}
