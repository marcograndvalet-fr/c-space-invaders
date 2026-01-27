#ifndef ENTITY_H
#define ENTITY_H

typedef struct
{
    float x, y;
    float vx, vy;
    int w, h;
    int pv;

} Entity;

#endif

#ifndef ARMY_H
#define ARMY_H

typedef struct {
    Entity* ptr;
    int longueur;
} Army;

#endif

#ifndef AMMO_H
#define AMMO_H

typedef struct {
    Entity* ptr;
    int longueur;
} Ammo;
#endif


