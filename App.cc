#include <set>

#include "Being.h"
#include "Constants.h"
#include "Window.h"

int main(int argc, char* argv[]) {
    Window w;
    FrameBuffer fb(w.sdlRenderer);
    std::set<SDL_Keycode> pressedKeys;

    Being player(Vec2D(10, 5));

    // clang-format off
    char map[16*16] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 3, 2, 2, 1, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    // clang-format on

    bool debug = false;
    bool quit = false;
    while (!quit) {
        SDL_Event e;
        std::set<SDL_Keycode> releasedKeys;
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
                releasedKeys.insert(key);
            }
        }

        if (pressedKeys.find(SDLK_h) != pressedKeys.end())
            player.vel -= rotationM(PI / 2) * player.dir * 0.05;
        if (pressedKeys.find(SDLK_j) != pressedKeys.end())
            player.vel -= player.dir * 0.05;
        if (pressedKeys.find(SDLK_k) != pressedKeys.end())
            player.vel += player.dir * 0.05;
        if (pressedKeys.find(SDLK_l) != pressedKeys.end())
            player.vel += rotationM(PI / 2) * player.dir * 0.05;
        if (pressedKeys.find(SDLK_q) != pressedKeys.end())
            player.rotate(-PI / 60);
        if (pressedKeys.find(SDLK_e) != pressedKeys.end())
            player.rotate(PI / 60);
        if (pressedKeys.find(SDLK_e) != pressedKeys.end())
            player.rotate(PI / 60);
        if (releasedKeys.find(SDLK_d) != releasedKeys.end()) debug = !debug;

        player.update();

        // Clear buffer
        for (int i = 0; i < fb.bufferSize; ++i) fb.buffer[i] = 0;

        if (!debug) {
            player.drawView(fb, &map[0]);
        } else {
            player.drawDebug(fb, &map[0]);
        }

        fb.copyBuffer();
        SDL_SetRenderDrawColor(w.sdlRenderer, 0x20, 0xAA, 0x20, 0xFF);
        SDL_RenderClear(w.sdlRenderer);
        w.renderFrameBuffer(fb, 80, 60);
        SDL_RenderPresent(w.sdlRenderer);
    }

    return 0;
}
