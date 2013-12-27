#include "colorutils.h"

int get_color_pair(int foreground, int background) {
    return foreground + (background * 8) + 1;
}

int initialize_all_color_pairs() {
    int foreground;
    int background;
    for (background = 0; background < 8; background++) {
        for (foreground = 0; foreground < 8; foreground++) {
            init_pair(get_color_pair(foreground, background),
                      foreground,
                      background);
        }
    }
}

