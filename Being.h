#pragma once

#include <optional>

#include "Color.h"
#include "FrameBuffer.h"
#include "Mat2D.h"
#include "Vec2D.h"

struct Being {
    Vec2D pos = Vec2D(0, 0);
    Vec2D dir = Vec2D(0, -1);
    Vec2D vel = Vec2D(0, 0);
    Vec2D view = Vec2D(0, -1);
    Vec2D cam = Vec2D(-1, 0);

    Being() = default;
    Being(const Vec2D& pos_) : pos(pos_) {}

    void update() {
        vel *= 0.9;
        pos += vel;
    }

    void draw(FrameBuffer& fb) {
        fb.drawRect(pos.x, pos.y, 10, 10, Color(255, 255, 255), true);
    }

    void drawDebug(FrameBuffer& fb) {
        int scaling = 10;
        auto p = pos + view * scaling;
        fb.drawRect(p.x, p.y, 5, 5, Color(255, 255, 255), true);
        p -= cam * scaling;
        fb.drawRect(p.x, p.y, 5, 5, Color(255, 255, 255), true);
        p += cam * scaling * 2;
        fb.drawRect(p.x, p.y, 5, 5, Color(255, 255, 255), true);
    }

    void rotate(double rad) {
        dir = rotationM(rad) * dir;
        view = rotationM(rad) * view;
        cam = rotationM(rad) * cam;
    }
};
