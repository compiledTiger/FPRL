#pragma once

#include <cstdlib>
#include <iostream>
#include <optional>

#include "Constants.h"
#include "Map.h"
#include "Mat2D.h"
#include "Vec2D.h"

struct Being {
    Vec2D pos;
    Vec2D lastPos;
    Vec2D dir = Vec2D(0, -1);
    Vec2D targetDir;
    bool moving = false;
    bool rotating = false;

    Being(const Vec2D& pos_)
        : pos(pos_),
          lastPos(pos),
          targetPos(pos),
          dir(Vec2D(0, -1)),
          targetDir(dir) {}

    void update(Map<MAP_SIZE>& map) {
        if (map[int(targetPos.x) + int(targetPos.y) * MAP_SIZE] &&
            (pos - lastPos).mag() > 0.3)
            targetPos = lastPos;

        Vec2D posDiff = (targetPos - pos);
        moving = posDiff.mag() > 0.25;
        if (moving)
            pos += posDiff / 8;
        else {
            pos += posDiff / 4;
            if (posDiff.mag() < 0.001) pos = targetPos;
        }

        if ((targetDir - dir).mag()) {
            double radDiff = atan2(targetDir.det(dir), targetDir.dot(dir));
            rotating = abs(radDiff) > 0.1;
            if (rotating)
                dir = rotationM(-radDiff / 8) * dir;
            else {
                dir = rotationM(-radDiff / 4) * dir;
                if (abs(radDiff) < 0.001) dir = targetDir;
            }
        }
    }

    Vec2D getTargetPos() { return targetPos; };
    void setTargetPos(Vec2D newTargetPos) {
        lastPos = targetPos;
        targetPos = newTargetPos;
    }

    Vec2D cam() const { return rotationM(PI / 2) * dir; }

   private:
    Vec2D targetPos;
};
