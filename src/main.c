#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include "entity.h"
#include "game.h"
#include <SDL2/SDL_ttf.h>

int main(void)
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    
    
    if (!init(&window, &renderer))
    {
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 48);
    if (!font)
    {
        SDL_Log("Erreur chargement police: %s", TTF_GetError());
        cleanup(window, renderer);
        return 1;
    }

    bool running = true;
    Uint32 last_ticks = SDL_GetTicks(); //GetTicks compte le nb de ms depuis t = 0

    Entity player = {
        .x = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2,
        .y = SCREEN_HEIGHT - 60,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT,
        .vx = 0,
        .vy = 0,
        .pv = 3
    };


    int nb_monster = 10;
    Army army = {
        .ptr = malloc(nb_monster * sizeof(Entity)),
        .longueur = nb_monster
    };

    for (int k = 0; k<army.longueur ; k++){
        Entity monster = (army.ptr)[k];
        monster.x = SCREEN_WIDTH * ((float)k/army.longueur);
        monster.y = 0;
        monster.w = 40;
        monster.h = 30;
        monster.vx = 0;
        monster.vy = 10;
        monster.pv = 1;
        (army.ptr)[k] = monster;
    } 

    Entity bullet = {0};
    bool bullet_active = false;

    // uniquement les munitions ennemies
    Ammo ammo = {
        .ptr = malloc(0),
        .longueur = 0
    };

    Life life = {
        .ptr = malloc(0),
        .longueur = 0
    };
    

    while (running)
    {
        Uint32 ticks = SDL_GetTicks();
        float dt = (ticks - last_ticks) / 1000.0f; //f c'est pour float c'est pour que le calcul se passe mieux
        if (dt > 0.05f)
            dt = 0.05f;
        last_ticks = ticks;

        SDL_PumpEvents(); //récupère les inputs du clavier
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        handle_input(&running, keys, &player, &bullet, &bullet_active);
        update(&player, &bullet, &army, &bullet_active, &ammo, &life, dt);
        render(renderer, &player, &bullet, &army, bullet_active, &ammo, &life);
        mass_shooting(&army, &ammo);
        healing(&life);
        int test = endgame(&player, &army);
        if (test != 0)
        {
            render_fin(renderer, font, test);
            SDL_Delay(3000);  
            break;
        }
        
    }
    
    cleanup(window, renderer);
    free(army.ptr);
    free(ammo.ptr);
    free(life.ptr);
    if (font){TTF_CloseFont(font);}
    return 0;
}
