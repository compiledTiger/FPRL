#pragma once

#include "Being.h"
#include "Constants.h"
#include "FrameBuffer.h"

struct DdaHit {
    int mapIndex;
    double dist;
    double wallPos;
    int normalIndex;
};

class Dda {
  public:
    Dda(const Being & player_, const Map<MAP_SIZE> & map_)
        : player(player_), map(map_) {}

    void run(Vec2D ray, std::optional<DdaHit> & wallHit,
             std::optional<DdaHit> & beingHit, FrameBuffer * debugFb) {
        wallHit = std::nullopt;
        beingHit = std::nullopt;

        Vec2D rayPos = player.getPos();

        int index = (int)rayPos.y * MAP_SIZE + (int)rayPos.x;
        double ratio = ray.x / ray.y + 1e-30;

        int xDir = ray.x > 0; // west or east
        int yDir = ray.y > 0; // north or south
        const double initialXStep = (int)rayPos.x + xDir - rayPos.x;
        const double initialYStep = (int)rayPos.y + yDir - rayPos.y;
        Vec2D xStep = Vec2D(initialXStep, initialXStep / ratio);
        Vec2D yStep = Vec2D(initialYStep * ratio, initialYStep);
        Vec2D rayTargetX = rayPos + xStep;
        Vec2D rayTargetY = rayPos + yStep;
        const double laterXStep = (xDir * 2 - 1) / 2.0;
        const double laterYStep = (yDir * 2 - 1) / 2.0;
        xStep = Vec2D(laterXStep, laterXStep / ratio);
        yStep = Vec2D(laterYStep * ratio, laterYStep);

        double wallPos = 0;
        Vec2D wallDistX(0, 0);
        Vec2D wallDistY(0, 0);
        int iter = 0;
        bool verticalSide;
        // *1.5 since diagonal distances are longer than the map size
        while (++iter < MAP_SIZE * 1.5) {
            wallDistX = rayTargetX - rayPos;
            wallDistY = rayTargetY - rayPos;

            // TODO skip square root here
            if (wallDistX.squaredMag() < wallDistY.squaredMag()) {
                rayPos += wallDistX;
                rayTargetX += xStep;
                wallPos = rayPos.y - (int)rayPos.y;
                verticalSide = true;
            } else {
                rayPos += wallDistY;
                rayTargetY += yStep;
                wallPos = rayPos.x - (int)rayPos.x;
                verticalSide = false;
            }

            if (rayPos.x < 0 || rayPos.x >= map.length || rayPos.y < 0 ||
                rayPos.y >= map.length)
                break;

            // TODO this need to be updated!
            index = (int)((rayPos.y + rayTargetY.y) / 2.0) * MAP_SIZE +
                    (int)((rayPos.x + rayTargetX.x) / 2.0);
            if (index > MAP_AREA)
                return;
            if (index < 0)
                return;

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

            bool atEdge = verticalSide ? int(rayPos.x * 2) % 2 == 0
                                       : int(rayPos.y * 2) % 2 == 0;
            // std::cout << rayPos << " " << rayTargetX << " " << rayTargetY
            //           << " " << verticalSide << " " << atEdge << std::endl;

            if (debugFb)
                debugFb->drawRect(rayPos.x * (debugFb->height / MAP_SIZE),
                                  rayPos.y * (debugFb->height / MAP_SIZE), 1, 1,
                                  Color(100, 30 + 100 * atEdge, 200));

            if (atEdge) {
                if (map.walls[index]) {
                    double dist =
                        (rayPos - player.getPos()).dot(player.dir.norm());
                    if (debugFb)
                        debugFb->drawRect(
                            rayPos.x * (debugFb->height / MAP_SIZE),
                            rayPos.y * (debugFb->height / MAP_SIZE), 1, 1,
                            Color(255, 30, 0));
                    wallHit = DdaHit{index, dist, wallPos, normalIndex};
                    return;
                }
            } else {
                std::cout << "looking for beings" << std::endl;
                auto * being = map.beings[index];
                if (being == &player)
                    being = nullptr;
                if (being) {
                    if (being->targetDir != side) {
                        if (being->targetDir - side == 4) {
                            /*
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
                            }*/
                        }
                    }
                    // rita bara om side är samma eller diff 4 (då flippa)
                    if (true) { // TODO
                        double dist =
                            (rayPos - player.getPos()).dot(player.dir.norm());
                        if (debugFb)
                            debugFb->drawRect(
                                rayPos.x * (debugFb->height / MAP_SIZE),
                                rayPos.y * (debugFb->height / MAP_SIZE), 1, 1,
                                Color(255, 30 + 100, 200));
                        beingHit = DdaHit{index, dist, wallPos, normalIndex};
                        std::cout << "HIT A BEING" << std::endl;
                    }
                }
            }
        }
    }

    const Being & player;
    const Map<MAP_SIZE> & map;
};
