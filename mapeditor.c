#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "directions.h"
#include "colorutils.h"
#include "tilepreview.h"

typedef struct MapDataStruct {
    int width;
    int height;
    int *tiles;
} MapData;

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

MapData load_map(char *filename) {
    FILE *map_file = fopen(filename, "r");

    MapData map_data;
    fscanf(map_file, "%d %d", &map_data.width, &map_data.height);
    map_data.tiles = malloc(sizeof(int) * (map_data.width * map_data.height));
    int tile_type;
    int i;
    for (i = 0; i < map_data.width * map_data.height; i++) {
        fscanf(map_file, "%d", &tile_type);
        map_data.tiles[i] = tile_type;
    }

    fclose(map_file);
    return map_data;
}

void save_map(char *filename, MapData map_data) {
    FILE *map_file = fopen(filename, "w");
    fprintf(map_file, "%d %d\n", map_data.width, map_data.height);
    int i;
    for (i = 0; i < (map_data.width * map_data.height); i++) {
        fprintf(map_file, "%d\n", map_data.tiles[i]);
    }

    fclose(map_file);
}

void display_map(MapData map_data) {
    int x;
    int y;
    for (y = 0; y < map_data.height; y++) {
        for (x = 0; x < map_data.width; x++) {
            int tile_type = map_data.tiles[(y * map_data.width) + x];
            TilePreview preview = get_tile_preview(tile_type);
            attron(COLOR_PAIR(preview.color_pair));
            mvaddch(y, x, preview.character);
            attroff(COLOR_PAIR(preview.color_pair));
        }
    }
}

int main(int argc, char *argv[]) {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    // Enable ncurses colors
    start_color();
    // Init color pairs
    initialize_all_color_pairs();

    char filename[1024];
    MapData map_data;

    if (argc == 4) {
        strcpy(filename, argv[1]);
        map_data.width = atoi(argv[2]);
        map_data.height = atoi(argv[3]);
        map_data.tiles = malloc(sizeof(int) *
                                (map_data.width * map_data.height));
        int i;
        for (i = 0; i < map_data.width * map_data.height; i++) {
            map_data.tiles[i] = 1;
        }
    } else if (argc == 2) {
        strcpy(filename, argv[1]);
        map_data = load_map(filename);
    } else {
        printf("Usage: %s <width> <height>\n", argv[0]);
        return 0;
    }
    
    Point cursor = {.x = 0, .y = 0};
    int tile_type = 1;

    int key;
    while (key != 'q') {
        display_map(map_data);
        int cursor_color_pair = get_color_pair(BLACK, WHITE);
        TilePreview preview = get_tile_preview(tile_type);
        attron(COLOR_PAIR(cursor_color_pair));
        mvaddch(cursor.y, cursor.x, preview.character);
        attroff(COLOR_PAIR(cursor_color_pair));

        key = getch();
        Point delta = get_delta_from_key(key);
        if (delta.x != 0 || delta.y != 0) {
            cursor.x += delta.x;
            cursor.y += delta.y;
            continue;
        }
        if (key == '1' ||
            key == '2' ||
            key == '3') {
            tile_type = key - '0';
        }
        if (key == 'p') {
            map_data.tiles[(cursor.y * map_data.width) + cursor.x] = tile_type;
        }
        if (key == 's') {
            save_map(filename, map_data);
        }
    }
    free(map_data.tiles);
    curs_set(1);
    endwin();
}
