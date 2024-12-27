#pragma once

#include "Being.h"
#include "Color.h"
#include "FrameBuffer.h"
#include "Map.h"
#include "Mat2D.h"

struct Renderer3D {
    struct ddaHit {
        double dist;
        int mapIndex;                  // TODO remove
        Texture<32, 32>* wallTexture;  // TODO guaranteed non-null?
        double wallPos;
    };

    FrameBuffer& fb;
    Map<16>& map;

    Renderer3D(FrameBuffer& fb_, Map<16>& map_) : fb(fb_), map(map_) {}

    double lightFactor(double dist) { return 1.0 / (1.5 * dist + 1.0); };

    double epsilon = 0.001;
    double calcHeight(double dist) const { return 255.0 / (dist + epsilon); }
    double invCalcHeight(double height) const {
        return 1.0 / height * 255.0 - epsilon;
    }

    double fov = 0.75;

    void drawView(const Being& player) {
        drawFloorAndCeiling(player);
        drawWalls(player);
    }

    void drawFloorAndCeiling(const Being& player) {
        for (int y = 0; y < fb.height / 2; ++y) {
            double dY = invCalcHeight((y + 1) * 2);
            for (int x = 0; x < fb.width; ++x) {
                double dX = double(x - fb.width / 2) / double((fb.width / 2)) *
                            fov * dY;
                Vec2D floorPos =
                    player.view * dY + player.cam * dX + player.pos;
                // TODO remove hardcoding
                int qX = (int)floorPos.x;
                int qY = (int)floorPos.y;
                double rX = floorPos.x - qX;
                double rY = floorPos.y - qY;
                double dist = std::sqrt(dX * dX + dY * dY);
                auto lf = lightFactor(dist);
                if (qX > 0 && qX < 16 && qY > 0 && qY < 16) {
                    Texture<32, 32>* floorTexture = map.floor[qX + 16 * qY];
                    Color c = floorTexture->at(rX * 32, rY * 32) * lf;
                    fb.drawPixel(fb.area / 2 + (x + fb.width * y), c);
                    fb.drawPixel(fb.area / 2 - (-x + fb.width * (y + 1)), c);
                }
            }
        }
    }

    void drawWalls(const Being& player) {
        int columns = fb.width;
        for (double screenX = 0; screenX < columns; ++screenX) {
            Vec2D ray = player.view +
                        (player.cam * fov * (screenX / columns * 2.0 - 1.0));
            auto hit = dda(player, ray);
            if (hit) {
                int wallHeight = calcHeight(hit->dist);

                int wallTop = (fb.height - wallHeight) / 2;
                int screenY = std::max(wallTop, 0);

                int texelX = hit->wallPos * hit->wallTexture->width;
                double texelY = 0;
                double dTexelY =
                    ((double)hit->wallTexture->height) / double(wallHeight);
                texelY += dTexelY * -std::min(double(wallTop), 0.0);

                auto lf = lightFactor(hit->dist);
                for (int i = 0; i < wallHeight; ++i) {
                    if (screenY >= fb.height) break;
                    Color color =
                        hit->wallTexture->at(texelX, std::round(texelY)) * lf;

                    fb.drawPixel(screenX, screenY, color);

                    ++screenY;
                    texelY += dTexelY;
                }
            }
        }
    }

    std::optional<ddaHit> dda(const Being& player, Vec2D ray,
                              bool debug = false) {
        Vec2D rayPos = player.pos;

        int index = (int)rayPos.y * 16 + (int)rayPos.x;
        int iter = 0;
        double ratio = ray.x / ray.y + 1e-30;
        int xDir = ray.x > 0;
        int yDir = ray.y > 0;
        int rayTargetX = (int)rayPos.x + xDir;
        int rayTargetY = (int)rayPos.y + yDir;
        double wallPos = 0;
        Texture<32, 32>* wallTexture;

        while (index != -1 && map[index] == nullptr && ++iter < 20) {
            double wallDistX = rayTargetX - rayPos.x;
            double wallDistY = rayTargetY - rayPos.y;

            if (wallDistX * wallDistX +
                    (wallDistX / ratio) * (wallDistX / ratio) <
                wallDistY * wallDistY +
                    (wallDistY * ratio) * (wallDistY * ratio)) {
                rayPos.x += wallDistX;
                rayPos.y += wallDistX / ratio;
                rayTargetX += xDir * 2 - 1;
                wallPos = rayPos.y - (int)rayPos.y;
            } else {
                rayPos.y += wallDistY;
                rayPos.x += wallDistY * ratio;
                rayTargetY += yDir * 2 - 1;
                wallPos = rayPos.x - (int)rayPos.x;
            }

            if (rayPos.x < 0 || rayPos.x >= map.length || rayPos.y < 0 ||
                rayPos.y >= map.length)
                break;

            index = (int)((rayPos.y + rayTargetY) / 2.0) * 16 +
                    (int)((rayPos.x + rayTargetX) / 2.0);
            if (index > 255) index = -1;
            if (index < 0) index = -1;
            if (debug)
                fb.drawRect(rayPos.x * 30, rayPos.y * 30, 5, 5,
                            Color(100, 30, 200));
        }
        if (index != -1 && map[index]) {
            double dist = (rayPos - player.pos).dot(player.view.norm());
            return ddaHit{dist, index, map[index], wallPos};
        } else
            return std::nullopt;
    }

    void drawDebug(const Being& player) {
        // Draw map
        for (int i = 0; i < 16 * 16; ++i) {
            fb.drawRect((i % 16) * 30, (i / 16) * 30, 30, 30,
                        Color(80 * (bool)map[i], 70 * (bool)map[i], 70));
        }

        int scaling = 30;
        auto drawW = [&](Vec2D v) {
            fb.drawRect(v.x * 30, v.y * 30, 5, 5, Color(255, 255, 255), true);
        };

        drawW(player.pos);

        int columns = fb.width;
        for (double screenX = 0; screenX < columns; ++screenX) {
            Vec2D ray =
                player.view + (player.cam * (screenX / columns * 2.0 - 1.0));
            dda(player, ray, true);
        }
    }
};
