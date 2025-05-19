#pragma once

#include "Being.h"
#include "Color.h"
#include "FrameBuffer.h"
#include "Map.h"
#include "Mat2D.h"

struct Renderer3D {
    enum class Side { north, west, south, east };

    struct DdaHit {
        int mapIndex;
        double dist;
        double wallPos;
        int normalIndex;
    };

    FrameBuffer& fb;
    Map<MAP_SIZE>& map;
    std::optional<DdaHit>* ddaRes;

    Renderer3D(FrameBuffer& fb_, Map<MAP_SIZE>& map_)
        : fb(fb_), map(map_), ddaRes(new std::optional<DdaHit>[fb.width]) {}

    double fov = 0.75;
    // TODO what is the relationship to fov?
    const double baseHeight = double(fb.height) * 0.7;
    const double epsilon = 0.001;
    double calcHeight(double dist) const {
        return baseHeight / (dist + epsilon);
    }
    double invCalcHeight(double height) const {
        return 1.0 / height * baseHeight - epsilon;
    }

    void drawView(const Being& player) {
        rayCast(player);
        drawFloorAndCeiling(player);
        drawWalls(player);
    }

    void drawFloorAndCeiling(const Being& player) {
        for (int x = 0; x < fb.width; ++x) {
            double dY = ddaRes[x]->dist;
            double ratio =
                double(x - fb.width / 2) / double((fb.width / 2)) * fov;
            double dX = ratio * dY;
            for (int y = calcHeight(dY) / 2; y < fb.height / 2; ++y) {
                dY = invCalcHeight((y + 1) * 2);
                dX = dY * ratio;
                Vec2D floorPos =
                    player.dir * dY + player.cam() * dX + player.pos;
                int qX = (int)floorPos.x;
                int qY = (int)floorPos.y;
                double rX = floorPos.x - qX;
                double rY = floorPos.y - qY;
                // TODO remove hardcoding
                Texture<32, 32>* floorTexture = map.floor[qX + MAP_SIZE * qY];
                Texture<32, 32>* ceilingTexture =
                    map.ceiling[qX + MAP_SIZE * qY];

                int darknessLevel = map.darknessLevel[qX + MAP_SIZE * qY];
                Color cF = floorTexture->at(rX * 32, rY * 32, darknessLevel);
                Color cC = ceilingTexture->at(rX * 32, rY * 32, darknessLevel);

                fb.drawPixel(fb.area / 2 + (x + fb.width * y), cF);
                fb.drawPixel(fb.area / 2 - (-x + fb.width * (y + 1)), cC);
            }
        }
    }

    void drawWalls(const Being& player) {
        for (int screenX = 0; screenX < fb.width; ++screenX) {
            auto hit = ddaRes[screenX];
            if (hit) {
                double wallHeight = calcHeight(hit->dist);

                double wallTop = (double(fb.height) - wallHeight) / 2.0;
                int screenY = std::max(wallTop, 0.0);

                auto* wallTexture = map[hit->mapIndex];
                int texelX = hit->wallPos * wallTexture->width;
                double texelY = 0;
                double dTexelY = ((double)wallTexture->height) / wallHeight;
                texelY += dTexelY * -std::min(wallTop, 0.0);

                for (int i = 0; i < wallHeight; ++i) {
                    if (screenY >= fb.height) break;
                    Color color =
                        wallTexture->at(texelX, std::round(texelY),
                                        map.darknessLevel[hit->normalIndex]);

                    fb.drawPixel(screenX, screenY, color);

                    ++screenY;
                    texelY += dTexelY;
                }
            }
        }
    }

    void rayCast(const Being& player) {
        Vec2D ray = player.dir - player.cam() * fov;
        Vec2D dRay = player.cam() * fov * (2.0 / fb.width);
        for (int screenX = 0; screenX < fb.width; ++screenX) {
            ddaRes[screenX] = dda(player, ray);
            ray += dRay;
        }
    }

    std::optional<DdaHit> dda(const Being& player, Vec2D ray,
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
        bool verticalSide;

        // *1.5 since diagonal distances are longer than the map size
        while (index != -1 && map[index] == nullptr &&
               ++iter < MAP_SIZE * 1.5) {
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
                verticalSide = true;
            } else {
                rayPos.y += wallDistY;
                rayPos.x += wallDistY * ratio;
                rayTargetY += yDir * 2 - 1;
                wallPos = rayPos.x - (int)rayPos.x;
                verticalSide = false;
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
            Side side = verticalSide ? (xDir ? Side::west : Side::east)
                                     : (yDir ? Side::north : Side::south);
            int normalIndex = index;
            switch (side) {
                case Side::north:
                    normalIndex -= MAP_SIZE;
                    break;
                case Side::west:
                    normalIndex -= 1;
                    wallPos = 1 - wallPos;
                    break;
                case Side::south:
                    normalIndex += MAP_SIZE;
                    wallPos = 1 - wallPos;
                    break;
                case Side::east:
                    normalIndex += 1;
                    break;
            };
            return DdaHit{index, dist, wallPos, normalIndex};
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
