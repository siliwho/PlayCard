#include "image_viewer.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "comm.h"
#include "input.h"
#include "cmsis_os.h"

static void draw_no_image_screen(void)
{
    ssd1306_Fill(Black);
    ssd1306_SetCursor(10, 20);
    ssd1306_WriteString("No image", Font_7x10, White);
    ssd1306_SetCursor(10, 35);
    ssd1306_WriteString("Open web &", Font_7x10, White);
    ssd1306_SetCursor(10, 45);
    ssd1306_WriteString("upload one!", Font_7x10, White);
    ssd1306_UpdateScreen();
}

static void draw_image_from_buf(void)
{
    if (!g_image_valid) {
        draw_no_image_screen();
        return;
    }

    ssd1306_Fill(Black);

    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 128; x++) {
            int bit_index  = y * 128 + x;
            int byte_index = bit_index >> 3;
            int bit        = 7 - (bit_index & 7);
            uint8_t v      = (g_image_buf[byte_index] >> bit) & 0x01;
            if (v) {
                ssd1306_DrawPixel(x, y, White);
            }
        }
    }

    ssd1306_UpdateScreen();
}

int image_viewer_run(void)
{
    input_clear();

    // Ask ESP32 to send the latest image
    comm_request_image();

    // small wait to let comm_task fill buffer
    osDelay(200);

    draw_image_from_buf();

    while (1) {
        InputState in = input_read();

        // A button exits back to menu
        if (in.a) {
            osDelay(150);
            input_clear();
            return 0;
        }

        osDelay(50);
    }
}
