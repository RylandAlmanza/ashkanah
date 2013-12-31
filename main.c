#include <ncurses.h>
#include <math.h>
#include "world.h"
#include "directions.h"
#include "colorutils.h"
#include "tiletypes.h"

int WORLD_WIDTH;
int WORLD_HEIGHT;

Point get_delta_from_key(int key) {
    Point delta = {.x = 0, .y = 0};
    if (key == KEY_UP || key == 'k') {
        delta = NORTH_DELTA;
    } else if (key == KEY_RIGHT || key == 'l') {
        delta = EAST_DELTA;
    } else if (key == KEY_DOWN || key == 'j') {
        delta = SOUTH_DELTA;
    } else if (key == KEY_LEFT || key == 'h') {
        delta = WEST_DELTA;
    }
    return delta;
}

void create_tile(int x, int y, int tile_type, World *world) {
    int tile = world->create_entity(world);
    world->mask[tile] = POSITION_COMPONENT | APPEARANCE_COMPONENT;
    if (tile_type == GRASS) {
        world->position[tile].x = x;
        world->position[tile].y = y;
        world->appearance[tile].character = '.';
        world->appearance[tile].foreground = WHITE;
        world->appearance[tile].background = GREEN;
    } else if (tile_type == TREE) {
        world->mask[tile] = world->mask[tile] | COLLISION_COMPONENT;
        world->position[tile].x = x;
        world->position[tile].y = y;
        world->appearance[tile].character = 'T';
        world->appearance[tile].foreground = WHITE;
        world->appearance[tile].background = GREEN;
    } else if (tile_type == WATER) {
        world->mask[tile] = world->mask[tile] | COLLISION_COMPONENT;
        world->position[tile].x = x;
        world->position[tile].y = y;
        world->appearance[tile].character = '~';
        world->appearance[tile].foreground = WHITE;
        world->appearance[tile].background = BLUE;
    }
}

World load_World(char *filename) {
    FILE *map_file = fopen(filename, "r");

    int width;
    int height;
    fscanf(map_file, "%d %d", &width, &height);
    WORLD_WIDTH = width;
    WORLD_HEIGHT = height;
    World world = create_World();
    int tile_type;
    int entity;
    int x;
    int y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            fscanf(map_file, "%d", &tile_type);
            create_tile(x, y, tile_type, &world);
        }
    }

    fclose(map_file);
    return world;
}

#define DISPLAY_MASK (POSITION_COMPONENT | APPEARANCE_COMPONENT)
void display_entities(World *world) {
    int entity;
    for (entity = 0; entity < world->entity_count; entity++) {
        if ((world->mask[entity] & DISPLAY_MASK) == DISPLAY_MASK) {
            int x = world->position[entity].x;
            int y = world->position[entity].y;
            int camera_x = world->camera.x;
            int camera_y = world->camera.y;
            if (x < camera_x ||
                y < camera_y ||
                x >= camera_x + 25 ||
                y >= camera_y + 15) continue;
            char character = world->appearance[entity].character;
            int foreground = world->appearance[entity].foreground;
            int background;
            if (world->appearance[entity].background == TRANSPARENT &&
                entity >= WORLD_WIDTH * WORLD_HEIGHT) {
                background = world->appearance[(y * WORLD_WIDTH) + x].background;
            } else {
                background = world->appearance[entity].background;
            }
            int color_pair = get_color_pair(foreground, background);
            attron(COLOR_PAIR(color_pair));
            mvaddch(y - camera_y, x - camera_x, character);
            attroff(COLOR_PAIR(color_pair));
        }
    }
    refresh();
}

bool move_entity(World *world, int entity, Point delta) {
    int proposed_x = world->position[entity].x + delta.x;
    int proposed_y = world->position[entity].y + delta.y;

    if ((world->mask[entity] & COLLISION_COMPONENT) == COLLISION_COMPONENT) {
        int entity_2;
        for (entity_2 = 0; entity_2 < world->entity_count; entity_2++) {
            if (entity_2 == entity) continue;
            if ((world->mask[entity_2] & COLLISION_COMPONENT) ==
                COLLISION_COMPONENT) {
                if (proposed_x == world->position[entity_2].x &&
                    proposed_y == world->position[entity_2].y) {
                    return false;
                }
            }
        }
    }
    world->position[entity].x = proposed_x;
    world->position[entity].y = proposed_y;
    return true;
}

#define CONTROL_MASK (POSITION_COMPONENT | CONTROL_COMPONENT)
void process_controls(World *world, int key) {
    int entity;
    for (entity = 0; entity < world->entity_count; entity++) {
        if ((world->mask[entity] & CONTROL_MASK) == CONTROL_MASK) {
            Point delta = get_delta_from_key(key);
            if (delta.x != 0 || delta.y != 0) {
                if (move_entity(world, entity, delta)) {
                    world->camera.x += delta.x;
                    world->camera.y += delta.y;
                    if (world->camera.x < 0 ||
                        world->camera.y < 0 ||
                        world->camera.x + 25 > WORLD_WIDTH ||
                        world->camera.y + 15 > WORLD_HEIGHT) {
                        world->camera.x -= delta.x;
                        world->camera.y -= delta.y;
                    }
                    return;
                }
            }
        }
    }
}

int main() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    // Enable ncurses colors
    start_color();
    // Init color pairs
    initialize_all_color_pairs();

    World world = load_World("maps/pork.mapdata");

    int player = world.create_entity(&world);
    world.mask[player] = POSITION_COMPONENT |
                         APPEARANCE_COMPONENT |
                         CONTROL_MASK |
                         COLLISION_COMPONENT;
    world.position[player].x = 12;
    world.position[player].y = 7;
    world.appearance[player].character = '@';
    world.appearance[player].foreground = WHITE;
    world.appearance[player].background = TRANSPARENT;

    display_entities(&world);

    int key;
    while (key != 'q') {
        key = getch();
        process_controls(&world, key);
        display_entities(&world);
    }

    destroy_World(&world);

    curs_set(1);
    endwin();
}
