#pragma once

#include "Texture.h"
#include "Vec2D.h"

template <int length_>
struct Map {
    static const int length = length_;
    Texture<32, 32>* walls[length * length];
    Texture<32, 32>* floor[length * length];
    Texture<32, 32>* ceiling[length * length];
    int darknessLevel[length * length];
    // walls() sets values to nullptr
    Map() : walls(), floor(), ceiling(), darknessLevel() {}
    Texture<32, 32>*& operator[](int i) { return walls[i]; }
    void generate() {
        // TODO destroy
        auto* wall02 = new Texture<32, 32>();
        wall02->loadBmp("assets/rltiles/dc-dngn/dngn_rock_wall_02.bmp");
        auto* wall07 = new Texture<32, 32>();
        wall07->loadBmp("assets/rltiles/dc-dngn/dngn_rock_wall_07.bmp");
        auto* wall06 = new Texture<32, 32>();
        wall06->loadBmp("assets/rltiles/dc-dngn/dngn_rock_wall_06.bmp");
        auto* shop = new Texture<32, 32>();
        shop->loadBmp("assets/rltiles/dc-dngn/dngn_enter_shop.bmp");
        for (int w = 0; w < length; ++w) {
            at(w, 0) = wall02;
            if (w < 9 || w > 10) {
                at(w, 7) = wall02;
                at(w, 14) = wall02;
            }
            if (w > 10) at(w, 10) = wall02;

            at(w, length - 1) = wall02;
        }
        for (int h = 0; h < length; ++h) {
            at(0, h) = wall07;
            if (h < 7 || h > 13) {
                at(8, h) = wall02;
                at(11, h) = wall02;
            }
            at(length - 1, h) = wall02;
        }
        at(3, 10) = wall06;
        at(0, 9) = shop;

        // TODO destroy
        auto* f = new Texture<32, 32>();
        f->loadBmp("assets/rltiles/nh-dngn/lit_corridor.bmp");
        auto* c = new Texture<32, 32>();
        c->loadBmp("assets/rltiles/nh-dngn/corridor_light.bmp");
        for (int h = 0; h < length; ++h) {
            for (int w = 0; w < length; ++w) {
                floor[h * length + w] = f;
                ceiling[h * length + w] = c;
                darknessLevel[h * length + w] = 50;
            }
        }
        darknessLevel[8 * length + 3] = 20;
        darknessLevel[8 * length + 2] = 30;
        darknessLevel[8 * length + 4] = 30;
        darknessLevel[9 * length + 3] = 30;
        darknessLevel[9 * length + 2] = 35;
        darknessLevel[9 * length + 4] = 35;
        darknessLevel[8 * length + 1] = 35;
        darknessLevel[8 * length + 5] = 35;
        darknessLevel[9 * length + 1] = 40;
        darknessLevel[9 * length + 5] = 40;
        darknessLevel[7 * length + 3] = 30;
        darknessLevel[7 * length + 2] = 35;
        darknessLevel[7 * length + 4] = 35;
        darknessLevel[7 * length + 1] = 40;
        darknessLevel[7 * length + 5] = 40;
        darknessLevel[7 * length + 0] = 45;
        darknessLevel[8 * length + 0] = 40;
        darknessLevel[9 * length + 0] = 45;
        darknessLevel[7 * length + 6] = 45;
        darknessLevel[8 * length + 6] = 40;
        darknessLevel[9 * length + 6] = 45;
        darknessLevel[7 * length + 7] = 47;
        darknessLevel[8 * length + 7] = 45;
        darknessLevel[9 * length + 7] = 47;
        double darknessFactor = 1.0;
        for (int h = 0; h < length; ++h) {
            for (int w = 0; w < length; ++w) {
                darknessLevel[h * length + w] *= darknessFactor;
            }
        }
    }

   private:
    Texture<32, 32>*& at(int x, int y) { return walls[y * length + x]; }
};
