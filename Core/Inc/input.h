#ifndef INPUT_H
#define INPUT_H
#include <stdint.h>
typedef struct {
    uint8_t up;
    uint8_t down;
    uint8_t left;
    uint8_t right;
    uint8_t a;
} InputState;
void input_init(void);
InputState input_read(void);
void input_clear(void);
#endif
