#include <stdlib.h>
#include "world.h"

int World_create_entity(World *self) {
    int entity = self->entity_count;
    self->entity_count += 1;
    self->mask[entity] = NO_COMPONENT;
    self->mask = realloc(self->mask, sizeof(int) * self->entity_count);
    self->position = realloc(self->position,
                             sizeof(Position) * self->entity_count);
    self->appearance = realloc(self->appearance,
                               sizeof(Appearance) * self->entity_count);
    self->collision = realloc(self->collision,
                              sizeof(Collision) * self->entity_count);
    self->light = realloc(self->light, sizeof(Light) * self->entity_count);

    return entity;
}

void World_destroy_entity(World *self, int entity) {
    self->mask[entity] = NO_COMPONENT;
}

World create_World() {
    World world;

    world.entity_count = 0;

    world.mask = malloc(sizeof(int));

    world.position = malloc(sizeof(Position));
    world.appearance = malloc(sizeof(Appearance));
    world.collision = malloc(sizeof(Collision));
    world.light = malloc(sizeof(Light));
    world.create_entity = &World_create_entity;
    world.destroy_entity = &World_destroy_entity;

    return world;
}

void destroy_World(World *world) {
    free(world->mask);
    free(world->position);
    free(world->appearance);
    free(world->collision);
    free(world->light);
}
