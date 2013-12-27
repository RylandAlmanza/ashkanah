#ifndef WORLD_H_
#define WORLD_H_

#include "point.h"
#include "components.h"

typedef struct WorldStruct World;
struct WorldStruct {
    int entity_count;

    Point camera;

    int *mask;

    Position *position;
    Appearance *appearance;
    Collision *collision;
    Light *light;

    int (*create_entity)(World *self);
    void (*destroy_entity)(World *self, int entity);
};

World create_World();
void destroy_World();

#endif
