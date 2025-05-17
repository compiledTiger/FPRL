#pragma once

#include <SDL2/SDL.h>

#include "FrameBuffer.h"

struct SDLWindow {
    const int WINDOW_WIDTH;
    const int WINDOW_HEIGHT;

    const int RENDER_WIDTH;
    const int RENDER_HEIGHT;

    const int SCALING;

    SDLWindow(int window_width, int window_height, int render_width,
              int render_height, int scaling)
        : WINDOW_WIDTH(window_width),
          WINDOW_HEIGHT(window_height),
          RENDER_WIDTH(render_width),
          RENDER_HEIGHT(render_height),
          SCALING(scaling) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "SDL_INIT_VIDEO failed" << std::endl;
            return;
        }

        sdlWindow = SDL_CreateWindow("RPG", SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                                     WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (!sdlWindow) {
            std::cout << "Failed to create SDL window: " << SDL_GetError()
                      << std::endl;
            return;
        }

        // TODO -1?
        sdlRenderer =
            SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        if (!sdlRenderer) {
            std::cout << "Failed to create SDL renderer: " << SDL_GetError()
                      << std::endl;
            return;
        }

        clearRenderer();

        sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       RENDER_WIDTH, RENDER_HEIGHT);
        if (!sdlTexture) {
            std::cout << "Failed to create SDL texture: " << SDL_GetError()
                      << std::endl;
            return;
        }
    }

    void drawFrameBuffer(const FrameBuffer& fb, int x, int y) {
        clearRenderer();
        copyFrameBuffer(fb);
        SDL_Rect destRect = {x, y, fb.width * SCALING, fb.height * SCALING};
        SDL_RenderCopy(sdlRenderer, sdlTexture, nullptr, &destRect);
        SDL_RenderPresent(sdlRenderer);
    }

    void clearRenderer() {
        SDL_SetRenderDrawColor(sdlRenderer, 0x20, 0xAA, 0x20, 0xFF);
        SDL_RenderClear(sdlRenderer);
    }

    const Uint8* getKeyboardState() { return SDL_GetKeyboardState(nullptr); }

    ~SDLWindow() {
        SDL_DestroyRenderer(sdlRenderer);
        SDL_DestroyWindow(sdlWindow);
        SDL_DestroyTexture(sdlTexture);
    }

   private:
    SDL_Window* sdlWindow = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;
    // Harware texture
    SDL_Texture* sdlTexture = nullptr;

    void copyFrameBuffer(const FrameBuffer& fb) {
        void* texturePixels;
        int texturePitch;
        if (SDL_LockTexture(sdlTexture, nullptr, &texturePixels,
                            &texturePitch) != 0) {
            std::cout << "Failed to lock texture: " << SDL_GetError()
                      << std::endl;
            return;
        }
        memcpy(texturePixels, fb.buffer, texturePitch * RENDER_HEIGHT);
        SDL_UnlockTexture(sdlTexture);
    }
};
