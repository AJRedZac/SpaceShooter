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
    : posX((float)x), posY((float)y), size(size), angle(0.0f), tex(shipTex),
      texW_(texW), texH_(texH) {
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
    // apuntar
    float dx = mouseX - posX;
    float dy = mouseY - posY;
    angle = std::atan2(mouseY - posY, mouseX - posX) + M_PI_2;



    // disparo
    fireCd -= dt;
    if (firing && fireCd <= 0.0f && bulletTex) {
        // suponiendo ventana fija por ahora; puedes pasarla como argumento si cambia
        int winW = 1920, winH = 1080;
        spawnBullet(winW, winH);
        fireCd = fireInterval;
    }

    // actualizar balas
    updateBullets(dt, 1920, 1080);
}

void PlayerShip::handle_input(const bool* keystate,  float dt) {
    const float speed = 300.0f; // velocidad en píxeles por segundo
    vx = vy = 0.0f;

    
    if (keystate[SDL_SCANCODE_S]) {
        vx += -std::sin(angle) * speed * dt;
        vy +=  std::cos(angle) * speed * dt;
    }
    if (keystate[SDL_SCANCODE_W]) {
        vx -= -std::sin(angle) * speed * dt;
        vy -=  std::cos(angle) * speed * dt;
    }

    posX += vx;
    posY += vy;

    posX = (int)std::round(posX);
    posY = (int)std::round(posY);


}

void PlayerShip::draw(SDL_Renderer* r) {
    // 1) Dibuja la nave
    if (tex) {
        // Alto visible = size*2
        const float H = (float)(size * 2);
        float W = H;

        // Si conoces las dimensiones del sprite, conserva proporción.
        // Asegúrate de haber llenado texW/texH en el ctor (o aquí).
        if (texW_ > 0 && texH_ > 0) {
            W = H * (float)texW_ / (float)texH_;
        } else {
            // Fallback: intenta consultarlas (si tu build lo soporta)
            float w = 0.0f, h = 0.0f;
            if (SDL_GetTextureSize(tex, &w, &h)) {
                texW_ = static_cast<int>(w); texH_ = static_cast<int>(h);
                W = H * (float)texW_ / (float)texH_;
            }
        }

        SDL_FRect dst{ posX - W * 0.5f,
                       posY - H * 0.5f,
                       W, H };

        const double deg = angle * (180 / M_PI);
        const SDL_FPoint pivot{ W * 0.5f, H * 0.5f };

        int renderResult = SDL_RenderTextureRotated(r, tex, nullptr, &dst, deg, &pivot, SDL_FLIP_NONE);

        // Si la función falló (por no estar disponible), hacemos fallback manual
        if (renderResult != 0 && SDL_GetError() != nullptr && *SDL_GetError() != '\0') {
            SDL_RenderTexture(r, tex, nullptr, &dst);
        }

        }

    // 2) Dibuja los proyectiles
    drawBullets(r);
}


// ---------- Proyectiles ----------

void PlayerShip::spawnBullet(int winW, int winH) {
    // sale del centro, hacia el ángulo actual
    bullets.emplace_back(std::make_unique<BasicBullet>(
        bulletTex,
        posX, posY,
         angle - M_PI_2,
        900.0f,     // velocidad px/s
        1,          // daño
        8,         // tamaño visual
        winW, winH,
        2.0f        // vida máxima
    ));
}

void PlayerShip::updateBullets(float dt, int winW, int winH) {
    for (auto& b : bullets) b->update(dt);
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
                       [](const auto& p){ return !p->alive(); }),
        bullets.end()
    );
}

void PlayerShip::drawBullets(SDL_Renderer* r) {
    for (auto& b : bullets) b->draw(r);
}
