#ifndef BULLET_H
#define BULLET_H
#define BULLET_SPEED 5
#define BULLETLIFETIME 60
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "../include/game.h"
#include "../include/character.h"

#define BULLET_WIDTH 5
#define BULLET_HEIGHT 5
typedef struct {
    float x;
    float y;
    float dx; // direction vector components
    float dy;
    SDL_Texture *texture;
    //SDL_Renderer *renderer;
} Bullet;

Bullet* createBullet(SDL_Renderer *renderer, float startX, float startY);
void destroyBullet(Bullet *bullet);
void moveBullet(Bullet *bullet);
void drawBullet(Bullet *bullet, SDL_Renderer *renderer);
bool checkCollisionBulletCharacter(Bullet *bullet, Character *pCharacter);

#endif