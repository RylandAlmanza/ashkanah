#include "tiletypes.h"
#include "tilepreview.h"
#include "colorutils.h"

TilePreview get_tile_preview(int tile_type) {
    TilePreview tile_preview = {.character = '0', .color_pair = 1};
    switch (tile_type) {
        case GRASS:
            tile_preview.character = '.';
            tile_preview.color_pair = get_color_pair(WHITE, GREEN);
            tile_preview.is_bold = false;
            break;
        case TREE:
            tile_preview.character = 'T';
            tile_preview.color_pair = get_color_pair(WHITE, GREEN);
            tile_preview.is_bold = false;
            break;
        case WATER:
            tile_preview.character = '~';
            tile_preview.color_pair = get_color_pair(WHITE, BLUE);
            tile_preview.is_bold = false;
            break;
        case ROCK_WALL:
            tile_preview.character = '#';
            tile_preview.color_pair = get_color_pair(BLACK, WHITE);
            tile_preview.is_bold = true;
            break;
        case BRIDGE:
            tile_preview.character = '=';
            tile_preview.color_pair = get_color_pair(BLACK, WHITE);
            tile_preview.is_bold = true;
            break;
        case ROCK_FLOOR:
            tile_preview.character = '.';
            tile_preview.color_pair = get_color_pair(BLACK, BLACK);
            tile_preview.is_bold = true;
            break;
    }
    return tile_preview;
}
