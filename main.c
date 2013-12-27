#include <ncurses.h>
#include "world.h"
#include "directions.h"
#include "colorutils.h"
#include <math.h>

#define WORLD_WIDTH 50
#define WORLD_HEIGHT 30

Point get_delta_from_key(int key) {
    Point delta = {.x = 0, .y = 0};
    switch (key) {
        case KEY_UP:
            delta = NORTH_DELTA;
            break;
        case KEY_RIGHT:
            delta = EAST_DELTA;
            break;
        case KEY_DOWN:
            delta = SOUTH_DELTA;
            break;
        case KEY_LEFT:
            delta = WEST_DELTA;
            break;
    }
    return delta;
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

    int color_pair = get_color_pair(WHITE, BLACK);
    attron(COLOR_PAIR(color_pair));

    World world = create_World();

    int x;
    int y;
    for (y = 0; y < WORLD_HEIGHT; y++) {
        for (x = 0; x < WORLD_WIDTH; x++) {
            int tile = world.create_entity(&world);
            world.mask[tile] = POSITION_COMPONENT | APPEARANCE_COMPONENT;
            world.position[tile].x = x;
            world.position[tile].y = y;
            world.appearance[tile].character = '.';
            world.appearance[tile].foreground = WHITE;
            world.appearance[tile].background = GREEN;
        }
    }       

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

    int tree = world.create_entity(&world);
    world.mask[tree] = POSITION_COMPONENT | APPEARANCE_COMPONENT | COLLISION_COMPONENT;
    world.position[tree].x = 12;
    world.position[tree].y = 6;
    world.appearance[tree].character = 'T';
    world.appearance[tree].foreground = WHITE;
    world.appearance[tree].background = TRANSPARENT;


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
