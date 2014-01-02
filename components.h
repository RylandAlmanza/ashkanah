#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include <stdbool.h>

typedef enum {
    NO_COMPONENT = 0,
    POSITION_COMPONENT = 1 << 0,
    APPEARANCE_COMPONENT = 1 << 1,
    CONTROL_COMPONENT = 1 << 2,
    COLLISION_COMPONENT = 1 << 3,
    LIGHT_COMPONENT = 1 << 4,
    QUEST_COMPONENT = 1 << 5,
    TREE_COMPONENT = 1 << 6
} Component;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    char character;
    int foreground;
    int background;
    bool is_bold;
} Appearance;

typedef struct {
    bool is_solid;
} Collision;

typedef struct {
    bool brightness;
} Light;

typedef struct {
    bool is_started;
    bool is_done;
    int item_needed;
    int amount_needed;
    char start_text[1024];
    char end_text[1024];
} Quest;

#endif
