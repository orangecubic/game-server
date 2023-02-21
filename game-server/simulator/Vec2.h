#pragma once

#include "box2d/box2d.h"

using simVec2 = b2Vec2;
/*
struct Vec2 : public b2Vec2 {
	Vec2() {}
    Vec2(const b2Vec2& vec) : b2Vec2(vec) {}
	Vec2(float xIn, float yIn) : b2Vec2(xIn, yIn) {}

    operator b2Vec2() {
        return b2Vec2(x, y);
    }
};
*/
