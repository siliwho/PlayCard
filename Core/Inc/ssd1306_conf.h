#ifndef __SSD1306_CONF_H__
#define __SSD1306_CONF_H__

// Include your CubeIDE-generated main.h file
#include "main.h"

// Tell the library to use I2C
#define SSD1306_USE_I2C

#define SSD1306_X_OFFSET   2

#define SSD1306_INCLUDE_FONT_11x18
#define SSD1306_INCLUDE_FONT_7x10
#define SSD1306_INCLUDE_FONT_6x8
// Tell the library which I2C peripheral to use (from your main.c)
#define SSD1306_I2C_PORT        hi2c1
extern I2C_HandleTypeDef      hi2c1;

// Define the I2C address. 0x3C is the 7-bit address
// The library shifts it, so you provide (0x3C << 1)
#define SSD1306_I2C_ADDR        (0x3C << 1)

// Define your display size
#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64

#endif // __SSD1306_CONF_H__
