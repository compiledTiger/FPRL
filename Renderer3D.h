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
    Map<MAP_SIZE>& map;

    Renderer3D(FrameBuffer& fb_, Map<MAP_SIZE>& map_) : fb(fb_), map(map_) {}

    const double epsilon = 0.001;
    const double baseHeight = double(fb.height);
    double calcHeight(double dist) const {
        return baseHeight / (dist + epsilon);
    }
    double invCalcHeight(double height) const {
        return 1.0 / height * baseHeight - epsilon;
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
                    player.dir * dY + player.cam() * dX + player.pos;
                // TODO remove hardcoding
                int qX = (int)floorPos.x;
                int qY = (int)floorPos.y;
                double rX = floorPos.x - qX;
                double rY = floorPos.y - qY;
                double dist = std::sqrt(dX * dX + dY * dY);
                // TODO this one seems to filter out a lot, is the loop above to
                // wide? qX and qY takes crazy values
                if (qX > 0 && qX < MAP_SIZE && qY > 0 && qY < MAP_SIZE) {
                    Texture<32, 32>* floorTexture =
                        map.floor[qX + MAP_SIZE * qY];
                    Texture<32, 32>* ceilingTexture =
                        map.ceiling[qX + MAP_SIZE * qY];

                    int darknessLevel = map.darknessLevel[qX + MAP_SIZE * qY];
                    Color cF = floorTexture->at(rX * 32, rY * 32, darknessLevel,
                                                downsampleLevel(dist));
                    Color cC = ceilingTexture->at(
                        rX * 32, rY * 32, darknessLevel, downsampleLevel(dist));

                    fb.drawPixel(fb.area / 2 + (x + fb.width * y), cF);
                    fb.drawPixel(fb.area / 2 - (-x + fb.width * (y + 1)), cC);
                }
            }
        }
    }

    void drawWalls(const Being& player) {
        int columns = fb.width;
        for (double screenX = 0; screenX < columns; ++screenX) {
            Vec2D ray = player.dir +
                        (player.cam() * fov * (screenX / columns * 2.0 - 1.0));
            auto hit = dda(player, ray);
            if (hit) {
                double wallHeight = calcHeight(hit->dist);

                double wallTop = (double(fb.height) - wallHeight) / 2.0;
                int screenY = std::max(wallTop, 0.0);

                int texelX = hit->wallPos * hit->wallTexture->width;
                double texelY = 0;
                double dTexelY =
                    ((double)hit->wallTexture->height) / wallHeight;
                texelY += dTexelY * -std::min(wallTop, 0.0);

                for (int i = 0; i < wallHeight; ++i) {
                    if (screenY >= fb.height) break;
                    Color color =
                        hit->wallTexture->at(texelX, std::round(texelY),
                                             map.darknessLevel[hit->mapIndex],
                                             downsampleLevel(hit->dist));

                    fb.drawPixel(screenX, screenY, color);

                    ++screenY;
                    texelY += dTexelY;
                }
            }
        }
    }

    int downsampleLevel(double dist) { return std::min(1, int(dist / 12)); }

    std::optional<ddaHit> dda(const Being& player, Vec2D ray,
                              bool debug = false) {
        Vec2D rayPos = player.pos;

        int index = (int)rayPos.y * MAP_SIZE + (int)rayPos.x;
        int iter = 0;
        double ratio = ray.x / ray.y + 1e-30;
        int xDir = ray.x > 0;
        int yDir = ray.y > 0;
        int rayTargetX = (int)rayPos.x + xDir;
        int rayTargetY = (int)rayPos.y + yDir;
        double wallPos = 0;
        Texture<32, 32>* wallTexture;

        while (index != -1 && map[index] == nullptr && ++iter < MAP_SIZE) {
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

            index = (int)((rayPos.y + rayTargetY) / 2.0) * MAP_SIZE +
                    (int)((rayPos.x + rayTargetX) / 2.0);
            if (index > MAP_AREA) index = -1;
            if (index < 0) index = -1;
            if (debug)
                fb.drawRect(rayPos.x * mapSquareSize, rayPos.y * mapSquareSize,
                            1, 1, Color(100, 30, 200));
        }
        if (index != -1 && map[index]) {
            double dist = (rayPos - player.pos).dot(player.dir.norm());
            return ddaHit{dist, index, map[index], wallPos};
        } else
            return std::nullopt;
    }

    int mapSquareSize = fb.height / MAP_SIZE;
    void drawDebug(const Being& player) {
        // Draw map
        for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
            fb.drawRect((i % MAP_SIZE) * mapSquareSize,
                        (i / MAP_SIZE) * mapSquareSize, mapSquareSize,
                        mapSquareSize,
                        Color(80 * (bool)map[i], 70 * (bool)map[i], 70));
        }

        auto drawW = [&](Vec2D v) {
            fb.drawRect(v.x * mapSquareSize, v.y * mapSquareSize, 2, 2,
                        Color(255, 255, 255), true);
        };

        drawW(player.pos);

        int columns = fb.width;
        for (double screenX = 0; screenX < columns; ++screenX) {
            Vec2D ray =
                player.dir + (player.cam() * (screenX / columns * 2.0 - 1.0));
            dda(player, ray, true);
        }
    }
};
