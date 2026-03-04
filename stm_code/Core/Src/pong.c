#include "pong.h"
#include "ssd1306.h"
#include "buzzer.h"
#include "cmsis_os.h"
#include "ssd1306_fonts.h"
#include "main.h"
#include "input.h"
#include <stdio.h>
#include <stdlib.h>

#define PADDLE_H 16
#define PADDLE_W 3
#define BALL_S   3
#define MAX_SCORE 5




static int p1_y, p2_y;
static int ball_x, ball_y, ball_vx, ball_vy;
static int p1_score, p2_score;

/* ------------ Read Buttons DIRECTLY (Fixed) -------------- */
static inline int btn_up()    { return HAL_GPIO_ReadPin(GPIOB, BTN_1_Pin) == 0; }
static inline int btn_down()  { return HAL_GPIO_ReadPin(GPIOB, BTN_2_Pin) == 0; }
static inline int btn_left()  { return HAL_GPIO_ReadPin(GPIOB, BTN_3_Pin) == 0; }
static inline int btn_right() { return HAL_GPIO_ReadPin(BTN_4_GPIO_Port, BTN_4_Pin) == 0; }
static inline int btn_A()     { return HAL_GPIO_ReadPin(GPIOB, BTN_5_Pin) == 0; }

// Exit menu return codes
#define EXIT_RESUME   0
#define EXIT_RESTART  1
#define EXIT_END      2

static int pong_exit_menu(int score)
{
    int selection = 0;      // 0 = Resume, 1 = Restart, 2 = End
    const char* items[3] = { "Resume", "Restart", "End" };

    input_clear();
    while (1)
    {
        ssd1306_Fill(Black);
        ssd1306_SetCursor(25, 0);
        ssd1306_WriteString("GAME OVER", Font_7x10, White);

        char buf[32];
        snprintf(buf, sizeof(buf), "Score: %d", score);
        ssd1306_SetCursor(10, 16);
        ssd1306_WriteString(buf, Font_7x10, White);

        // Draw options
        for (int i = 0; i < 3; i++)
        {
            ssd1306_SetCursor(10, 32 + i*12);
            if (i == selection) {
                ssd1306_WriteString("> ", Font_7x10, White);
            } else {
                ssd1306_WriteString("  ", Font_7x10, White);
            }
            ssd1306_WriteString((char*)items[i], Font_7x10, White);
        }

        ssd1306_UpdateScreen();

        // Read input
        InputState in = input_read();
        if (in.up) {
            if (selection > 0) selection--;
            osDelay(100);
        }
        if (in.down) {
            if (selection < 2) selection++;
            osDelay(100);
        }
        if (in.a) {
            osDelay(100);
            return selection;   // 0/1/2
        }

        osDelay(10);
    }
}


/* ------------ Reset Ball ---------------- */
static void reset_ball(int dir)
{
    ball_x = SSD1306_WIDTH / 2;
    ball_y = SSD1306_HEIGHT / 2;

    ball_vx = dir * 2;               // was 1 → now 2x faster
    ball_vy = (rand() % 3) - 1;
    if (ball_vy == 0) ball_vy = 1;
}


/* -------------- Draw everything ------------------ */
static int prev_p1_y = -1, prev_p2_y = -1;
static int prev_ball_x = -1, prev_ball_y = -1;


static void draw_pong(void) {
	ssd1306_Fill(Black);/* Player Paddle */
	ssd1306_FillRectangle( 2, p1_y, 2 + PADDLE_W - 1, p1_y + PADDLE_H - 1, White ); /* AI Paddle */
	ssd1306_FillRectangle( SSD1306_WIDTH - 2 - PADDLE_W, p2_y, SSD1306_WIDTH - 2 - 1, p2_y + PADDLE_H - 1, White ); /* Ball */
	ssd1306_FillRectangle( ball_x - BALL_S/2, ball_y - BALL_S/2, ball_x + BALL_S/2, ball_y + BALL_S/2, White ); /* Score */
	char buf[8];
	snprintf(buf, sizeof(buf), "%d", p1_score);
	ssd1306_SetCursor(48, 2);
	ssd1306_WriteString(buf, Font_7x10, White);
	snprintf(buf, sizeof(buf), "%d", p2_score);
	ssd1306_SetCursor(128 - 48 - 7, 2);
	ssd1306_WriteString(buf, Font_7x10, White);
	ssd1306_UpdateScreen();
	}

