#include "menu.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "input.h"
#include "buzzer.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>

static const char *items[APP_COUNT] = {
	    "Image",
    "Pong",
    "Flappy",
    "Dino",
    "Shooter",
    "To-Do",
	"Snake"
};


/* helper to avoid compile error for earlier placeholder call */
static inline int nine_line_offset(void){ return 10; }

void menu_init(void)
{
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0,0);
    ssd1306_WriteString("MAIN MENU", Font_7x10, White);
    ssd1306_UpdateScreen();
    HAL_Delay(650);
}

AppID menu_run(void)
{
    int selected = 0;
    InputState in;

    while (1)
    {
        ssd1306_Fill(Black);
        for (int i = 0; i < APP_COUNT; i++) {
            ssd1306_SetCursor(0, i* nine_line_offset()); // helper to compute y (implement below)
        }
        // simple layout
        for (int i = 0; i < APP_COUNT; i++) {
            ssd1306_SetCursor(0, i*10);
            if (i == selected)
                ssd1306_WriteString(">", Font_7x10, White);
            else
                ssd1306_WriteString(" ", Font_7x10, White);

            ssd1306_WriteString(items[i], Font_7x10, White);
        }
        ssd1306_UpdateScreen();

        in = input_read();
        if (in.up) {
            if (selected > 0) selected--;
            buzzer_beep();
            osDelay(150);
        }
        if (in.down) {
            if (selected < APP_COUNT-1) selected++;
            buzzer_beep();
            osDelay(150);
        }
        if (in.a) {
            buzzer_beep();
            osDelay(120);

            input_clear();        // <- fixes exit menu freeze
            return (AppID)selected;
        }
        osDelay(50);
    }
}

