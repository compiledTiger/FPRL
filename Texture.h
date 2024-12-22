#pragma once

#include "Color.h"

template <int width_, int height_>
struct Texture {
    static const int width = width_;
    static const int height = height_;
    Color pixels[width * height];
    // pixels() sets values to nullptr
    Texture() : pixels() {}
    Color& at(int x, int y) { return pixels[y * width + x]; }
    void generateWall() {
        for (auto& p : pixels) p = Color(100, 100, 40);

        for (int h = 0; h < height; h += 4) {
            for (int w = 0; w < width; ++w) {
                at(w, h) = Color(42, 42, 42);
            }
        }
        bool offset = false;
        for (int w = 0; w < width; w += 4) {
            int h = 0 + offset * 4;
            while (h < height) {
                at(w, h) = Color(42, 42, 42);
                if ((h + 1) % 4)
                    ++h;
                else
                    h += 5;
            }
            offset = !offset;
        }
    }
    void generateFloor() {
        for (auto& p : pixels) p = Color(100, 100, 40);

        for (int h = 0; h < height; h += 8) {
            for (int w = 0; w < width; ++w) {
                at(w, h) = Color(42, 42, 42);
            }
        }
        bool offset = false;
        for (int w = 0; w < width; w += 4) {
            int h = 0 + offset * 8;
            while (h < height) {
                at(w, h) = Color(42, 42, 42);
                if ((h + 1) % 8)
                    ++h;
                else
                    h += 9;
            }
            offset = !offset;
        }
    }
};
