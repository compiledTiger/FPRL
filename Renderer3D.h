#pragma once

#include "Being.h"
#include "Color.h"
#include "FrameBuffer.h"
#include "Map.h"
#include "Mat2D.h"

struct Renderer3D {
    struct ddaHit {
        double dist;
        int mapIndex;                      // TODO remove
        WallTexture<32, 32>* wallTexture;  // TODO guaranteed non-null?
        double wallPos;
    };

    unsigned char flicker = 0;

    void drawView(const Being& player, Map<16>& map, FrameBuffer& fb) {
        int columns = fb.width;
        flicker += rand() % 50 - 20;
        for (double screenX = 0; screenX < columns; ++screenX) {
            double fov = 0.7;
            Vec2D ray = player.view +
                        (player.cam * (screenX / columns * 2.0 - 1.0) * fov);
            auto hit = dda(player, map, ray);
            if (hit) {
                int wallHeight = 255 / (hit->dist + 0.001);

                int wallTop = (fb.height - wallHeight) / 2;
                int screenY = std::max(wallTop, 0);

                int texelX = hit->wallPos * hit->wallTexture->width;
                double texelY = 0;
                double dTexelY =
                    ((double)hit->wallTexture->height) / wallHeight;
                texelY += dTexelY * -std::min(wallTop, 0);

                double lightFactor = 1.0 / (0.3 * hit->dist * hit->dist + 1);
                double torchFactor =
                    1.0 / (hit->dist * hit->dist * hit->dist + 1);

                for (int i = 0; i < wallHeight; ++i) {
                    if (screenY >= fb.height) break;
                    auto color =
                        hit->wallTexture->at(texelX, (int)texelY) *
                            std::min((lightFactor + torchFactor), 1.0) +
                        Color(flicker / 4, 0, 0) * torchFactor;
                    fb.drawPixel(screenX, screenY, color);

                    ++screenY;
                    texelY += dTexelY;
                }
            }
        }
    }

    std::optional<ddaHit> dda(const Being& player, Map<16>& map, Vec2D ray,
                              FrameBuffer* debug = nullptr) {
        Vec2D rayPos = player.pos;

        int index = (int)rayPos.y * 16 + (int)rayPos.x;
        int iter = 0;
        double ratio = ray.x / ray.y + 1e-30;
        int xDir = ray.x > 0;
        int yDir = ray.y > 0;
        int rayTargetX = (int)rayPos.x + xDir;
        int rayTargetY = (int)rayPos.y + yDir;
        double wallPos = 0;
        WallTexture<32, 32>* wallTexture;

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
                debug->drawRect(rayPos.x * 30, rayPos.y * 30, 5, 5,
                                Color(100, 30, 200));
        }
        if (index != -1 && map[index])
            return ddaHit{(rayPos - player.pos).dot(player.view.norm()), index,
                          map[index], wallPos};
        else
            return std::nullopt;
    }

    void drawDebug(const Being& player, Map<16>& map, FrameBuffer& fb) {
        // Draw map
        for (int i = 0; i < 16 * 16; ++i) {
            fb.drawRect((i % 16) * 30, (i / 16) * 30, 29, 29,
                        Color(80 * (bool)map[i], 70 * (bool)map[i], 70));
        }

        int scaling = 30;
        auto drawW = [&](Vec2D v) {
            fb.drawRect(v.x * 30, v.y * 30, 5, 5, Color(255, 255, 255), true);
        };

        drawW(player.pos);

        auto p = player.pos + player.view;
        p -= player.cam;
        drawW(p);
        p += player.cam * 2;
        drawW(p);

        int columns = fb.width;
        for (double screenX = 0; screenX < columns; ++screenX) {
            Vec2D ray =
                player.view + (player.cam * (screenX / columns * 2.0 - 1.0));
            dda(player, map, ray, &fb);
        }
    }
};
