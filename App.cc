#include "Being.h"
#include "Constants.h"
#include "Map.h"
#include "Renderer3D.h"
#include "SDLWindow.h"

int main(int argc, char* argv[]) {
    SDLWindow w(1500, 900, 480, 360);
    FrameBuffer fb(w.RENDER_WIDTH, w.RENDER_HEIGHT);

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
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                    break;
                }
            }

            double speed = 0.01;
            double rotationSpeed = PI / 80;

            const Uint8* keyboard = SDL_GetKeyboardState(nullptr);
            if (keyboard[SDL_SCANCODE_LSHIFT]) speed = 0.02;
            if (keyboard[SDL_SCANCODE_H])
                player.vel -= rotationM(PI / 2) * player.dir * speed;
            if (keyboard[SDL_SCANCODE_J]) player.vel -= player.dir * speed;
            if (keyboard[SDL_SCANCODE_K]) player.vel += player.dir * speed;
            if (keyboard[SDL_SCANCODE_L])
                player.vel += rotationM(PI / 2) * player.dir * speed;
            if (keyboard[SDL_SCANCODE_Q]) player.rotate(-rotationSpeed);
            if (keyboard[SDL_SCANCODE_E]) player.rotate(rotationSpeed);
            quit = quit || keyboard[SDL_SCANCODE_ESCAPE];
            debug = keyboard[SDL_SCANCODE_D];

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

            w.drawFrameBuffer(fb, 240, 60);

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
