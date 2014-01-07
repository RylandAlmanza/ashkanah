#ifndef TILEPREVIEW_H_
#define TILEPREVIEW_H_

#include <stdbool.h>

typedef struct TilePreviewStruct TilePreview;

struct TilePreviewStruct {
    char character;
    int color_pair;
    bool is_bold;
};

TilePreview get_tile_preview(int tile_type);

#endif
