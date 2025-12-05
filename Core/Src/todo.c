#include "todo.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "input.h"
#include "comm.h"
#include "cmsis_os.h"
#include <stdio.h>

int todo_run(void)
{
    int selected = 0;

    input_clear();

    // Ask ESP32 to send current To-Do list
    comm_request_todos();
    osDelay(200);

    while (1) {
        // -------- Draw UI ----------
        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("To-Do", Font_7x10, White);

        if (g_todo_count == 0) {
            ssd1306_SetCursor(0, 15);
            ssd1306_WriteString("No tasks.", Font_7x10, White);
            ssd1306_SetCursor(0, 25);
            ssd1306_WriteString("Use web to add.", Font_7x10, White);
        } else {
            if (selected >= g_todo_count) selected = g_todo_count - 1;

            // Show at most 5 tasks on screen
            int max_on_screen = 5;
            int start = 0;
            if (selected >= max_on_screen) {
                start = selected - (max_on_screen - 1);
            }

            for (int i = 0; i < max_on_screen; i++) {
                int idx = start + i;
                if (idx >= g_todo_count) break;

                int y = 12 + i * 10;
                ssd1306_SetCursor(0, y);

                if (idx == selected) {
                    ssd1306_WriteString(">", Font_7x10, White);
                } else {
                    ssd1306_WriteString(" ", Font_7x10, White);
                }

                ssd1306_WriteString(g_todos[idx].done ? "[x] " : "[ ] ", Font_7x10, White);
                ssd1306_WriteString(g_todos[idx].text, Font_7x10, White);
            }
        }

        ssd1306_UpdateScreen();

        // ---------- Input ----------
        InputState in = input_read();

        if (in.up && g_todo_count > 0) {
            if (selected > 0) selected--;
            osDelay(120);
        }
        if (in.down && g_todo_count > 0) {
            if (selected < g_todo_count - 1) selected++;
            osDelay(120);
        }

        // Toggle with A
        if (in.a && g_todo_count > 0) {
            TodoItem *t = &g_todos[selected];
            t->done ^= 1;
            comm_send_todo_toggle(t->id);   // notify ESP32 / backend
            osDelay(150);
        }

        // Exit with RIGHT
        if (in.right) {
            osDelay(120);
            input_clear();
            return 0;
        }

        osDelay(30);
    }
}
