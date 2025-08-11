#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <cmath>
#include "PlayerShip.h"

// Debug opcional: un circulito donde está el mouse
static void draw_circle(SDL_Renderer* r, int cx, int cy, int rad) {
    for (int y = -rad; y <= rad; ++y)
        for (int x = -rad; x <= rad; ++x)
            if (x*x + y*y <= rad*rad) SDL_RenderPoint(r, cx + x, cy + y);
}



int main(int, char**) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init fallo: " << SDL_GetError() << "\n";
        return 1;
    }

    const int WIN_W = 1920, WIN_H = 1080;
    SDL_Window* window = SDL_CreateWindow("Space Shooter", WIN_W, WIN_H,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (!window) { std::cerr << SDL_GetError() << "\n"; SDL_Quit(); return 1; }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) { std::cerr << SDL_GetError() << "\n"; SDL_DestroyWindow(window); SDL_Quit(); return 1; }

    // Cargar sprite de la nave (ejecutando desde bin/ → subir un nivel)
    SDL_Texture* shipTex = IMG_LoadTexture(renderer, "../sprites/ship.png");
    if (!shipTex) shipTex = IMG_LoadTexture(renderer, "sprites/ship.png");
    if (!shipTex) {
        const char* base = SDL_GetBasePath();
        std::cerr << "No cargo ship.png\nBasePath: " << (base ? base : "(null)") << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Cargar sprite de la bala
    SDL_Texture* bulletTex = IMG_LoadTexture(renderer, "../sprites/bullet.png");
    if (!bulletTex) bulletTex = IMG_LoadTexture(renderer, "sprites/bullet.png");
    if (!bulletTex) {
        const char* base = SDL_GetBasePath();
        std::cerr << "No cargo bullet.png\nBasePath: " << (base ? base : "(null)") << "\n";
        SDL_DestroyTexture(shipTex);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    PlayerShip player(shipTex, WIN_W/2, WIN_H/2, 32);
    player.setBulletTexture(bulletTex);

    bool running = true;
    SDL_Event ev;

    // Timing para dt
    Uint64 lastTicks = SDL_GetTicks();

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN && ev.button.button == SDL_BUTTON_LEFT)
                player.setFiring(true);
            if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP && ev.button.button == SDL_BUTTON_LEFT)
                player.setFiring(false);
        }

        // dt en segundos
        Uint64 now = SDL_GetTicks();
        float dt = (now - lastTicks) / 1000.0f;
        if (dt > 0.05f) dt = 0.05f; // clamp anti picos
        lastTicks = now;

        // Inputs de nave
        float mx, my;
        SDL_GetMouseState(&mx, &my);
        player.update(mx, my, dt);

        const bool* keys = SDL_GetKeyboardState(nullptr);
        player.handle_input(keys);

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Debug: mouse
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        draw_circle(renderer, (int)mx, (int)my, 8);

        player.draw(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyTexture(bulletTex);
    SDL_DestroyTexture(shipTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
