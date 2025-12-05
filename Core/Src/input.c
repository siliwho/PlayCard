#include "input.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

/* Map pins to your board: use the same pins you tested earlier */
void input_init(void) { /* nothing, pins initialized by MX_GPIO_Init */ }

InputState input_read(void)
{
    InputState s;
    s.up    = (HAL_GPIO_ReadPin(GPIOB, BTN_1_Pin) == GPIO_PIN_RESET); // active low
    s.down  = (HAL_GPIO_ReadPin(GPIOB, BTN_2_Pin) == GPIO_PIN_RESET);
    s.left  = (HAL_GPIO_ReadPin(GPIOB, BTN_3_Pin) == GPIO_PIN_RESET);
    s.right = (HAL_GPIO_ReadPin(BTN_4_GPIO_Port, BTN_4_Pin) == GPIO_PIN_RESET);
    s.a     = (HAL_GPIO_ReadPin(GPIOB, BTN_5_Pin) == GPIO_PIN_RESET);
    return s;
}


void input_clear(void)
{
    // read pins once to clear any bounce or latched states
    input_read();
    osDelay(20);
}
