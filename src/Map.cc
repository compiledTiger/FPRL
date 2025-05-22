#include "Map.h"

#include "Being.h"

// TODO
template <>
Vec2D Map<MAP_SIZE>::generate() {
    Texture<32, 32>* stone = &textures[textureMap["stone"]];

    for (int y = 0; y < length; ++y) {
        for (int x = 0; x < length; ++x) {
            at(x, y) = stone;
            floor[y * length + x] = stone;
            ceiling[y * length + x] = stone;
            darknessLevel[y * length + x] = 40;
        }
    }

    srand(time(0));
    Vec2D mid1 = genRoom();
    Vec2D mid2 = genRoom();
    Vec2D mid3 = genRoom();
    Vec2D mid4 = genRoom();
    Vec2D mid5 = genRoom();
    Vec2D mid6 = genRoom();
    connectRooms(mid1, mid2);
    connectRooms(mid1, mid3);
    connectRooms(mid1, mid4);
    connectRooms(mid3, mid4);
    connectRooms(mid2, mid5);
    connectRooms(mid5, mid6);
    // TODO player seems to be able to spawn on shop tiles!
    return mid1;
}

template <int length_>
Vec2D Map<length_>::genRoom() {
    Texture<32, 32>* shop = &textures[textureMap["shop"]];
    Texture<32, 32>* fungus = &textures[textureMap["fungus"]];

    int roomWidth = (rand() % 8) + 2;
    int roomHeight = (rand() % 8) + 2;
    int roomNWCornerX = rand() % (length - roomWidth - 1) + 1;
    int roomNWCornerY = rand() % (length - roomHeight - 1) + 1;
    int dl = 40;
    for (int x = roomNWCornerX; x < roomNWCornerX + roomWidth; ++x) {
        for (int y = roomNWCornerY; y < roomNWCornerY + roomHeight; ++y) {
            darknessLevel[y * length + x] = dl;
            at(x, y) = nullptr;
            if (y == roomNWCornerY && x - roomNWCornerX == 7)
                at(x, y - 1) = shop;
            if (x == roomNWCornerX && y - roomNWCornerY == 3)
                at(x - 1, y) = shop;
            if (x - roomNWCornerX == 3 && y - roomNWCornerY == 2)
                // TODO place in a container
                new Being(this, x, y, fungus);
        }
        dl -= 4;
        if (dl < 0) dl = 0;
    }
    return Vec2D(roomNWCornerX + roomWidth / 2, roomNWCornerY + roomHeight / 2);
}

template <int length_>
void Map<length_>::connectRooms(Vec2D mid1, Vec2D mid2) {
    int xDir = mid1.x > mid2.x ? -1 : 1;
    int yDir = mid1.y > mid2.y ? -1 : 1;
    int corX = mid1.x;
    int corY = mid1.y;
    while (corX != mid2.x) {
        corX += xDir;
        at(corX, corY) = nullptr;
    }
    while (corY != mid2.y) {
        corY += yDir;
        at(corX, corY) = nullptr;
    }
}
