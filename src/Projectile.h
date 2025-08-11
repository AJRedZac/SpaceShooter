#pragma once
#include <SDL3/SDL.h>
#include <memory>

class Projectile {
public:
    virtual ~Projectile() = default;

    // Constructor base (úsalo desde subclases o directamente)
    Projectile(SDL_Texture* tex,
               float x, float y,
               float angle_rad,
               float speed_px_s,
               int damage,
               int size_px,           // diámetro/alto visual aprox
               int winW, int winH,
               float maxAge_s = 3.0f  // vida útil máxima
               
    );

    // Actualiza posición/vida. dt en segundos.
    virtual void update(float dt);

    // Dibujo (intenta rotar; si no, dibuja sin rotación)
    virtual void draw(SDL_Renderer* r);

    // AABB para colisiones simples (puedes cambiar a círculo si prefieres)
    virtual SDL_FRect bounds() const;

    // Señales de estado
    bool alive()  const { return alive_; }
    int  damage() const { return dmg_; }
    void kill()         { alive_ = false; }

protected:
    SDL_Texture* tex_{nullptr};
    float x_, y_;          // centro
    float angle_;          // rad
    float speed_;          // px/s
    int   dmg_{1};
    int   size_{8};        // alto/diámetro aprox
    bool  alive_{true};
    float age_{0.0f}, maxAge_{3.0f};
    int   winW_, winH_;
    
    
};

/// Ejemplo de subclase: bala básica (si luego quieres balas con humo, misiles, etc.)
class BasicBullet : public Projectile {
public:
    using Projectile::Projectile; // hereda el ctor
};
