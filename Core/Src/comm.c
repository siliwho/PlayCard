#include "comm.h"
#include "main.h"
#include "cmsis_os.h"
#include <string.h>

/* ---------------------------------------------------------
   GLOBAL VARIABLES (shared with apps)
   --------------------------------------------------------- */
uint8_t  g_image_buf[IMAGE_BUF_SIZE];
uint8_t  g_image_valid = 0;

TodoItem g_todos[MAX_TODOS];
uint8_t  g_todo_count = 0;

/* ---------------------------------------------------------
   UART + Protocol Definitions
   --------------------------------------------------------- */
extern UART_HandleTypeDef huart2;

#define SYNC_BYTE         0xAA

#define MSG_IMAGE_FULL    0x01   // ESP32 → STM32
#define MSG_TODO_FULL     0x02

#define MSG_REQ_IMAGE     0x03   // STM32 → ESP32
#define MSG_REQ_TODOS     0x04

#define MSG_TODO_TOGGLE   0x10   // STM32 → ESP32

/* ---------------------------------------------------------
   RX STATE MACHINE
   --------------------------------------------------------- */
static uint8_t  rx_byte;          // one byte buffer for interrupt RX
static uint8_t  rx_state = 0;     // 0..5
static uint8_t  rx_type;
static uint16_t rx_len;
static uint16_t rx_pos;
static uint8_t  rx_checksum;

static uint8_t  rx_payload[1100]; // enough to hold largest frame (image=1024)

/* ---------------------------------------------------------
   FRAME PROCESSOR
   --------------------------------------------------------- */
static void process_frame(uint8_t type, uint16_t len, uint8_t *payload)
{
    if (type == MSG_IMAGE_FULL && len == IMAGE_BUF_SIZE)
    {
        memcpy(g_image_buf, payload, IMAGE_BUF_SIZE);
        g_image_valid = 1;
    }
    else if (type == MSG_TODO_FULL)
    {
        if (len < 1) return;

        uint8_t count = payload[0];
        if (count > MAX_TODOS) count = MAX_TODOS;

        uint16_t pos = 1;
        for (uint8_t i = 0; i < count; i++)
        {
            if (pos + 3 > len) break;

            g_todos[i].id   = payload[pos++];
            g_todos[i].done = payload[pos++];

            uint8_t tl      = payload[pos++];
            if (tl > TODO_TEXT_LEN - 1) tl = TODO_TEXT_LEN - 1;
            if (pos + tl > len) break;

            memcpy(g_todos[i].text, &payload[pos], tl);
            g_todos[i].text[tl] = '\0';
            pos += tl;
        }
        g_todo_count = count;
    }
}

/* ---------------------------------------------------------
   PARSE ONE BYTE (called from ISR)
   --------------------------------------------------------- */
static void parse_rx_byte(uint8_t b)
{
    switch (rx_state)
    {
    case 0: // wait SYNC
        if (b == SYNC_BYTE) {
            rx_state = 1;
            rx_checksum = 0;
        }
        break;

    case 1: // TYPE
        rx_type = b;
        rx_checksum ^= b;
        rx_state = 2;
        break;

    case 2: // LEN_L
        rx_len = b;
        rx_checksum ^= b;
        rx_state = 3;
        break;

    case 3: // LEN_H
        rx_len |= ((uint16_t)b << 8);
        rx_checksum ^= b;

        if (rx_len > sizeof(rx_payload)) {
            rx_state = 0;  // invalid → reset
        } else {
            rx_pos = 0;
            rx_state = 4;
        }
        break;

    case 4: // PAYLOAD
        rx_payload[rx_pos++] = b;
        rx_checksum ^= b;

        if (rx_pos >= rx_len) {
            rx_state = 5;
        }
        break;

    case 5: // CHECKSUM
        if (rx_checksum == b) {
            process_frame(rx_type, rx_len, rx_payload);
        }
        rx_state = 0;
        break;
    }
}

/* ---------------------------------------------------------
   HAL UART INTERRUPT CALLBACK
   --------------------------------------------------------- */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart2)
    {
        parse_rx_byte(rx_byte);

        // Restart interrupt reception
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}

/* ---------------------------------------------------------
   NON-BLOCKING FRAME SENDER
   --------------------------------------------------------- */
static void send_frame(uint8_t type, const uint8_t *payload, uint16_t len)
{
    uint8_t header[4];
    uint8_t checksum = 0;

    header[0] = SYNC_BYTE;
    header[1] = type;
    header[2] = (uint8_t)(len & 0xFF);
    header[3] = (uint8_t)(len >> 8);

    checksum ^= header[1];
    checksum ^= header[2];
    checksum ^= header[3];

    HAL_UART_Transmit(&huart2, header, 4, 50);

    if (payload && len)
    {
        for (uint16_t i = 0; i < len; i++)
            checksum ^= payload[i];

        HAL_UART_Transmit(&huart2, (uint8_t*)payload, len, 50);
    }

    HAL_UART_Transmit(&huart2, &checksum, 1, 50);
}

/* ---------------------------------------------------------
   PUBLIC API FUNCTIONS
   --------------------------------------------------------- */
void comm_send_todo_toggle(uint8_t id)
{
    send_frame(MSG_TODO_TOGGLE, &id, 1);
}

void comm_request_image(void)
{
    send_frame(MSG_REQ_IMAGE, NULL, 0);
}

void comm_request_todos(void)
{
    send_frame(MSG_REQ_TODOS, NULL, 0);
}

/* ---------------------------------------------------------
   COMM TASK (VERY LIGHTWEIGHT)
   ---------------------------------------------------------
   Does NOTHING except yield.
   All real RX happens in UART interrupt callback.
   --------------------------------------------------------- */
static void comm_task(void const *arg)
{
    for (;;)
    {
        osDelay(2);   // keep task alive, do not block CPU
    }
}

/* ---------------------------------------------------------
   INITIALIZER
   --------------------------------------------------------- */
void comm_init(void)
{
    // Start interrupt-based RX
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

    // Start background task
    osThreadDef(commTask, comm_task, osPriorityLow, 0, 256);
    osThreadCreate(osThread(commTask), NULL);
}
