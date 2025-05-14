#pragma once

#include <SDL2/SDL.h>

#include <iostream>

#include "Color.h"

class FrameBuffer {
   public:
    FrameBuffer(int width_, int height_)
        : width(width_),
          height(height_),
          area(width * height),
          buffer(new Color[area]) {}

    void drawPixel(int x, int y, Color c) {
        drawPixel((buffer + x + y * width), c);
    }

    void drawPixel(int i, Color c) { drawPixel((buffer + i), c); }

    void drawPixel(Color* p, Color c) { *p = c; }

    void drawRect(int x, int y, int w, int h, Color c, bool centered = false) {
        if (centered) {
            x -= w / 2;
            y -= h / 2;
        }
        for (int row = 0; row < h; ++row) {
            for (int column = 0; column < w; ++column) {
                drawPixel(x + column, y + row, c);
            }
        }
    }

    const int width;
    const int height;
    const int area;
    Color* buffer;
};
