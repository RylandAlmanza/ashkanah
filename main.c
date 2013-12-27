#include <ncurses.h>
#include "world.h"
#include "directions.h"
#include "colorutils.h"

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
            char character = world->appearance[entity].character;
            int foreground = world->appearance[entity].foreground;
            int background = world->appearance[entity].background;
            int color_pair = get_color_pair(foreground, background);
            attron(COLOR_PAIR(color_pair));
            mvaddch(y, x, character);
            attroff(COLOR_PAIR(color_pair));
        }
    }
    refresh();
}

#define CONTROL_MASK (POSITION_COMPONENT | CONTROL_COMPONENT)
void process_controls(World *world, int key) {
    int entity;
    for (entity = 0; entity < world->entity_count; entity++) {
        if ((world->mask[entity] & CONTROL_MASK) == CONTROL_MASK) {
            Point delta = get_delta_from_key(key);
            if (delta.x != 0 || delta.y != 0) {
                world->position[entity].x += delta.x;
                world->position[entity].y += delta.y;
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
    int player = world.create_entity(&world);
    world.mask[player] = POSITION_COMPONENT | APPEARANCE_COMPONENT | CONTROL_MASK;
    world.position[player].x = 2;
    world.position[player].y = 2;
    world.appearance[player].character = '@';
    world.appearance[player].foreground = WHITE;
    world.appearance[player].background = BLACK;

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
