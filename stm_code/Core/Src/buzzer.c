/* buzzer.c - PWM based sound engine using TIM2 CH1 (PA0) */
/* Requires MX_TIM2_Init() to configure TIM2 for PWM on CH1 */

#include "buzzer.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <stdlib.h>

extern TIM_HandleTypeDef htim2;

/* Helper: set freq and duty (0-100) */
static void _buzzer_set(uint32_t freq, uint8_t duty_percent)
{
    if (freq == 0 || duty_percent == 0) {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
        return;
    }
    uint32_t timclk = HAL_RCC_GetPCLK1Freq();
    // On APB1 with prescaler, actual timer clk = PCLK1 * 2 if APB1 prescaler != 1
    if ((RCC->CFGR & RCC_CFGR_PPRE1) != 0) timclk *= 2;
    // choose prescaler = 0 (keep simple), compute ARR
    uint32_t arr = (timclk / freq);
    if (arr < 10) arr = 10;
    __HAL_TIM_SET_AUTORELOAD(&htim2, arr - 1);
    uint32_t ccr = (arr * duty_percent) / 100;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, ccr);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    /* ensure PWM started */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

/* non-blocking stop */
static void _buzzer_stop(void) {
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
}

void buzzer_init(void) {
    // Timer config created by CubeMX (MX_TIM2_Init)
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    _buzzer_stop();
}

/* tiny beep */
void buzzer_beep(void)
{
    _buzzer_set(2500, 40);
    osDelay(25);
    _buzzer_stop();
}

/* food sound */
void buzzer_food(void)
{
    _buzzer_set(1500, 60);
    osDelay(40);
    _buzzer_set(2200, 70);
    osDelay(30);
    _buzzer_stop();
}

/* win fanfare */
void buzzer_win(void)
{
    const uint16_t tune[] = {800, 1200, 1600, 2000};
    const uint8_t dur[] = {80, 80, 100, 140};
    for (int i=0;i<4;i++){
        _buzzer_set(tune[i], (i+1)*30);
        HAL_Delay(dur[i]);
    }
    _buzzer_stop();
}

/* laser: descending sweep */
void buzzer_laser(void)
{
    for (int f = 3000; f > 1200; f -= 100) {
        _buzzer_set(f, 60);
        osDelay(8);
    }
    _buzzer_stop();
}

/* explosion: chaotic noisy burst with varying freq and duty */
void buzzer_explosion(void)
{
    // short burst ~100 ms with many tiny steps
    for (int i = 0; i < 140; ++i)
    {
        uint32_t freq = 120 + (rand() % 3000); // chaotic frequency
        uint8_t duty = 15 + (rand() % 75);     // not silent at low
        _buzzer_set(freq, duty);
        osDelay(1);
    }
    _buzzer_stop();
}

/* ... after buzzer_explosion() ... */

/* fail: descending tone */
void buzzer_fail(void)
{
    _buzzer_set(600, 70);
    osDelay(100);
    _buzzer_set(400, 70);
    osDelay(100);
    _buzzer_set(200, 70);
    osDelay(100);
    _buzzer_stop();
}
