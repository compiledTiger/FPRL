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
    Vec2D cam = Vec2D(1, 0);

    Being() = default;
    Being(const Vec2D& pos_) : pos(pos_) {}

    void update() {
        vel *= 0.7;
        pos += vel;
    }

    void drawView(FrameBuffer& fb, char* map) {
        int columns = fb.width;
        for (double screenX = 0; screenX < columns; ++screenX) {
            double fov = 0.7;
            Vec2D ray = view + (cam * (screenX / columns * 2.0 - 1.0) * fov);
            auto hit = dda(fb, map, ray);
            if (hit) {
                double dist = hit->first;
                int mapIndex = hit->second;
                int wallHeight = 255 / (dist + 0.001);
                if (wallHeight > 1000) wallHeight = 1000;
                for (int screenY = (fb.height - wallHeight) / 2;
                     screenY < (fb.height + wallHeight) / 2; ++screenY) {
                    fb.drawPixel(screenX, screenY,
                                 Color(100 / (0.5 * dist * dist + 1) + 100,
                                       80 * map[mapIndex], 70));
                }
            }
        }
    }

    std::optional<std::pair<double, int>> dda(FrameBuffer& fb, char* map,
                                              Vec2D ray, bool debug = false) {
        Vec2D rayPos = pos;

        int index = (int)rayPos.y * 16 + (int)rayPos.x;
        int iter = 0;
        double ratio = ray.x / ray.y + 1e-30;
        int xDir = ray.x > 0;
        int yDir = ray.y > 0;
        int rayTargetX = (int)rayPos.x + xDir;
        int rayTargetY = (int)rayPos.y + yDir;

        while (index != -1 && map[index] == 0 && ++iter < 20) {
            double wallDistX = rayTargetX - rayPos.x;
            double wallDistY = rayTargetY - rayPos.y;

            if (wallDistX * wallDistX +
                    (wallDistX / ratio) * (wallDistX / ratio) <
                wallDistY * wallDistY +
                    (wallDistY * ratio) * (wallDistY * ratio)) {
                rayPos.x += wallDistX;
                rayPos.y += wallDistX / ratio;
                rayTargetX += xDir * 2 - 1;
            } else {
                rayPos.y += wallDistY;
                rayPos.x += wallDistY * ratio;
                rayTargetY += yDir * 2 - 1;
            }

            index = (int)((rayPos.y + rayTargetY) / 2.0) * 16 +
                    (int)((rayPos.x + rayTargetX) / 2.0);
            if (index > 255) index = -1;
            if (index < 0) index = -1;

            if (debug)
                fb.drawRect(rayPos.x * 30, rayPos.y * 30, 5, 5,
                            Color(100, 30, 200));
        }
        if (index != -1 && map[index])
            return std::make_pair((rayPos - pos).dot(view.norm()), index);
        else
            return std::nullopt;
    }

    void drawDebug(FrameBuffer& fb, char* map) {
        // Draw map
        for (int i = 0; i < 16 * 16; ++i) {
            fb.drawRect((i % 16) * 30, (i / 16) * 30, 29, 29,
                        Color(80 * map[i], 70 * map[i], 70));
        }

        int scaling = 30;
        auto drawW = [&](Vec2D v) {
            fb.drawRect(v.x * 30, v.y * 30, 5, 5, Color(255, 255, 255), true);
        };

        drawW(pos);

        auto p = pos + view;
        p -= cam;
        drawW(p);
        p += cam * 2;
        drawW(p);

        int columns = fb.width;
        for (double screenX = 0; screenX < columns; ++screenX) {
            Vec2D ray = view + (cam * (screenX / columns * 2.0 - 1.0));
            dda(fb, map, ray, true);
        }
    }

    void rotate(double rad) {
        dir = rotationM(rad) * dir;
        view = rotationM(rad) * view;
        cam = rotationM(rad) * cam;
    }
};
