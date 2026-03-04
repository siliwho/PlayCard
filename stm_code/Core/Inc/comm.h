#ifndef COMM_H
#define COMM_H

#include <stdint.h>

#define IMAGE_BUF_SIZE  1024
#define MAX_TODOS       8
#define TODO_TEXT_LEN   24

typedef struct {
    char    text[TODO_TEXT_LEN];
    uint8_t done;   // 0 = not done, 1 = done
    uint8_t id;     // unique id used by backend
} TodoItem;

/* Shared data updated by comm_task (from ESP32) */
extern uint8_t  g_image_buf[IMAGE_BUF_SIZE];
extern uint8_t  g_image_valid;    // 0 = none, 1 = valid image

extern TodoItem g_todos[MAX_TODOS];
extern uint8_t  g_todo_count;

/* Init & background RX task */
void comm_init(void);

/* Outgoing commands to ESP32 */
void comm_send_todo_toggle(uint8_t id);
void comm_request_image(void);
void comm_request_todos(void);

#endif
