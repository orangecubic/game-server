#pragma once

#include "Vec2.h"

struct SimulatorSetting {
    simVec2 gunshotPower = {1,2};
    simVec2 playerMovementForceArial = {1,2};
    simVec2 playerMovementForceGround = {1,2};
    simVec2 playerJumpVelocity = {0, 2};
    simVec2 rockfallInitialVelocity = {1,1};
    int playerInitialHp = 3;
    
};
