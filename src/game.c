#include <SDL2/SDL.h>
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

bool init(SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Erreur SDL_Init: %s", SDL_GetError());
        return false;
    }

    *window = SDL_CreateWindow("Space Invaders (SDL)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!*window)
    {
        SDL_Log("Erreur SDL_CreateWindow: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer)
    {
        SDL_Log("Erreur SDL_CreateRenderer: %s", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }

    return true;
}

void handle_input(bool *running, const Uint8 *keys, Entity *player, Entity *bullet, bool *bullet_active)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            *running = false;
    }

    player->vx = 0.0f;
    if (keys[SDL_SCANCODE_LEFT])
        player->vx = -PLAYER_SPEED;
    if (keys[SDL_SCANCODE_RIGHT])
        player->vx = PLAYER_SPEED;

    if (keys[SDL_SCANCODE_SPACE] && !*bullet_active)
    {
        *bullet_active = true;
        bullet->x = player->x + player->w / 2 - BULLET_WIDTH / 2;
        bullet->y = player->y;
        bullet->w = BULLET_WIDTH;
        bullet->h = BULLET_HEIGHT;
        bullet->vy = -BULLET_SPEED;
    }
}

void update(Entity *player, Entity *bullet, Army *army, bool *bullet_active, Ammo* ammo, float dt)
{

    // déplacement joueur
    player->x += player->vx * dt;
    
    if (player->x < 0)
        player->x = 0;
    if (player->x + player->w > SCREEN_WIDTH)
        player->x = SCREEN_WIDTH - player->w;

    // déplacement munition
    if (*bullet_active)
    {
        bullet->y += bullet->vy * dt;
        if (bullet->y + bullet->h < 0)
            *bullet_active = false;
    }

    // déplacement armée
    for (int k = 0 ; k < army->longueur ; k++)
    {
        (army->ptr)[k].y += (army->ptr)[k].vy * dt; 
    }

    // déplacement munitions monstres
    for (int k = 0 ; k < ammo->longueur ; k++)
    {
        Entity balle = (ammo->ptr)[k];
        if (balle.y < SCREEN_HEIGHT) {
            balle.y += balle.vy * dt;
            (ammo->ptr)[k] = balle;
        }
        
    }

    // collision entre munition et monstre
    if (*bullet_active)
    {
        int xb = bullet->x;
        int yb = bullet->y;
        for (int k = 0 ; k < army->longueur ; k++)
        {
            int xm = (army->ptr)[k].x;
            int ym = (army->ptr)[k].y;
            if (xb >= xm && xb <= xm + (army->ptr)[k].w)
            {
                if (yb >= ym && yb <= ym + (army->ptr)[k].h)
                {
                    if ((army->ptr)[k].pv != 0)
                    {
                        (army->ptr)[k].pv -= 1;
                        bullet_active = false;
                    }
                }

            }
        }
    }

    // collision tirs ennemis et joueur 
    for (int k = 0; k < ammo->longueur ; k++)
    {
        Entity balle = (ammo->ptr)[k];
        int xb = balle.x;
        int yb = balle.y;
        if (xb >= player->x && xb <= player->x + PLAYER_WIDTH)
        {
            if (yb >= player->y && yb <= player->y + PLAYER_HEIGHT)
            {
                if (player->pv != 0)
                {
                    player->pv -= 1;
                    balle.y = SCREEN_HEIGHT;
                    (ammo->ptr)[k] = balle;
                }
            }
        }
    }

}

void render(SDL_Renderer *renderer, Entity *player, Entity *bullet, Army* army, bool bullet_active, Ammo* ammo)
{
    // backgroud
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // joueur
    SDL_Rect player_rect = {
        (int)player->x, (int)player->y,
        player->w, player->h};
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &player_rect);

    // armée

    for (int k = 0; k< army->longueur; k++){
        if ((army->ptr)[k].pv != 0) {
            Entity monster = (army->ptr)[k];
            SDL_Rect monster_rect = {
            (int)monster.x, (int)monster.y,
            monster.w, monster.h};

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &monster_rect);
        }
    }

    // tir du joueur
    if (bullet_active)
    {
        SDL_Rect bullet_rect = {
            (int)bullet->x, (int)bullet->y,
            bullet->w, bullet->h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &bullet_rect);
    }

    // tirs des monstres
    for (int k = 0; k< ammo->longueur; k++){
        Entity balle = (ammo->ptr)[k];
        if (balle.y < SCREEN_HEIGHT) {
            SDL_Rect balle_rect = {
            (int)balle.x, (int)balle.y,
            balle.w, balle.h};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &balle_rect);
        }        
    }
    SDL_RenderPresent(renderer);
}

void cleanup(SDL_Window *window, SDL_Renderer *renderer)
{
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

// cette fonction renvoie 0 si il ne se passe rien 1 si victoire et 2 si défaite
int endgame(Entity* player, Army* army)
{
    if (player->pv == 0)
        {
            return 2;
        }
    bool flag = true; //pour savoir s'ils sont tous morts
    for (int k = 0; k< army->longueur ; k++)
    {
        if (((army->ptr)[k].y + (army->ptr)[k].h >= SCREEN_HEIGHT) && (army->ptr)[k].pv != 0)
        {
            return 2;
        }
        if ((army->ptr)[k].pv != 0)
        {
            flag = false;
        }
    }
    if (flag)
    {
        return 1;
    }
    return 0;
}

// générer les tirs ennemis
void mass_shooting(Army* army, Ammo* ammo){
    for (int k = 0; k<army->longueur ; k++)
    {
        if ((army->ptr)[k].pv != 0)
        {
            int roll = rand() % 10000;
            if (roll<1)
            {
                Entity shoot = {
                    .x = (army->ptr)[k].x + 0.5*(army->ptr)[k].w,
                    .y = (army->ptr)[k].y + (army->ptr)[k].h,
                    .vx = 0,
                    .vy = 200,
                    .h = BULLET_HEIGHT,
                    .w = BULLET_WIDTH,
                    .pv = 0 
                };
                ammo->longueur += 1;
                ammo->ptr = realloc(ammo->ptr, (ammo->longueur) * sizeof(Entity));
                (ammo->ptr)[ammo->longueur - 1] = shoot;
            }
        }
    }
}