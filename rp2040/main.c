/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <bsp/board_api.h>
#include <hardware/spi.h>
#include <hardware/uart.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>
#include <queue.h>
#include <task.h>
#include <tusb.h>

#include "dc33_fw_spi.pb-c.h"
#include "led.h"
#include "spi.h"

enum {
    DINO_LEFT_R = 20,
    DINO_LEFT_G = 19,
    DINO_LEFT_B = 18,

    DINO_RIGHT_R = 28,
    DINO_RIGHT_G = 27,
    DINO_RIGHT_B = 26,

    LED_MASK = (1 << DINO_LEFT_R) | (1 << DINO_LEFT_G) | (1 << DINO_LEFT_B) | (1 << DINO_RIGHT_R) | (1 << DINO_RIGHT_G) | (1 << DINO_RIGHT_B),
    LEFT_MASK = DINO_LEFT_R_P | DINO_LEFT_G_P | DINO_LEFT_B_P,
    RIGHT_MASK = DINO_RIGHT_R_P | DINO_RIGHT_G_P | DINO_RIGHT_B_P,
};

static QueueHandle_t spi_queue;

static inline void start_spi_queue(void) {
    static uint8_t buf[256];
    static StaticQueue_t queue;
    spi_queue = xQueueCreateStatic(sizeof(buf) / sizeof(*buf), sizeof(*buf), buf, &queue);
}

static void spi_reader(void* param) {
    (void)param;
    for (;;) {
        uint8_t byte;
        spi_read_blocking(spi1, 0x7e, &byte, 1);
        xQueueSend(spi_queue, &byte, 0);
    }
}

static inline void start_spi_reader(void) {
    static StackType_t stack[configMINIMAL_STACK_SIZE * 2];
    static StaticTask_t task;
    (void)xTaskCreateStatic(spi_reader, "spi_reader", sizeof(stack) / sizeof(*stack), NULL, 5, stack, &task);
}

static void message_handler(void* param) {
    (void)param;
    for (;;) {
        uint8_t byte;
        if (xQueueReceive(spi_queue, &byte, portMAX_DELAY)) {
            if (spi_handle_byte(byte)) {
                Message* message = spi_get_message();
                switch (message->message_case) {
                case MESSAGE__MESSAGE_CAN_FRAME:
                    printf("%03" PRIX32 "  [%" PRIu32 "] ", message->can_frame->arbitration_id, message->can_frame->dlc);
                    for (size_t i = 0; i < message->can_frame->data.len; i++) {
                        printf(" %02X", message->can_frame->data.data[i]);
                    }
                    printf("\n");
                    break;
                case MESSAGE__MESSAGE_LED_CONTROL:
                    // TODO: add support for and, or, xor
                    gpio_put_masked(LED_MASK, ~(((message->led_control->set_leds & LEFT_MASK) << 15) | ((message->led_control->set_leds & RIGHT_MASK) << 26)));
                    break;
                default:
                    printf("SPI: Unknown message type!\n");
                    break;
                }
                message__free_unpacked(message, NULL);
            }
        }
    }
}

static inline void start_message_handler(void) {
    static StackType_t stack[configMINIMAL_STACK_SIZE * 2];
    static StaticTask_t task;
    (void)xTaskCreateStatic(message_handler, "message_handler", sizeof(stack) / sizeof(*stack), NULL, 10, stack, &task);
}

static QueueHandle_t cdc_queue;

static inline void start_cdc_queue(void) {
    static uint8_t buf[256];
    static StaticQueue_t queue;
    cdc_queue = xQueueCreateStatic(sizeof(buf) / sizeof(*buf), sizeof(*buf), buf, &queue);
}

static void usb_reader(void* param) {
    (void)param;
    for (;;) {
        tud_task();
    }
}

void tud_cdc_rx_cb(uint8_t itf) {
    uint8_t byte;
    while (tud_cdc_n_read(itf, &byte, 1)) {
        xQueueSend(cdc_queue, &byte, 0);
    }
}

static inline void start_usb_reader(void) {
    static StackType_t stack[configMINIMAL_STACK_SIZE * 2];
    static StaticTask_t task;
    (void)xTaskCreateStatic(usb_reader, "usb_reader", sizeof(stack) / sizeof(*stack), NULL, 5, stack, &task);
}

static void uart_flasher(void* param) {
    (void)param;
    for (;;) {
        uint8_t byte;
        if (xQueueReceive(cdc_queue, &byte, portMAX_DELAY)) {
            uart_write_blocking(uart1, &byte, 1);
        }
    }
}

static inline void start_uart_flasher(void) {
    static StackType_t stack[configMINIMAL_STACK_SIZE * 2];
    static StaticTask_t task;
    (void)xTaskCreateStatic(uart_flasher, "uart_flasher", sizeof(stack) / sizeof(*stack), NULL, 15, stack, &task);
}

static void uart_reader(void* param) {
    (void)param;
    for (;;) {
        uint8_t byte;
        uart_read_blocking(uart1, &byte, 1);
        tud_cdc_n_write(0, &byte, 1);
        tud_cdc_n_write_flush(0);
    }
}

static inline void start_uart_reader(void) {
    static StackType_t stack[configMINIMAL_STACK_SIZE * 2];
    static StaticTask_t task;
    (void)xTaskCreateStatic(uart_reader, "uart_reader", sizeof(stack) / sizeof(*stack), NULL, 5, stack, &task);
}

int main(void) {
    gpio_set_function(4, UART_FUNCSEL_NUM(uart1, 4));
    gpio_set_function(5, UART_FUNCSEL_NUM(uart1, 5));
    uart_init(uart1, 38400);
    bi_decl(bi_2pins_with_func(4, 5, GPIO_FUNC_UART));

    spi_init(spi1, 1000 * 1000);
    spi_set_slave(spi1, true);
    gpio_set_function_masked((1 << 9) | (1 << 10) | (1 << 11) | (1 << 12), GPIO_FUNC_SPI);
    bi_decl(bi_4pins_with_func(9, 10, 11, 12, GPIO_FUNC_SPI));

    // LEDs
    gpio_init_mask(LED_MASK);
    gpio_set_dir_out_masked(LED_MASK);
    gpio_put_masked(LED_MASK, LED_MASK);

    board_init();
    tusb_init();
    if (board_init_after_tusb) {
        board_init_after_tusb();
    }
    stdio_init_all();
    printf("Main\n");

    start_spi_queue();
    start_spi_reader();
    start_message_handler();
    start_cdc_queue();
    start_usb_reader();
    start_uart_flasher();
    start_uart_reader();

    vTaskStartScheduler();
    for (;;) { }
}
