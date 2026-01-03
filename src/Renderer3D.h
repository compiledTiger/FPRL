#pragma once

#include "Being.h"
#include "Color.h"
#include "FrameBuffer.h"
#include "Map.h"
#include "Mat2D.h"
#include "Dda.h"

struct Renderer3D {
    FrameBuffer & fb;
    Map<MAP_SIZE> & map;
    std::optional<DdaHit> * walls;
    std::optional<DdaHit> * beings; // Currently one being per x

    Renderer3D(FrameBuffer & fb_, Map<MAP_SIZE> & map_)
        : fb(fb_), map(map_), walls(new std::optional<DdaHit>[fb.width]),
          beings(new std::optional<DdaHit>[fb.width]) {}

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

    void drawView(const Being & player) {
        rayCast(player);
        drawFloorAndCeiling(player);
        drawWalls(player);
        drawBeings(player);
    }

    void drawFloorAndCeiling(const Being & player) {
        for (int x = 0; x < fb.width; ++x) {
            double dY = walls[x]->dist;
            double ratio =
                double(x - fb.width / 2) / double((fb.width / 2)) * fov;
            double dX = ratio * dY;
            for (int y = calcHeight(dY) / 2; y < fb.height / 2; ++y) {
                dY = invCalcHeight((y + 1) * 2);
                dX = dY * ratio;
                Vec2D floorPos =
                    player.dir * dY + player.cam() * dX + player.getPos();
                int qX = (int)floorPos.x;
                int qY = (int)floorPos.y;
                double rX = floorPos.x - qX;
                double rY = floorPos.y - qY;
                // TODO remove hardcoding
                Texture<32, 32> * floorTexture = map.floor[qX + MAP_SIZE * qY];
                Texture<32, 32> * ceilingTexture =
                    map.ceiling[qX + MAP_SIZE * qY];

                int darknessLevel = map.darknessLevel[qX + MAP_SIZE * qY];
                Color cF = floorTexture->at(rX * 32, rY * 32, darknessLevel);
                Color cC = ceilingTexture->at(rX * 32, rY * 32, darknessLevel);

                fb.drawPixel(fb.area / 2 + (x + fb.width * y), cF);
                fb.drawPixel(fb.area / 2 - (-x + fb.width * (y + 1)), cC);
            }
        }
    }

    void drawWalls(const Being & player) {
        for (int screenX = 0; screenX < fb.width; ++screenX) {
            auto hit = walls[screenX];
            if (hit) {
                double wallHeight = calcHeight(hit->dist);

                double wallTop = (double(fb.height) - wallHeight) / 2.0;
                int screenY = std::max(wallTop, 0.0);

                auto * wallTexture = map[hit->mapIndex];
                int texelX = hit->wallPos * wallTexture->width;
                double texelY = 0;
                double dTexelY = ((double)wallTexture->height) / wallHeight;
                texelY += dTexelY * -std::min(wallTop, 0.0);

                for (int i = 0; i < wallHeight; ++i) {
                    if (screenY >= fb.height)
                        break;
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

    void drawBeings(const Being & player) {
        for (int screenX = 0; screenX < fb.width; ++screenX) {
            auto hit = beings[screenX];
            if (hit) {
                double wallHeight = calcHeight(hit->dist);

                double wallTop = (double(fb.height) - wallHeight) / 2.0 +
                                 double(fb.height) * 0.25 / hit->dist;
                int screenY = std::max(wallTop, 0.0);

                auto * wallTexture = map.beings[hit->mapIndex]->texture;
                int texelX = hit->wallPos * wallTexture->width;
                double texelY = 0;
                double dTexelY = ((double)wallTexture->height) / wallHeight;
                texelY += dTexelY * -std::min(wallTop, 0.0);

                for (int i = 0; i < wallHeight; ++i) {
                    if (screenY >= fb.height)
                        break;

                    Color color =
                        wallTexture->at(texelX, std::round(texelY),
                                        map.darknessLevel[hit->normalIndex]);
                    if (color.a)
                        fb.drawPixel(screenX, screenY, color);

                    ++screenY;
                    texelY += dTexelY;
                }
            }
        }
    }

    void rayCast(const Being & player) {
        Vec2D ray = player.dir - player.cam() * fov;
        Vec2D dRay = player.cam() * fov * (2.0 / fb.width);
        for (int screenX = 0; screenX < fb.width; ++screenX) {
            dda(player, ray, screenX);
            dda(player, ray, screenX);
            ray += dRay;
        }
    }

    void dda(const Being & player, Vec2D ray, int screenX, bool debug = false) {
        Dda dda(player, ray, map, &fb);
        beings[screenX] = std::nullopt;
        walls[screenX] = std::nullopt;
        auto res = dda.run();
        if (res.being)
            beings[screenX] = *res.being;
        if (res.wall)
            walls[screenX] = *res.wall;
    }

    int mapSquareSize = fb.height / MAP_SIZE;
    void drawDebug(const Being & player) {
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

        drawW(player.getPos());

        int columns = fb.width;
        for (double screenX = 0; screenX < columns; ++screenX) {
            Vec2D ray =
                player.dir + (player.cam() * (screenX / columns * 2.0 - 1.0));
            dda(player, ray, screenX, true);
        }
    }
};
