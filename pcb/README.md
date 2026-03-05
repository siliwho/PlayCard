### 1. The ILI9341 TFT Display (Hardware SPI)

To get the highest frame rate possible for games, you should connect the display to **SPI1**. On the STM32F411, SPI1 is on the APB2 bus, which can run at up to 50MHz, making it the fastest SPI peripheral on the board.

- **SCK** ➔ **PA5** (SPI1_SCK)
- **SDI\_(MOSI)** ➔ **PA7** (SPI1_MOSI)
- **SDO\_(MISO)** ➔ **PA6** (SPI1*MISO - \_Note: You only need this if you plan to read pixel data back from the screen. If you are only drawing to it, you can leave this unconnected.*)
- **CS** ➔ **PB0** (Standard GPIO Output - manual chip select)
- **D/C** ➔ **PB1** (Standard GPIO Output - Data/Command toggle)
- **RESET** ➔ **PB2** (Standard GPIO Output)
- **LED** ➔ **3V3** (This turns the backlight on full blast. Alternatively, wire it to **PA8** later if you want to write a hardware PWM driver for brightness control).

### 2. The Buttons (SW1 - SW6)

Since the switches are tied to `GND` on one side, you need to connect the other side to standard GPIO pins and configure them as **Inputs with Internal Pull-Up Resistors** in the bare-metal code. When a button is pressed, the pin will read `0` (LOW).

I recommend using a contiguous block of pins on the same port so reading the buttons is a single, fast register read (e.g., reading the `GPIOB->IDR` register).

- **SW1** ➔ **PB12**
- **SW2** ➔ **PB13**
- **SW3** ➔ **PB14**
- **SW4** ➔ **PB15**
- **SW5** ➔ **PA0**
- **SW6** ➔ **PA1**

### 3. The ESP32-S3-ZERO (UART Bluetooth Bridge)

the schematic already has this handled perfectly with the net labels! Just to confirm the logic:

- **BlackPill PA9 (TX1)** is correctly routed to **ESP32 RX**.
- **BlackPill PA10 (RX1)** is correctly routed to **ESP32 TX**.
- **Crucial step:** Ensure the `GND` net for the BlackPill, the Display, and the ESP32 are all tied together so the UART and SPI signals share the same reference voltage.

### 4. Ignored Pins (For Now)

- **Touch Pins (T_CLK, T_CS, etc.):** Unless the games require a touchscreen, leave these disconnected to save GPIO and complexity.
- **SD Card Pins:** Leave these unconnected for now. You can always wire them to SPI2 later if you decide to load game assets from an SD card.
