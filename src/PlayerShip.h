#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include "Projectile.h"

class PlayerShip {
public:
    PlayerShip(SDL_Texture* shipTex, int x, int y, int size);

    void update(float mouseX, float mouseY, float dt);
    void handle_input(const bool* keystate);
    void draw(SDL_Renderer* renderer);

    // Disparo
    void setFiring(bool on);            // mouse down/up
    void setBulletTexture(SDL_Texture* tex);  // asigna textura de bala

    // Exponer proyectiles para colisiones externas (solo lectura)
    const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const { return bullets; }

    // Utilidades
    int   getX() const { return center.x; }
    int   getY() const { return center.y; }
    float getAngle() const { return angle; }

private:
    // Nave
    SDL_Point center;
    int size;
    float angle;
    float vx{0}, vy{0};
    SDL_Texture* tex{nullptr};

    // Proyectiles
    std::vector<std::unique_ptr<Projectile>> bullets;
    SDL_Texture* bulletTex{nullptr};
    bool firing{false};
    float fireCd{0.0f};             // cooldown actual
    float fireInterval{0.10f};      // seg entre disparos (10/s)

    // Helpers
    void spawnBullet(int winW, int winH);
    void updateBullets(float dt, int winW, int winH);
    void drawBullets(SDL_Renderer* r);
};
