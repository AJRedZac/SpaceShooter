#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include "Projectile.h"

class PlayerShip {
public:
    PlayerShip(SDL_Texture* shipTex, int x, int y, int size, int texW = 0, int texH = 0);

    void update(float mouseX, float mouseY, float dt);
    void handle_input(const bool* keystate, float dt);
    void draw(SDL_Renderer* renderer);
    void setFireRate(float shotsPerSecond);
    void setFiring(bool on);
    void setBulletTexture(SDL_Texture* tex);
    const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const { return bullets; }

    float getX() const { return posX; }
    float getY() const { return posY; }
    float getAngle() const { return angle; }
    int getHP() const { return hp; }
    int getMaxHP() const { return maxHp; }
    
private:
    float posX, posY;
    int hp{100};
    int maxHp{100};
    float targetX = 0.0f, targetY = 0.0f;
    int size;
    float angle;
    float vx{0}, vy{0};
    SDL_Texture* tex{nullptr};

    std::vector<std::unique_ptr<Projectile>> bullets;
    SDL_Texture* bulletTex{nullptr};
    bool firing{false};
    float fireCd{0.0f};
    float fireInterval{1.0f};

    void spawnBullet(int winW, int winH);
    void updateBullets(float dt, int winW, int winH);
    void drawBullets(SDL_Renderer* r);

    int texW_ = 0;
    int texH_ = 0;
};