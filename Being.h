#pragma once

#include <cstdlib>
#include <optional>

#include "Map.h"
#include "Vec2D.h"
#include "Mat2D.h"

struct Being {
    Vec2D pos = Vec2D(0, 0);
    Vec2D dir = Vec2D(0, -1);
    Vec2D vel = Vec2D(0, 0);
    Vec2D view = Vec2D(0, -1);
    Vec2D cam = Vec2D(0.75, 0);

    Being() = default;
    Being(const Vec2D& pos_) : pos(pos_) {}

    void update(Map<16>& map) {
        vel *= 0.7;
        auto col = pos + (vel * 5.0);
        if (map.at(int(col.x), int(col.y))) {
            vel *= 0.0;
        }
        pos += vel;
    }

    void rotate(double rad) {
        dir = rotationM(rad) * dir;
        view = rotationM(rad) * view;
        cam = rotationM(rad) * cam;
    }
};
