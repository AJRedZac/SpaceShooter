#include "PlayerShip.h"
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 (M_PI / 2.0)
#endif

PlayerShip::PlayerShip(SDL_Texture* shipTex, int x, int y, int size): center{ x, y }, size(size), angle(0.0f), tex(shipTex) {}

void PlayerShip::setBulletTexture(SDL_Texture* tex) { bulletTex = tex; }
void PlayerShip::setFiring(bool on) { firing = on; }

void PlayerShip::update(float mouseX, float mouseY, float dt) {
    // apuntar
    float dx = mouseX - center.x;
    float dy = mouseY - center.y;
    angle = std::atan2(dy, dx) + M_PI_2;

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

void PlayerShip::handle_input(const bool* keystate) {
    const float step = 10.0f;

    float fwdx = std::cos(angle);
    float fwdy = std::sin(angle);
    float rgtx = -fwdy;
    float rgty =  fwdx;

    vx = vy = 0.0f;
    if (keystate[SDL_SCANCODE_W]) { vx += fwdx * step; vy += fwdy * step; }
    if (keystate[SDL_SCANCODE_S]) { vx -= fwdx * step; vy -= fwdy * step; }
    if (keystate[SDL_SCANCODE_D]) { vx += rgtx * step; vy += rgty * step; }
    if (keystate[SDL_SCANCODE_A]) { vx -= rgtx * step; vy -= rgty * step; }

    center.x += (int)std::round(vx);
    center.y += (int)std::round(vy);
}

void PlayerShip::draw(SDL_Renderer* r) {
    // 1) Dibuja la nave
    if (tex) {
        // Alto visible = size*2
        const float H = (float)(size * 2);
        float W = H;

        // Si conoces las dimensiones del sprite, conserva proporción.
        // Asegúrate de haber llenado texW/texH en el ctor (o aquí).
        if (texW > 0 && texH > 0) {
            W = H * (float)texW / (float)texH;
        } else {
            // Fallback: intenta consultarlas (si tu build lo soporta)
            int w = 0, h = 0;
            if (SDL_GetTextureSize(tex, &w, &h)) {
                texW = w; texH = h;
                W = H * (float)texW / (float)texH;
            }
        }

        SDL_FRect dst{ (float)center.x - W * 0.5f,
                       (float)center.y - H * 0.5f,
                       W, H };

        const double deg = angle * (180.0 / M_PI);
        const SDL_FPoint pivot{ W * 0.5f, H * 0.5f };

        // Dibujo con rotación; si falla por versión, dibuja sin rotar.
        if (SDL_RenderTextureRotated(r, tex, nullptr, &dst, deg, &pivot, SDL_FLIP_NONE) != 0) {
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
        (float)center.x, (float)center.y,
        angle,
        900.0f,     // velocidad px/s
        1,          // daño
        12,         // tamaño visual
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
