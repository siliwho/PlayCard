# PlayCard

### **Real-time Embedded UI with Cloud Connectivity and Offline Caching**

This project implements a compact handheld device with a modular application framework, capable of rendering a real-time user interface, running simple games, displaying images, and managing a TODO list. The system integrates a dual-microcontroller architecture combining deterministic UI performance with modern IoT connectivity.

---

## **1. Overview**

The device uses:

- **STM32F411CEU6** as the real-time computation and display controller
- **ESP32-WROOM-32** as the networking, HTTP backend, and persistent storage module
- **SSD1306 OLED display** for user interface
- **Custom UART protocol** for inter-MCU communication

The architecture separates time-critical operations from network-related tasks, ensuring stable frame rendering and predictable behavior irrespective of WiFi conditions.

---

## **2. Key Features**

### STM32 Firmware

- Real-time display rendering using SSD1306
- FreeRTOS-based task scheduling
- Pong, Snake, and menu-driven application system
- GPIO interrupt-driven input handling
- PWM-based buzzer feedback
- Efficient frame buffering and UI drawing
- Deterministic performance independent of WiFi latency

### ESP32 Firmware

- Non-blocking WiFi connectivity
- HTTP GET/POST communication with backend server
- SPIFFS filesystem for persistent offline caching
- JSON parsing for TODO list management
- Binary transfer of image data to STM32
- Automatic fallback to cached data when WiFi is unavailable
- Custom message-framed UART protocol with checksum validation

---

## **3. System Architecture**

### STM32 Responsibilities

- Main application logic
- User interface and display updates
- Input processing
- Execution of games and utilities
- Rendering image and TODO data received from ESP32

### ESP32 Responsibilities

- WiFi connection management
- Backend synchronization (images, TODO list, toggle states)
- File-based offline storage (SPIFFS)
- Data serialization/deserialization
- Responding to STM32 data requests

This separation reflects industry standards for embedded IoT devices where networking and control loops are isolated to ensure stability.

---

## **4. Offline Operation and Memory Handling**

The ESP32 maintains cached copies of backend data in its SPIFFS partition:

- `image.bin` — raw binary image buffer
- `todos.json` — serialized list of tasks

At boot:

1. ESP32 loads both files into RAM if they exist.
2. STM32 requests data as needed.
3. If WiFi is available, ESP32 attempts to fetch updated data from the backend.
4. If WiFi is unavailable, ESP32 immediately returns cached data.

This ensures:

- Instant boot and UI responsiveness
- No dependency on WiFi availability
- Consistent data availability for the STM32
- Reduced backend load due to on-demand fetching

---

## **5. Communication Protocol**

A reliable framed UART protocol is used:

- Sync byte: `0xAA`
- Message type
- Payload length (little endian)
- Payload
- XOR checksum

Supported message types include:

- `MSG_REQ_IMAGE`
- `MSG_REQ_TODOS`
- `MSG_IMAGE_FULL`
- `MSG_TODO_FULL`
- `MSG_TODO_TOGGLE`

Checksum validation ensures corruption detection on noisy UART lines. Failed checksums are discarded and the parser resets to sync state, preventing protocol desynchronization.

---

## **6. Error Handling and Fault Tolerance**

### Networking Errors

- Non-blocking WiFi initiation prevents system stalling.
- HTTP failures fall back to cached SPIFFS data.
- ESP32 logs status codes for debugging (200, 404, 500, timeouts).

### UART Errors

- Checksum mismatches are detected and ignored.
- Invalid or unknown message types do not affect system state.
- Fixed-sized reception buffers prevent overflows.

### Storage Errors

- Missing SPIFFS files trigger safe defaults.
- Corrupted JSON files are detected using `deserializeJson` return codes.
- Partial or invalid binary file reads mark image as invalid.

### STM32 RTOS Stability

- Tasks are designed to avoid long blocking operations.
- Menu loop enforces periodic yielding (`osDelay`) to maintain UI responsiveness.

---

## **7. Hardware Summary**

- STM32F411CEU6 microcontroller
- ESP32-WROOM-32 WiFi module
- SSD1306 128×64 OLED display (I2C)
- Momentary push buttons for UI control
- PWM-driven buzzer
- UART, I2C, GPIO and timer peripherals utilized
- Perfboard-based hardware assembly and soldering

---

## **8. Engineering Skills Demonstrated**

- Real-time embedded development (C, FreeRTOS)
- Low-level driver integration (GPIO, I2C, PWM, UART)
- Reliable inter-MCU communication protocol design
- IoT backend integration using HTTP and JSON
- Efficient use of SPI Flash filesystems (SPIFFS)
- Fault-tolerant system design
- Memory-constrained data handling
- Architecture aligned with industrial IoT practices

---

## **9. Repository Structure**

```
/stm32/
    Core/Src/
    Core/Inc/
    SSD1306/
    Apps/
        menu/
        pong/
        todo/
        image_viewer/
    comm.c

/esp32/
    main.cpp
    storage/
    http_client/
    uart_protocol/
```
