#pragma once

#include <SDL2/SDL.h>

#include <iostream>

#include "Color.h"

class FrameBuffer {
   public:
    FrameBuffer(SDL_Renderer* renderer, int width_ = 800, int height_ = 600)
        : width(width_), height(height_), area(width * height) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_STREAMING, width, height);
        buffer = new Color[area];
    }

    SDL_Texture* getTexture() { return texture; }

    void copyBuffer() {
        lock();
        memcpy(texturePixels, buffer, texturePitch * height);
        unlock();
    }

    bool isLocked() { return locked; }

    bool lock() {
        if (isLocked()) {
            std::cout << "Failed to lock already locked texture" << std::endl;
            return false;
        }
        // TODO nullptr?
        if (SDL_LockTexture(texture, nullptr, &texturePixels, &texturePitch) !=
            0) {
            std::cout << "Failed to lock texture: " << SDL_GetError()
                      << std::endl;
            return false;
        }
        locked = true;
        return true;
    }

    bool unlock() {
        if (!isLocked()) {
            std::cout << "Failed to unlock not locked texture" << std::endl;
            return false;
        }
        SDL_UnlockTexture(texture);
        texturePixels = nullptr;
        texturePitch = 0;
        locked = false;
        return true;
    }

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

   private:
    // Hardware texture
    SDL_Texture* texture;
    void* texturePixels = nullptr;
    int texturePitch;
    bool locked = false;
};
