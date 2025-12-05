#ifndef BUZZER_H
#define BUZZER_H
#include <stdint.h>
void buzzer_init(void);
void buzzer_beep(void);
void buzzer_food(void);
void buzzer_win(void);
void buzzer_explosion(void);
void buzzer_laser(void);
void buzzer_fail(void);
#endif
