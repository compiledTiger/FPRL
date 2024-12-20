#include <set>

#include "Being.h"
#include "Constants.h"
#include "Map.h"
#include "Renderer3D.h"
#include "Window.h"

int main(int argc, char* argv[]) {
    Window w;
    FrameBuffer fb(w.sdlRenderer);
    Renderer3D renderer;

    std::set<SDL_Keycode> pressedKeys;

    Being player(Vec2D(9.5, 5));
    player.rotate(PI);

    Map<16> map;
    map.generate();

    bool debug = false;
    bool quit = false;
    int frames = 0;
    auto lastMillis = SDL_GetTicks();
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

        double speed = 0.01;
        double rotationSpeed = PI / 80;
        if (pressedKeys.find(SDLK_LSHIFT) != pressedKeys.end()) speed = 0.02;
        if (pressedKeys.find(SDLK_h) != pressedKeys.end())
            player.vel -= rotationM(PI / 2) * player.dir * speed;
        if (pressedKeys.find(SDLK_j) != pressedKeys.end())
            player.vel -= player.dir * speed;
        if (pressedKeys.find(SDLK_k) != pressedKeys.end())
            player.vel += player.dir * speed;
        if (pressedKeys.find(SDLK_l) != pressedKeys.end())
            player.vel += rotationM(PI / 2) * player.dir * speed;
        if (pressedKeys.find(SDLK_q) != pressedKeys.end())
            player.rotate(-rotationSpeed);
        if (pressedKeys.find(SDLK_e) != pressedKeys.end())
            player.rotate(rotationSpeed);
        if (releasedKeys.find(SDLK_d) != releasedKeys.end()) debug = !debug;

        player.update(map);

        if (!debug) {
            renderer.drawView(player, map, fb);
        } else {
            renderer.drawDebug(player, map, fb);
        }

        fb.copyBuffer();
        SDL_SetRenderDrawColor(w.sdlRenderer, 0x20, 0xAA, 0x20, 0xFF);
        SDL_RenderClear(w.sdlRenderer);
        w.renderFrameBuffer(fb, 240, 60);
        SDL_RenderPresent(w.sdlRenderer);

        ++frames;
        int fpsPollInterval = 60;
        if (frames % fpsPollInterval == 0) {
            std::cout << "FPS: "
                      << int(double(fpsPollInterval) /
                             double(SDL_GetTicks() - lastMillis) * 1000.0)
                      << std::endl;
            lastMillis = SDL_GetTicks();
        }
    }

    return 0;
}
