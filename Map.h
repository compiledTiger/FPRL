#pragma once

#include "Texture.h"

template <int length_>
struct Map {
    static const int length = length_;
    Texture<32, 32>* walls[length * length];
    Texture<32, 32>* floor[length * length];
    // walls() sets values to nullptr
    Map() : walls(), floor() {}
    Texture<32, 32>*& operator[](int i) { return walls[i]; }
    Texture<32, 32>*& at(int x, int y) { return walls[y * length + x]; }
    void generate() {
        // TODO destroy
        auto* wall = new Texture<32, 32>();
        wall->generateWall();
        for (int w = 0; w < length; ++w) {
            at(w, 0) = wall;
        }
        for (int w = 0; w < length; ++w) {
            if (w != 9) at(w, 7) = wall;
        }
        for (int w = 0; w < length; ++w) {
            if (w != 9) at(w, 10) = wall;
        }
        for (int w = 0; w < length; ++w) {
            at(w, length - 1) = wall;
        }
        for (int h = 0; h < length; ++h) {
            at(0, h) = wall;
        }
        for (int h = 0; h < length; ++h) {
            if (h < 7 || h > 10) at(8, h) = wall;
        }
        for (int h = 0; h < length; ++h) {
            if (h < 7 || h > 10) at(10, h) = wall;
        }
        for (int h = 0; h < length; ++h) {
            at(length - 1, h) = wall;
        }

        // TODO destroy
        auto* f = new Texture<32, 32>();
        f->generateFloor();
        for (int h = 0; h < length; ++h) {
            for (int w = 0; w < length; ++w) {
                floor[h * length + w] = f;
            }
        }
    }
};
