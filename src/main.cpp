#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 (M_PI / 2.0)
#endif


void draw_circle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void draw_triangle(SDL_Renderer* renderer, int cx, int cy, int size) {
    SDL_Point p1 = { cx, cy - size };           // Punto superior
    SDL_Point p2 = { cx - size, cy + size };    // Punto inferior izquierdo
    SDL_Point p3 = { cx + size, cy + size };    // Punto inferior derecho

    // Dibuja los lados del triángulo
    SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
    SDL_RenderLine(renderer, p2.x, p2.y, p3.x, p3.y);
    SDL_RenderLine(renderer, p3.x, p3.y, p1.x, p1.y);
}

void draw_filled_triangle(SDL_Renderer* renderer, SDL_Point p1, SDL_Point p2, SDL_Point p3) {
    // Ordenar puntos por Y (p1.y <= p2.y <= p3.y)
    if (p2.y < p1.y) std::swap(p1, p2);
    if (p3.y < p1.y) std::swap(p1, p3);
    if (p3.y < p2.y) std::swap(p2, p3);

    auto interpolate = [](int y0, int x0, int y1, int x1, int y) -> int {
        if (y1 == y0) return x0;
        return x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    };

    for (int y = p1.y; y <= p3.y; ++y) {
        bool second_half = y > p2.y || p2.y == p1.y;
        int segment_height = second_half ? p3.y - p2.y : p2.y - p1.y;
        if (segment_height == 0) continue;

        int y_start = second_half ? p2.y : p1.y;
        int xA = interpolate(p1.y, p1.x, p3.y, p3.x, y);
        int xB = second_half
                 ? interpolate(p2.y, p2.x, p3.y, p3.x, y)
                 : interpolate(p1.y, p1.x, p2.y, p2.x, y);

        if (xA > xB) std::swap(xA, xB);

        for (int x = xA; x <= xB; ++x) {
            SDL_RenderPoint(renderer, x, y);
        }
    }
}

SDL_Point rotate_point(SDL_Point p, SDL_Point center, float angle_rad) {
    
    float s = sin(angle_rad);
    float c = cos(angle_rad);

    // Trasladar punto al origen
    float x = p.x - center.x;
    float y = p.y - center.y;

    // Aplicar rotación
    float x_new = x * c - y * s;
    float y_new = x * s + y * c;

    // Volver a trasladar
    return { static_cast<int>(x_new + center.x), static_cast<int>(y_new + center.y) };
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "No se pudo inicializar SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Space Shooter",
        1920, 1080,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cerr << "No se pudo crear la ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "No se pudo crear el renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    

    

    // Bucle de eventos básico
    bool corriendo = true;
    SDL_Event evento;

    while (corriendo) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_EVENT_QUIT) {
                corriendo = false;
            }
        }

        float mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Negro fondo
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rojo círculo
        draw_circle(renderer, mouseX, mouseY, 8); // Dibuja círculo en el cursor

        // Triángulo amarillo en el centro de la pantalla
        // Coordenadas del centro de la pantalla
        SDL_Point center = { 1920 / 2, 1080 / 2 };
        
        // Calcular ángulo hacia el mouse
        float dx = mouseX - center.x;
        float dy = mouseY - center.y;
        //float angle = atan2(dy, dx); // en radianes
        float angle = atan2(dy, dx) + M_PI_2; // Agrega 90° (π/2 radianes)

        // Triángulo base (apuntando hacia arriba)
        int size = 40;
        SDL_Point p1 = { center.x, center.y - size };     // punta
        SDL_Point p2 = { center.x - size / 2, center.y + size }; // base izq
        SDL_Point p3 = { center.x + size / 2, center.y + size }; // base der
        
        // Rotar los puntos
        p1 = rotate_point(p1, center, angle);
        p2 = rotate_point(p2, center, angle);
        p3 = rotate_point(p3, center, angle);
        
        // Dibujar triángulo relleno apuntando al cursor
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        draw_filled_triangle(renderer, p1, p2, p3);


        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Esperar ~1 frame (60fps)
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
