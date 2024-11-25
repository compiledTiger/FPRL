#include <set>

#include "Window.h"
#include "Mat2D.h"

int main(int argc, char* argv[]) {
    Window w;
    FrameBuffer fb(w.sdlRenderer);
    std::set<SDL_Keycode> pressedKeys;

    Vec2D pos (320, 240);
    Vec2D dir (0, -1);
    Vec2D vel (0, 0);

    // clang-format off
    char map[16*16] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    // clang-format on

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

        if (pressedKeys.find(SDLK_w) != pressedKeys.end()) vel+=dir;
        if (pressedKeys.find(SDLK_s) != pressedKeys.end()) vel-=dir;

        vel *= 0.9;
        pos += vel;

        // Clear buffer
        for (int i = 0; i < fb.bufferSize; ++i) fb.buffer[i] = 0;

        // Draw map
        for (int i = 0; i < 16 * 16; ++i) {
            for (int j = 0; j < 30; ++j) {
                auto* byte = fb.at((i % 16) * 30, (i / 16) * 30 + j);
                for (int k = 0; k < 30; ++k) {
                    *byte++ = 255;  // A
                    *byte++ = 0;    // B
                    *byte++ = 40;  // G
                    *byte++ = 120*map[i];  // R
                }
            }
        }

        // Draw player
        for (int i = 0; i < 10; ++i) {
            auto* byte = fb.at(pos.x, pos.y + i);
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
