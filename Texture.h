#pragma once

#include "Color.h"

template <int width_, int height_>
struct Texture {
    static const int width = width_;
    static const int height = height_;
    static const int area = width * height;
    static const int darknessLevels = 64;
    static const int nPixels = darknessLevels * area;
    Color pixels[nPixels];

    // pixels() sets values to nullptr
    Texture() : pixels() {}

    Color& at(int x, int y, int darknessLevel = 0) {
        return pixels[darknessLevel * area + y * width + x];
    }

    void calcDarknessLevels() {
        for (int darknessLevel = 1; darknessLevel < darknessLevels;
             ++darknessLevel) {
            for (int i = 0; i < area; ++i) {
                pixels[darknessLevel * area + i] =
                    pixels[i] * (double(darknessLevels - darknessLevel) /
                                 double(darknessLevels));
            }
        }
    }

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
        calcDarknessLevels();
    }
    void generateFloor(Color c) {
        for (auto& p : pixels) p = c;

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
        calcDarknessLevels();
    }
};
