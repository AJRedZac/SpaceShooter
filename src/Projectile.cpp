#include "Projectile.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Projectile::Projectile(SDL_Texture* tex,
                       float x, float y,
                       float angle_rad,
                       float speed_px_s,
                       int damage,
                       int size_px,
                       int winW, int winH,
                       float maxAge_s)
: tex_(tex), x_(x), y_(y),
  angle_(angle_rad), speed_(speed_px_s),
  dmg_(damage), size_(size_px),
  alive_(true), age_(0.0f), maxAge_(maxAge_s),
  winW_(winW), winH_(winH) {}

void Projectile::update(float dt) {
    if (!alive_) return;
    // Avanza
    x_ += std::cos(angle_) * speed_ * dt;
    y_ += std::sin(angle_) * speed_ * dt;

    // Envejece
    age_ += dt;
    if (age_ >= maxAge_) { alive_ = false; return; }

    // Auto-destruir si sale de pantalla (con margen)
    const float m = (float)size_;
    if (x_ < -m || x_ > (float)winW_ + m || y_ < -m || y_ > (float)winH_ + m) {
        alive_ = false;
    }
}

void Projectile::draw(SDL_Renderer* r) {
    if (!alive_) return;

    // Rect destino centrado
    const float h = (float)size_;
    float w = h;
    // Si quieres respetar proporción de la textura, cámbialo:
    // (SDL_GetTextureSize si lo tienes disponible)
    SDL_FRect dst{ x_ - w*0.5f, y_ - h*0.5f, w, h };

    // Intento con rotación (si falla, render simple)
    const double deg = angle_ * (180.0 / M_PI);
    const SDL_FPoint pivot{ w * 0.5f, h * 0.5f };
    if (tex_ && SDL_RenderTextureRotated(r, tex_, nullptr, &dst, deg, &pivot, SDL_FLIP_NONE) == 0) {
        return;
    }
    if (tex_) {
        SDL_RenderTexture(r, tex_, nullptr, &dst);
    } else {
        // Fallback: dibuja un cuadrito blanco si no hay textura
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_RenderRect(r, &dst);
    }
}

SDL_FRect Projectile::bounds() const {
    return SDL_FRect{ x_ - size_*0.5f, y_ - size_*0.5f, (float)size_, (float)size_ };
}
