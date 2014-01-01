#include <ncurses.h>
#include <math.h>
#include <string.h>
#include "world.h"
#include "directions.h"
#include "colorutils.h"
#include "tiletypes.h"

int WORLD_WIDTH;
int WORLD_HEIGHT;

typedef enum {
    TREES_CUT = 0
} Stats;

int player_stats[1];

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

void log_message(char message[1024]) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    int x = 0;
    int y = 15;
    int i;
    attron(COLOR_PAIR(get_color_pair(WHITE, BLACK)));
    for (i = 0; i < strlen(message); i++) {
        mvaddch(y, x, message[i]);
        x += 1;
        if (x == rows) {
            x = 0;
            y += 1;
        }
    }
}   

void create_tile(int x, int y, int tile_type, int tile, World *world) {
    world->mask[tile] = POSITION_COMPONENT | APPEARANCE_COMPONENT;
    if (tile_type == GRASS) {
        world->position[tile].x = x;
        world->position[tile].y = y;
        world->appearance[tile].character = '.';
        world->appearance[tile].foreground = WHITE;
        world->appearance[tile].background = GREEN;
    } else if (tile_type == TREE) {
        world->mask[tile] = world->mask[tile] |
                            COLLISION_COMPONENT |
                            TREE_COMPONENT;
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
            int tile = world.create_entity(&world);
            create_tile(x, y, tile_type, tile, &world);
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

void cut_tree(World *world, int tree) {
    player_stats[TREES_CUT] += 1;
    create_tile(world->position[tree].x,
                world->position[tree].y,
                GRASS,
                tree,
                world);
}

void do_quest(World *world, int entity) {
    Quest *quest = &world->quest[entity];
    if (player_stats[quest->stat_watched] >= quest->value_needed) {
        log_message(quest->finish_text);
    } else {
        log_message(quest->start_text);
    }
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
                    if ((world->mask[entity] & CONTROL_COMPONENT) ==
                        CONTROL_COMPONENT &&
                        (world->mask[entity_2] & QUEST_COMPONENT) ==
                        QUEST_COMPONENT)
                        do_quest(world, entity_2);
                    if ((world->mask[entity] & CONTROL_COMPONENT) ==
                        CONTROL_COMPONENT &&
                        (world->mask[entity_2] & TREE_COMPONENT) ==
                        TREE_COMPONENT)
                        cut_tree(world, entity_2);
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
                    world->camera.x = world->position[entity].x - 12;
                    world->camera.y = world->position[entity].y - 7;
                    if (world->camera.x < 0) world->camera.x = 0;
                    if (world->camera.y < 0) world->camera.y = 0;
                    if (world->camera.x + 25 > WORLD_WIDTH)
                        world->camera.x = WORLD_WIDTH - 25;
                    if (world->camera.y + 15 > WORLD_HEIGHT)
                        world->camera.y = WORLD_HEIGHT - 15;
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
                         CONTROL_COMPONENT |
                         COLLISION_COMPONENT;
    world.position[player].x = 12;
    world.position[player].y = 7;
    world.appearance[player].character = '@';
    world.appearance[player].foreground = WHITE;
    world.appearance[player].background = TRANSPARENT;

    player_stats[TREES_CUT] = 0;

    int logger = world.create_entity(&world);
    world.mask[logger] = POSITION_COMPONENT |
                         APPEARANCE_COMPONENT |
                         COLLISION_COMPONENT |
                         QUEST_COMPONENT;
    world.position[logger].x = 20;
    world.position[logger].y = 20;
    world.appearance[logger].character = '@';
    world.appearance[logger].foreground = BLUE;
    world.appearance[logger].background = TRANSPARENT;
    world.quest[logger].stat_watched = TREES_CUT;
    world.quest[logger].value_needed = 5;

    char start_text[1024] = "Hey, dude, I'm feelin' a little lazy today. ";
    strcat(start_text, "Wanna' cut down some trees for me? I need 5 logs.");
    strcpy(world.quest[logger].start_text, start_text);
    
    char finish_text[1024] = "Thanks, man. Huh? Compensation? Uh, sorry, ";
    strcat(finish_text, "dude. I don't really have anything to give you. I ");
    strcat(finish_text, "thought you were just doing me a favor.");
    strcpy(world.quest[logger].finish_text, finish_text);

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
