#pragma once

#include <SDL2/SDL.h>

#include <iostream>

#include "Color.h"

class FrameBuffer {
   public:
    FrameBuffer(SDL_Renderer* renderer, int width_ = 800, int height_ = 600)
        : width(width_),
          height(height_),
          pitch(width * bytesPerPixel),
          area(width * height),
          bufferSize(height * pitch) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_STREAMING, width, height);
        // RGBA8888 = 4 bytes = 4 chars
        // first alpha, then B G R
        buffer = new unsigned char[bufferSize];
    }
    unsigned char* at(int i) { return buffer + i * bytesPerPixel; }

    unsigned char* at(int x, int y) {
        return buffer + x * bytesPerPixel + y * pitch;
    }

    const unsigned char* at(int x, int y) const { return at(x, y); }

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
        if (x >= 0 && x < width && y >= 0 && y < height)
            drawPixel(at(x, y), c);
        else {
            std::cout << "Warning: out of bounds drawPixel(x, y)" << std::endl;
        }
    }

    void drawPixel(int i, Color c) {
        if (i >= 0 && i < area)
            drawPixel(at(i), c);
        else {
            std::cout << "Warning: out of bounds drawPixel(i)" << std::endl;
        }
    }

    void drawPixel(unsigned char* byte, Color c) {
        *byte++ = c.a;
        *byte++ = c.b;
        *byte++ = c.g;
        *byte++ = c.r;
    }

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

    const int bytesPerPixel = 4;
    const int width;
    const int height;
    const int pitch;
    const int area;
    const int bufferSize;
    unsigned char* buffer;

   private:
    // Hardware texture
    SDL_Texture* texture;
    void* texturePixels = nullptr;
    int texturePitch;
    bool locked = false;
};
