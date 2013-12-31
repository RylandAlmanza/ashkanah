#include "tilepreview.h"
#include "colorutils.h"

TilePreview get_tile_preview(int tile_type) {
    TilePreview tile_preview = {.character = '0', .color_pair = 1};
    switch (tile_type) {
        case 1:
            tile_preview.character = '.';
            tile_preview.color_pair = get_color_pair(WHITE, GREEN);
            break;
        case 2:
            tile_preview.character = 'T';
            tile_preview.color_pair = get_color_pair(WHITE, GREEN);
            break;
        case 3:
            tile_preview.character = '~';
            tile_preview.color_pair = get_color_pair(WHITE, BLUE);
            break;
    }
    return tile_preview;
}
