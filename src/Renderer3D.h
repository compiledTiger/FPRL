#pragma once

#include "Being.h"
#include "Color.h"
#include "FrameBuffer.h"
#include "Map.h"
#include "Mat2D.h"

struct Renderer3D {
    struct DdaHit {
        int mapIndex;
        double dist;
        double wallPos;
        int normalIndex;
    };

    FrameBuffer& fb;
    Map<MAP_SIZE>& map;
    std::optional<DdaHit>* walls;
    std::optional<DdaHit>* beings;  // Currently one being per x

    Renderer3D(FrameBuffer& fb_, Map<MAP_SIZE>& map_)
        : fb(fb_),
          map(map_),
          walls(new std::optional<DdaHit>[fb.width]),
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

    void drawView(const Being& player) {
        rayCast(player);
        drawFloorAndCeiling(player);
        drawWalls(player);
        drawBeings(player);
    }

    void drawFloorAndCeiling(const Being& player) {
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
            auto hit = walls[screenX];
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

    void drawBeings(const Being& player) {
        for (int screenX = 0; screenX < fb.width; ++screenX) {
            auto hit = beings[screenX];
            if (hit) {
                double wallHeight = calcHeight(hit->dist);

                double wallTop = (double(fb.height) - wallHeight) / 2.0;
                int screenY = std::max(wallTop, 0.0);

                auto* wallTexture = map.beings[hit->mapIndex]->texture;
                int texelX = hit->wallPos * wallTexture->width;
                double texelY = 0;
                double dTexelY = ((double)wallTexture->height) / wallHeight;
                texelY += dTexelY * -std::min(wallTop, 0.0);

                for (int i = 0; i < wallHeight; ++i) {
                    if (screenY >= fb.height) break;

                    Color color =
                        wallTexture->at(texelX, std::round(texelY),
                                        map.darknessLevel[hit->normalIndex]);
                    if (color.a) fb.drawPixel(screenX, screenY, color);

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
            dda(player, ray, screenX);
            dda(player, ray, screenX);
            ray += dRay;
        }
    }

    void dda(const Being& player, Vec2D ray, int screenX, bool debug = false) {
        Vec2D rayPos = player.getPos();

        int index = (int)rayPos.y * MAP_SIZE + (int)rayPos.x;
        int iter = 0;
        double ratio = ray.x / ray.y + 1e-30;
        int xDir = ray.x > 0;
        int yDir = ray.y > 0;
        double rayTargetX = (int)rayPos.x + xDir;
        double rayTargetY = (int)rayPos.y + yDir;
        double wallPos = 0;
        bool verticalSide;
        double wallDistX;
        double wallDistY;

        walls[screenX] = std::nullopt;
        beings[screenX] = std::nullopt;

        // *1.5 since diagonal distances are longer than the map size
        while (map.walls[index] == nullptr && ++iter < MAP_SIZE * 1.5) {
            wallDistX = rayTargetX - rayPos.x;
            wallDistY = rayTargetY - rayPos.y;

            if (wallDistX * wallDistX +
                    (wallDistX / ratio) * (wallDistX / ratio) <
                wallDistY * wallDistY +
                    (wallDistY * ratio) * (wallDistY * ratio)) {
                rayPos.x += wallDistX;
                rayPos.y += wallDistX / ratio;
                rayTargetX += (xDir * 2 - 1) / 2.0;
                wallPos = rayPos.y - (int)rayPos.y;
                verticalSide = true;
            } else {
                rayPos.y += wallDistY;
                rayPos.x += wallDistY * ratio;
                rayTargetY += (yDir * 2 - 1) / 2.0;
                wallPos = rayPos.x - (int)rayPos.x;
                verticalSide = false;
            }

            if (rayPos.x < 0 || rayPos.x >= map.length || rayPos.y < 0 ||
                rayPos.y >= map.length)
                break;

            // TODO this need to be updated!
            index = (int)((rayPos.y + rayTargetY) / 2.0) * MAP_SIZE +
                    (int)((rayPos.x + rayTargetX) / 2.0);
            if (index > MAP_AREA) return;
            if (index < 0) return;

            bool atEdge = verticalSide ? int(rayPos.x * 2) % 2 == 0
                                       : int(rayPos.y * 2) % 2 == 0;
            // std::cout << rayPos << " " << rayTargetX << " " << rayTargetY
            //           << " " << verticalSide << " " << atEdge << std::endl;
            if (debug)
                fb.drawRect(rayPos.x * mapSquareSize, rayPos.y * mapSquareSize,
                            1, 1, Color(100, 30 + 100 * atEdge, 200));

            FixedDir side = verticalSide
                                ? (xDir ? FixedDir::Dir::w : FixedDir::Dir::e)
                                : (yDir ? FixedDir::Dir::n : FixedDir::Dir::s);
            int normalIndex = index;
            switch (side.dir) {
                case FixedDir::Dir::n:
                    normalIndex -= MAP_SIZE;
                    break;
                case FixedDir::Dir::w:
                    normalIndex -= 1;
                    wallPos = 1 - wallPos;
                    break;
                case FixedDir::Dir::s:
                    normalIndex += MAP_SIZE;
                    wallPos = 1 - wallPos;
                    break;
                case FixedDir::Dir::e:
                    normalIndex += 1;
                    break;
                default:
                    // TODO nw, ne, sw, se
                    break;
            }

            if (atEdge) {
                if (map.walls[index]) {
                    double dist =
                        (rayPos - player.getPos()).dot(player.dir.norm());
                    walls[screenX] = DdaHit{index, dist, wallPos, normalIndex};
                }
            } else {
                auto* being = map.beings[index];
                if (being == &player) being = nullptr;
                if (being) {
                    if (being->targetDir != side) {
                        if (being->targetDir - side == 4) {
                            side = side + 4;
                            switch (side.dir) {
                                case FixedDir::Dir::n:
                                    wallPos = 1 - wallPos;
                                    break;
                                case FixedDir::Dir::w:
                                    break;
                                case FixedDir::Dir::s:
                                    break;
                                case FixedDir::Dir::e:
                                    wallPos = 1 - wallPos;
                                    break;
                                default:
                                    // TODO nw, ne, sw, se
                                    break;
                            }
                        } else {  // 90 degrees off
                            being->targetDir = side;
                            switch (side.dir) {
                                case FixedDir::Dir::n:
                                    wallPos = 1 - wallPos;
                                    break;
                                case FixedDir::Dir::w:
                                    wallPos = 1 - wallPos;
                                    break;
                                case FixedDir::Dir::s:
                                    wallPos = 1 - wallPos;
                                    break;
                                case FixedDir::Dir::e:
                                    wallPos = 1 - wallPos;
                                    break;
                                default:
                                    // TODO nw, ne, sw, se
                                    break;
                            }
                        }
                    }
                    if (!beings[screenX]) {
                        double dist =
                            (rayPos - player.getPos()).dot(player.dir.norm());
                        beings[screenX] =
                            DdaHit{index, dist, wallPos, normalIndex};
                    }
                }
            }
        }
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

        drawW(player.getPos());

        int columns = fb.width;
        for (double screenX = 0; screenX < columns; ++screenX) {
            Vec2D ray =
                player.dir + (player.cam() * (screenX / columns * 2.0 - 1.0));
            dda(player, ray, screenX, true);
        }
    }
};
