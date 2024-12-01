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
        vel *= 0.7;
        pos += vel;
    }

    void draw(FrameBuffer& fb) {
        fb.drawRect(pos.x * 30, pos.y * 30, 10, 10, Color(255, 255, 255), true);
    }

    void drawDebug(FrameBuffer& fb, char* map) {
        int scaling = 30;
        auto p = pos * 30 + view * scaling;
        fb.drawRect(p.x, p.y, 5, 5, Color(255, 255, 255), true);
        p -= cam * scaling;
        fb.drawRect(p.x, p.y, 5, 5, Color(255, 255, 255), true);
        p += cam * scaling * 2;
        fb.drawRect(p.x, p.y, 5, 5, Color(255, 255, 255), true);

        for (double screenX = 0; screenX < fb.width; ++screenX) {
            Vec2D ray = view + (cam * (screenX / fb.width * 2.0 - 1.0));
            Vec2D rayPos = pos;

            int index = (int)rayPos.y * 16 + (int)rayPos.x;
            int iter = 0;
            double ratio = ray.x / ray.y + 1e-30;
            int xDir = ray.x > 0;
            int yDir = ray.y > 0;
            int rayTargetX = (int)rayPos.x + xDir;
            int rayTargetY = (int)rayPos.y + yDir;

            while (map[index] == 0 && ++iter < 3) {
                double wallDistX = rayTargetX - rayPos.x;
                double wallDistY = rayTargetY - rayPos.y;

                if (std::abs(wallDistX) < std::abs(wallDistY)) {
                    rayPos.x += wallDistX;
                    rayPos.y += wallDistX / ratio;
                    rayTargetX += xDir*2 - 1;
                } else {
                    rayPos.y += wallDistY;
                    rayPos.x += wallDistY * ratio;
                    rayTargetY += yDir*2 - 1;
                }
                std::cout << "pos    " << pos.x << " " << pos.y << std::endl;
                std::cout << "raypos " << rayPos.x << " " << rayPos.y
                          << std::endl;
                index = (int)rayPos.y * 16 + (int)rayPos.x;
                if (index > 255) index = 255;
                if (index < 0) index = 0;
                std::cout << index << " index" << std::endl;
            }
            if (map[index] && rayPos.x > 0 && rayPos.x*30 < fb.width && rayPos.y > 0 &&
                rayPos.y*30 < fb.height)
            fb.drawRect(rayPos.x * 30, rayPos.y * 30, 5, 5,
                        Color(255, 255, 255), true);
            fb.drawRect((pos + ray).x * 30, (pos + ray).y * 30, 5, 5,
                        Color(255, 255, 255), true);
        }
    }

    void rotate(double rad) {
        dir = rotationM(rad) * dir;
        view = rotationM(rad) * view;
        cam = rotationM(rad) * cam;
    }
};
