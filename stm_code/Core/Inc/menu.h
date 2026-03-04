#ifndef MENU_H
#define MENU_H

#include <stdint.h>

typedef enum {
    APP_SNAKE = 0,
    APP_PONG,
    APP_FLAPPY,
    APP_DINO,
    APP_SHOOTER,
    APP_TODO,
    APP_IMAGE,
    APP_COUNT
} AppID;

void menu_init(void);
AppID menu_run(void);

#endif
