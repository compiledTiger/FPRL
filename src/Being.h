#pragma once

#include <cstdlib>
#include <iostream>
#include <optional>

#include "Constants.h"
#include "FixedDir.h"
#include "Map.h"
#include "Mat2D.h"
#include "Vec2D.h"

class Being {
   private:
    Map<MAP_SIZE>* map;
    int x;
    int y;
    Vec2D targetPos;
    Vec2D lastPos;
    Vec2D pos;

   public:
    // TODO move private
    FixedDir targetDir;
    Vec2D dir;
    bool moving = false;
    bool rotating = false;
    bool invalidMove = false;

    Being(Map<MAP_SIZE>* map_, int x_, int y_)
        : map(map_),
          x(x_),
          y(y_),
          targetPos(Vec2D(x + 0.5, y + 0.5)),
          lastPos(targetPos),
          pos(targetPos),
          targetDir(FixedDir::Dir::n),
          dir(targetDir) {}

    void update() {
        if (invalidMove && (pos - lastPos).mag() > 0.3) targetPos = lastPos;

        Vec2D posDiff = (targetPos - pos);
        moving = posDiff.mag() > 0.25;
        if (moving)
            pos += posDiff / 8;
        else {
            pos += posDiff / 4;
            if (posDiff.mag() < 0.001) pos = targetPos;
        }

        Vec2D tDir = Vec2D(targetDir).norm();
        if ((tDir - dir).mag()) {
            double radDiff = atan2(tDir.det(dir), tDir.dot(dir));
            rotating = abs(radDiff) > 0.1;
            if (rotating)
                dir = rotationM(-radDiff / 8) * dir;
            else {
                dir = rotationM(-radDiff / 4) * dir;
                if (abs(radDiff) < 0.001) dir = tDir;
            }
        }
    }

    Vec2D getTargetPos() const { return targetPos; };
    void setTargetPos(Vec2D targetPos_) {
        targetPos = targetPos_;
        if (!map->walls[int(targetPos.x) + int(targetPos.y) * MAP_SIZE] &&
            !map->beings[int(targetPos.x) + int(targetPos.y) * MAP_SIZE]) {
            x = (int)targetPos.x;
            y = (int)targetPos.y;
            lastPos = targetPos;
            invalidMove = false;
        } else {
            invalidMove = true;
        }
    }
    Vec2D getPos() const { return pos; };

    Vec2D cam() const { return rotationM(PI / 2) * dir; }
};
