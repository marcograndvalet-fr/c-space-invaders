#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "entity.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 20
#define PLAYER_SPEED 400.0f

#define BULLET_WIDTH 10
#define BULLET_HEIGHT 20
#define BULLET_SPEED 600.0f

bool init(SDL_Window **window, SDL_Renderer **renderer);
void handle_input(bool *running, const Uint8 *keys, Entity *player, Entity *bullet, bool *bullet_active);
void update(Entity *player, Entity *bullet, Army *army, bool *bullet_active, Ammo* ammo, float dt);
void render(SDL_Renderer *renderer, Entity *player, Entity *bullet, Army* army, bool bullet_active, Ammo* ammo);
void cleanup(SDL_Window *window, SDL_Renderer *renderer);
int endgame(Entity *player,Army *army);
void mass_shooting(Army* army, Ammo* ammo);

#endif
