#include <set>

#include "Window.h"

int main(int argc, char* argv[]) {
    Window w;
    FrameBuffer fb(w.sdlRenderer);
    std::set<SDL_Keycode> pressedKeys;

    int x = 320;
    int y = 240;

    bool quit = false;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_KEYDOWN) {
                auto key = e.key.keysym.sym;
                pressedKeys.insert(key);
                if (key == SDLK_ESCAPE) quit = true;
            } else if (e.type == SDL_KEYUP) {
                auto key = e.key.keysym.sym;
                pressedKeys.erase(key);
            }
        }

        if (pressedKeys.find(SDLK_w) != pressedKeys.end()) --y;
        if (pressedKeys.find(SDLK_a) != pressedKeys.end()) --x;
        if (pressedKeys.find(SDLK_s) != pressedKeys.end()) ++y;
        if (pressedKeys.find(SDLK_d) != pressedKeys.end()) ++x;

        for (int i = 0; i < fb.bufferSize; ++i) fb.buffer[i] = 0;

        for (int i = 0; i < 10; ++i) {
            auto* byte = fb.at(x, y + i);
            for (int j = 0; j < 10; ++j) {
                *byte++ = 255;  // A
                *byte++ = 255;  // B
                *byte++ = 0;    // G
                *byte++ = 0;    // R
            }
        }

        fb.copyBuffer();
        SDL_SetRenderDrawColor(w.sdlRenderer, 0x20, 0xAA, 0x20, 0xFF);
        SDL_RenderClear(w.sdlRenderer);
        w.renderFrameBuffer(fb, 80, 60);
        SDL_RenderPresent(w.sdlRenderer);
    }

    return 0;
}
