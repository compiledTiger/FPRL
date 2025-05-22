#pragma once

#include <math.h>

#include "Vec2D.h"

struct Mat2D {
    double r0c0;
    double r0c1;
    double r1c0;
    double r1c1;

    Mat2D(double r0c0_, double r0c1_, double r1c0_, double r1c1_)
        : r0c0(r0c0_), r0c1(r0c1_), r1c0(r1c0_), r1c1(r1c1_) {}

    Vec2D operator*(const Vec2D& rhs) {
        return Vec2D(r0c0 * rhs.x + r0c1 * rhs.y, r1c0 * rhs.x + r1c1 * rhs.y);
    }
};

inline Mat2D rotationM(double rad) {
    return Mat2D(std::cos(rad), -std::sin(rad), std::sin(rad), std::cos(rad));
}
