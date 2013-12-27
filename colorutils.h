#ifndef COLORUTILS_H_
#define COLORUTILS_H_

#include <ncurses.h>

static const int BLACK = COLOR_BLACK;
static const int YELLOW = COLOR_YELLOW;
static const int GREEN = COLOR_GREEN;
static const int BLUE = COLOR_BLUE;
static const int WHITE = COLOR_WHITE;
static const int TRANSPARENT = -1;

int get_color_pair(int foreground, int background);
int initialize_all_color_pairs();
#endif