static void fast_draw_pong(void)
{
    /* Erase previous paddle positions */
    if (prev_p1_y >= 0)
        ssd1306_FillRectangle(2, prev_p1_y,
                              2 + PADDLE_W - 1,
                              prev_p1_y + PADDLE_H - 1,
                              Black);

    if (prev_p2_y >= 0)
        ssd1306_FillRectangle(SSD1306_WIDTH - 2 - PADDLE_W,
                              prev_p2_y,
                              SSD1306_WIDTH - 2 - 1,
                              prev_p2_y + PADDLE_H - 1,
                              Black);

    /* Erase previous ball */
    if (prev_ball_x >= 0)
        ssd1306_FillRectangle(prev_ball_x - BALL_S/2,
                              prev_ball_y - BALL_S/2,
                              prev_ball_x + BALL_S/2,
                              prev_ball_y + BALL_S/2,
                              Black);

    /* Draw new paddle positions */
    ssd1306_FillRectangle(2, p1_y,
                          2+PADDLE_W-1,
                          p1_y+PADDLE_H-1,
                          White);

    ssd1306_FillRectangle(SSD1306_WIDTH - 2 - PADDLE_W,
                          p2_y,
                          SSD1306_WIDTH - 2 - 1,
                          p2_y+PADDLE_H-1,
                          White);

    /* Draw new ball */
    ssd1306_FillRectangle(ball_x - BALL_S/2,
                          ball_y - BALL_S/2,
                          ball_x + BALL_S/2,
                          ball_y + BALL_S/2,
                          White);

    /* Update score rarely (every goal only) – not every frame */
    static int last_p1 = -1, last_p2 = -1;
    if (p1_score != last_p1 || p2_score != last_p2)
    {
        ssd1306_FillRectangle(40, 0, 88, 12, Black);
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", p1_score);
        ssd1306_SetCursor(48, 2);
        ssd1306_WriteString(buf, Font_7x10, White);

        snprintf(buf, sizeof(buf), "%d", p2_score);
        ssd1306_SetCursor(128 - 48 - 7, 2);
        ssd1306_WriteString(buf, Font_7x10, White);

        last_p1 = p1_score;
        last_p2 = p2_score;
    }

    ssd1306_UpdateScreen();

    /* Store old positions */
    prev_p1_y = p1_y;
    prev_p2_y = p2_y;
    prev_ball_x = ball_x;
    prev_ball_y = ball_y;
}


/* --------------- PONG GAME ------------------- */
int pong_run(void)
{

	ssd1306_Fill(Black);      // FULL CLEAR
    ssd1306_UpdateScreen();   // IMPORTANT
    osDelay(50);
    // Initialize once at the start
    p1_y = (64 - PADDLE_H) / 2;
    p2_y = (64 - PADDLE_H) / 2;
    p1_score = 0;
    p2_score = 0;
    reset_ball(1);

    int paused = 0;

    while (1)
    {
        // Read input
        InputState in = input_read();

        // ► EXIT/PAUSE BUTTON
        if (in.a) {
        	input_clear();
            int choice = pong_exit_menu(p1_score);

            if (choice == EXIT_RESUME) {
                // Clear entire screen when returning
                ssd1306_Fill(Black);
                ssd1306_UpdateScreen();
                osDelay(50);
                input_clear();
            }
            else if (choice == EXIT_RESTART) {
                p1_score = 0;
                p2_score = 0;
                p1_y = (64 - PADDLE_H)/2;
                p2_y = (64 - PADDLE_H)/2;
                reset_ball(1);

                // Also clear screen before restarting
                ssd1306_Fill(Black);
                ssd1306_UpdateScreen();
                osDelay(50);
            }
            else if (choice == EXIT_END) {
                // END → return to READY screen
                ssd1306_Fill(Black);
                ssd1306_UpdateScreen();
                return p1_score;
            }

            // IMPORTANT: Skip drawing this frame
            continue;
        }


        // Player input
        if (in.up)   p1_y -= 4;
        if (in.down) p1_y += 4;


        if (p1_y < 0) p1_y = 0;
        if (p1_y > (64 - PADDLE_H)) p1_y = 64 - PADDLE_H;

        // AI
        int p2_center = p2_y + PADDLE_H/2;
        if (p2_center < ball_y - 3) p2_y += 3;
        if (p2_center > ball_y + 3) p2_y -= 3;
        if (p2_y < 0) p2_y = 0;
        if (p2_y > 64 - PADDLE_H) p2_y = 64 - PADDLE_H;

        // Ball physics
        ball_x += ball_vx * 2;
        ball_y += ball_vy * 2;


        // Bounce top/bottom
        if (ball_y <= 0 || ball_y >= 63) {
            ball_vy = -ball_vy;
            buzzer_beep();
        }

        // Paddles
        if (ball_x <= (2 + PADDLE_W) &&
            ball_y >= p1_y && ball_y <= p1_y + PADDLE_H)
        {
            ball_vx = -ball_vx;
            ball_x = 2 + PADDLE_W + 1;
            buzzer_beep();
        }

        if (ball_x >= (128 - PADDLE_W - 2) &&
            ball_y >= p2_y && ball_y <= p2_y + PADDLE_H)
        {
            ball_vx = -ball_vx;
            ball_x = 128 - PADDLE_W - 3;
            buzzer_beep();
        }

        // Scoring
        if (ball_x < 0) {
            p2_score++;
            buzzer_fail();
            reset_ball(1);
            osDelay(200);
        }
        if (ball_x > 127) {
            p1_score++;
            buzzer_food();
            reset_ball(-1);
            osDelay(200);
        }

        draw_pong();
        osDelay(1);   // smaller delay = smoother animation

    }
}
