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
        auto lightFactor = [](double dist) {
            return 1.0 / (0.3 * dist * dist + 1);
        };
        auto torchFactor = [](double dist) {
            return 1.0 / (dist * dist * dist + 1);
        };
        flicker += rand() % 50 - 20;
        Color torchColor(flicker / 4, 0, 0);

        // Clear buffer and draw ceiling and floor
        for (int y = 0; y < fb.height / 2; ++y) {
            for (int x = 0; x < fb.width; ++x)
                fb.drawPixel(x, y, Color(14, 28, 46));
        }
        for (int y = fb.height / 2; y < fb.height; ++y) {
            // TODO take x into account, and make it fast
            auto d = (double(fb.height - y) / double(fb.height / 2)) * 3.0;
            auto lf = lightFactor(d);
            auto tf = torchFactor(d);
            auto c =
                Color(36, 28, 14) * std::min(lf + tf, 1.0) + torchColor * tf;
            for (int x = 0; x < fb.width; ++x) fb.drawPixel(x, y, c);
        }

        int columns = fb.width;
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

                auto tf = torchFactor(hit->dist);
                for (int i = 0; i < wallHeight; ++i) {
                    if (screenY >= fb.height) break;
                    auto color =
                        hit->wallTexture->at(texelX, (int)texelY) *
                            std::min((lightFactor(hit->dist) + tf), 1.0) +
                        torchColor * tf;
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
