#pragma once

class SDL_Renderer;

class FrameBuffer {
   public:
    FrameBuffer(SDL_Renderer* renderer, int width_ = 640, int height_ = 480)
        : width(width_),
          height(height_),
          pitch(width * bytesPerPixel),
          bufferSize(height * pitch) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_STREAMING, width, height);
        // RGBA8888 = 4 bytes = 4 chars
        // first alpha, then B G R
        buffer = new unsigned char[bufferSize];
    }
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
    const int bytesPerPixel = 4;
    const int width;
    const int height;
    const int pitch;
    const int bufferSize;
    unsigned char* buffer;

   private:
    // Hardware texture
    SDL_Texture* texture;
    void* texturePixels = nullptr;
    int texturePitch;
    bool locked = false;
};
