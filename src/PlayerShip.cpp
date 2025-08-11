// PlayerShip.cpp
#include "PlayerShip.h"
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 (M_PI / 2.0)
#endif

PlayerShip::PlayerShip(SDL_Texture* shipTex, int x, int y, int size, int texW, int texH)
    : posX((float)x), posY((float)y), size(size), angle(0.0f), tex(shipTex), texW_(texW), texH_(texH) {
    if ((texW_ == 0 || texH_ == 0) && tex) {
        float w = 0.0f, h = 0.0f;
        SDL_GetTextureSize(tex, &w, &h);
        texW_ = static_cast<int>(w);
        texH_ = static_cast<int>(h);
    }
}

void PlayerShip::setBulletTexture(SDL_Texture* tex) { bulletTex = tex; }
void PlayerShip::setFiring(bool on) { firing = on; }

void PlayerShip::update(float mouseX, float mouseY, float dt) {
    targetX = mouseX;
    targetY = mouseY;
    angle = std::atan2(mouseY - posY, mouseX - posX) + M_PI_2;

    fireCd -= dt;
    if (firing && fireCd <= 0.0f && bulletTex) {
        spawnBullet(1920, 1080);
        fireCd = fireInterval;
    }

    updateBullets(dt, 1920, 1080);
}

void PlayerShip::handle_input(const bool* keystate, float dt) {
    const float speed = 300.0f;
    vx = vy = 0.0f;

    float dx = targetX - posX;
    float dy = targetY - posY;
    float distance = std::sqrt(dx * dx + dy * dy);

    float dirX = dx / distance;
    float dirY = dy / distance;

    if (keystate[SDL_SCANCODE_W] && distance > (size + 2.0f)) {
        vx = dirX * speed * dt;
        vy = dirY * speed * dt;
        posX += vx;
        posY += vy;
    }

    if (keystate[SDL_SCANCODE_S]) {
        vx = -dirX * speed * dt;
        vy = -dirY * speed * dt;
        posX += vx;
        posY += vy;
    }
}

void PlayerShip::draw(SDL_Renderer* r) {
    if (tex) {
        const float H = (float)(size * 2);
        float W = H;
        if (texW_ > 0 && texH_ > 0) {
            W = H * (float)texW_ / (float)texH_;
        } else {
            float w = 0.0f, h = 0.0f;
            if (SDL_GetTextureSize(tex, &w, &h)) {
                texW_ = static_cast<int>(w); texH_ = static_cast<int>(h);
                W = H * (float)texW_ / (float)texH_;
            }
        }

        SDL_FRect dst{ posX - W * 0.5f, posY - H * 0.5f, W, H };
        const double deg = angle * (180 / M_PI);
        const SDL_FPoint pivot{ W * 0.5f, H * 0.5f };

        int renderResult = SDL_RenderTextureRotated(r, tex, nullptr, &dst, deg, &pivot, SDL_FLIP_NONE);
        if (renderResult != 0 && SDL_GetError() != nullptr && *SDL_GetError() != '\0') {
            SDL_RenderTexture(r, tex, nullptr, &dst);
        }
    }

    drawBullets(r);
}

void PlayerShip::spawnBullet(int winW, int winH) {
    bullets.emplace_back(std::make_unique<BasicBullet>(
        bulletTex, posX, posY, angle - M_PI_2,
        900.0f, 1, 8, winW, winH, 2.0f
    ));
}

void PlayerShip::updateBullets(float dt, int winW, int winH) {
    for (auto& b : bullets) b->update(dt);
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const auto& p){ return !p->alive(); }), bullets.end());
}

void PlayerShip::drawBullets(SDL_Renderer* r) {
    for (auto& b : bullets) b->draw(r);
}

void PlayerShip::setFireRate(float shotsPerSecond) {
    if (shotsPerSecond > 0.0f)
        fireInterval = 1.0f / shotsPerSecond;
}
