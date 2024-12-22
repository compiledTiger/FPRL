#pragma once

#include <SDL2/SDL.h>

#include <iostream>

#include "FrameBuffer.h"

struct Window {
    int WINDOW_WIDTH = 1280;
    int WINDOW_HEIGHT = 720;

    SDL_Window* sdlWindow = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;

    bool initialized = false;

    Window() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "SDL_INIT_VIDEO" << std::endl;
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
        sdlRenderer = SDL_CreateRenderer(
            sdlWindow, -1,
            SDL_RENDERER_ACCELERATED);

        if (!sdlRenderer) {
            std::cout << "Failed to create SDL renderer: " << SDL_GetError()
                      << std::endl;
            return;
        }

        SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        initialized = true;
    }

    void renderFrameBuffer(FrameBuffer& fb, int x, int y) {
        SDL_Rect destRect = {x, y, fb.width*2, fb.height*2};
        // TODO nullptr?
        SDL_RenderCopy(sdlRenderer, fb.getTexture(), nullptr, &destRect);
    }

    ~Window() {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = nullptr;
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
    }
};
