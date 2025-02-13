#include <set>

#include "Being.h"
#include "Constants.h"
#include "Map.h"
#include "Renderer3D.h"
#include "Window.h"

int main(int argc, char* argv[]) {
    Window w;
    FrameBuffer fb(w.sdlRenderer);

    std::set<SDL_Keycode> pressedKeys;

    Being player(Vec2D(9.5, 5));
    player.rotate(PI);

    Map<16> map;
    map.generate();

    Renderer3D renderer(fb, map);

    bool debug = false;
    bool quit = false;
    long frames = 0;
    long ticks = 0;
    auto lastTickMillis = 0;
    int ticksPerSecond = 60;
    int tickTimeCompensation = 0;
    auto lastLoggedTickMillis = 0;
    auto lastLoggedRenderMillis = SDL_GetTicks();
    while (!quit) {
        if (double(SDL_GetTicks() - lastTickMillis) >=
            1000.0 / (ticksPerSecond + tickTimeCompensation)) {
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
            if (pressedKeys.find(SDLK_LSHIFT) != pressedKeys.end())
                speed = 0.02;
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

            ++ticks;
            if (ticks % ticksPerSecond == 0) {
                std::cout << "Ticks/Second: "
                          << int(double(ticksPerSecond) /
                                 double(SDL_GetTicks() - lastLoggedTickMillis) *
                                 1000.0)
                          << std::endl;
                lastLoggedTickMillis = SDL_GetTicks();
            }
            int tps =
                int(1.0 / double(SDL_GetTicks() - lastTickMillis) * 1000.0);
            if (tps < ticksPerSecond)
                ++tickTimeCompensation;
            else if (tps > ticksPerSecond + 5)
                --tickTimeCompensation;
            lastTickMillis = SDL_GetTicks();
        } else {
            if (!debug) {
                renderer.drawView(player);
            } else {
                renderer.drawDebug(player);
            }

            fb.copyBuffer();
            SDL_SetRenderDrawColor(w.sdlRenderer, 0x20, 0xAA, 0x20, 0xFF);
            SDL_RenderClear(w.sdlRenderer);
            w.renderFrameBuffer(fb, 240, 60);
            SDL_RenderPresent(w.sdlRenderer);

            ++frames;
            if (frames % ticksPerSecond == 0) {
                std::cout << "FPS: "
                          << int(double(ticksPerSecond) /
                                 double(SDL_GetTicks() -
                                        lastLoggedRenderMillis) *
                                 1000.0)
                          << std::endl;
                lastLoggedRenderMillis = SDL_GetTicks();
            }
        }
    }

    return 0;
}
