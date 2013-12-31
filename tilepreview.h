#ifndef TILEPREVIEW_H_
#define TILEPREVIEW_H_

typedef struct TilePreviewStruct TilePreview;

struct TilePreviewStruct {
    char character;
    int color_pair;
};

TilePreview get_tile_preview(int tile_type);

#endif
