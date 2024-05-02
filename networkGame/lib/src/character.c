#include "../include/character.h"
#include <stdlib.h>
#include "../include/game.h"
#include "../include/world.h"
#include "../include/bullet.h"

#define SPRITE_WIDTH 128
#define SPRITE_HEIGHT 128
#define ANIMATION_FRAMES 3

Character *createCharacter(SDL_Renderer *renderer)
{
    Character *pCharacter = malloc(sizeof(Character));
    pCharacter->dest.x = 260;
    pCharacter->dest.y = 220;
    pCharacter->dest.w = CHARACTER_WIDTH;
    pCharacter->dest.h = CHARACTER_HEIGHT;

    IMG_Init(IMG_INIT_PNG | IMG_INIT_PNG);
    SDL_Surface *image = IMG_Load("../lib/resources/SpriteMonkey.png");
    if (!image) {
        printf("Error loading background image: %s\n", IMG_GetError());
        return FALSE;
    }

    pCharacter->tex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    pCharacter->source.x = 0;
    pCharacter->source.y = 0;
    pCharacter->source.w = SPRITE_WIDTH;
    pCharacter->source.h = SPRITE_HEIGHT;

    pCharacter->health = MAX_HEALTH;
    pCharacter->currentFrame = 0;
    pCharacter->animationTimer = 0;
    pCharacter->direction = 0;

    return pCharacter;
}

void updateCharacterAnimation(Character *pCharacter, Uint32 deltaTime)
{
    // Update animation timer
    pCharacter->animationTimer += deltaTime;

    // Change sprite frame every 400 milliseconds
    if (pCharacter->animationTimer > 400)
    {
        pCharacter->currentFrame = (pCharacter->currentFrame + 1) % ANIMATION_FRAMES;
        pCharacter->animationTimer = 0;
    }

    // Update sprite row based on movement direction
    switch (pCharacter->direction)
    {
        case 1: // Left
            pCharacter->source.y = SPRITE_HEIGHT * 2;
            pCharacter->source.x = pCharacter->currentFrame * SPRITE_WIDTH;
            break;
        case 2: // Right
            pCharacter->source.y = SPRITE_HEIGHT;
            pCharacter->source.x = pCharacter->currentFrame * SPRITE_WIDTH;
            break;
        case 3: // Up
            pCharacter->source.y = SPRITE_HEIGHT * 3;
            pCharacter->source.x = pCharacter->currentFrame * SPRITE_WIDTH;
            break;
        default: // Down
            pCharacter->source.y = 0;
            pCharacter->source.x = pCharacter->currentFrame * SPRITE_WIDTH;
            break;
    }
}


void renderCharacter(Character *pCharacter, SDL_Renderer *renderer)
{
    // Render current sprite frame
    SDL_RenderCopy(renderer, pCharacter->tex, &pCharacter->source, &pCharacter->dest);
}

void destroyCharacter(Character *pCharacter)
{
    SDL_DestroyTexture(pCharacter->tex);
    free(pCharacter);
}

void decreaseHealth(Character *pCharacter)
{
    pCharacter->health -= 1;
    //if health is zero, character dies meaning destroy
    if (pCharacter->health <= 0)
    {
        pCharacter->health = 0;
        destroyCharacter(pCharacter);
    }
}

int isCharacterAlive(Character *pCharacter)
{
    return pCharacter->health > 0;
}

void turnLeft(Character *pCharacter)
{
    pCharacter->dest.x -= MOVE_SPEED;
    pCharacter->direction = 1;
    updateCharacterAnimation(pCharacter, 100); // Update animation with a fixed delta time
}

void turnRight(Character *pCharacter)
{
    pCharacter->dest.x += MOVE_SPEED;
    pCharacter->direction = 2;
    updateCharacterAnimation(pCharacter, 100);
}

void turnUp(Character *pCharacter)
{
    pCharacter->dest.y -= MOVE_SPEED;
    pCharacter->direction = 3;
    updateCharacterAnimation(pCharacter, 100);
}

void turnDown(Character *pCharacter)
{
    pCharacter->dest.y += MOVE_SPEED;
    pCharacter->direction = 0;
    updateCharacterAnimation(pCharacter, 100);
}
