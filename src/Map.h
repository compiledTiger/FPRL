#pragma once

#include <unordered_map>
#include <vector>

#include "Texture.h"
#include "Vec2D.h"

class Being;

template <int length_>
struct Map {
    static const int length = length_;
    Texture<32, 32>* walls[length * length];
    Texture<32, 32>* floor[length * length];
    Texture<32, 32>* ceiling[length * length];
    Being* beings[length * length];
    int darknessLevel[length * length];

    std::vector<Texture<32, 32>> wallTextures;
    std::vector<Texture<20, 20>> beingTextures;
    std::unordered_map<std::string, int> textureMap;

    // walls() sets values to nullptr
    Map() : walls(), floor(), ceiling(), beings(), darknessLevel() {
        // TODO destroy
        loadWallTexture("assets/rltiles/nh-dngn/floor_of_a_room.bmp", "stone");
        loadWallTexture("assets/rltiles/dc-dngn/dngn_enter_shop.bmp", "shop");
        loadBeingTexture("assets/wee/bmps/gobwar.bmp", "gobwar");
    }
    Texture<32, 32>*& operator[](int i) { return walls[i]; }

    void loadWallTexture(const std::string path, const std::string name) {
        Texture<32, 32> t;
        t.loadBmp(path.c_str());
        wallTextures.push_back(t);
        textureMap[name] = wallTextures.size() - 1;
    }

    void loadBeingTexture(const std::string path, const std::string name) {
        Texture<20, 20> t;
        t.loadBmp(path.c_str());
        beingTextures.push_back(t);
        textureMap[name] = beingTextures.size() - 1;
    }

    Vec2D generate();

    Vec2D genRoom();

    void connectRooms(Vec2D mid1, Vec2D mid2);

   private:
    Texture<32, 32>*& at(int x, int y) { return walls[y * length + x]; }
};
