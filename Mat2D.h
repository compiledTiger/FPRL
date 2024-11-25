#pragma once

#include "Vec2D.h"

struct Mat2D {
    float r0c0;
    float r0c1;
    float r1c0;
    float r1c1;

    Mat2D(float r0c0_, float r0c1_, float r1c0_, float r1c1_)
        : r0c0(r0c0_), r0c1(r0c1_), r1c0(r1c0_), r1c1(r1c1_) {}

    Vec2D operator*(const Vec2D& rhs) {
        return Vec2D(r0c0 * rhs.x + r0c1 * rhs.y, r1c0 * rhs.x + r1c1 * rhs.y);
    }
};
