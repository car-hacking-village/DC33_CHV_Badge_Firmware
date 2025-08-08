/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <hardware/spi.h>
#include <hardware/uart.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>
#include <task.h>

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

void main_task(void* param) {
    (void)param;
    printf("Init\n");
    gpio_set_function(5, UART_FUNCSEL_NUM(uart1, 5));
    gpio_set_function(6, UART_FUNCSEL_NUM(uart1, 6));
    uart_init(uart1, 38400);
    spi_init(spi1, 1000 * 1000);
    spi_set_slave(spi1, true);
    gpio_set_function_masked((1 << 9) | (1 << 10) | (1 << 11) | (1 << 12), GPIO_FUNC_SPI);
    bi_decl(bi_4pins_with_func(9, 10, 11, 12, GPIO_FUNC_SPI));

    // LEDs
    gpio_init_mask(LED_MASK);
    gpio_set_dir_out_masked(LED_MASK);
    gpio_put_masked(LED_MASK, LED_MASK);

    uint8_t in_buf;
    while (true) {
        spi_read_blocking(spi1, 0x7e, &in_buf, 1);
        bool message_ready = spi_handle_byte(in_buf);
        if (message_ready) {
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

int main(void) {
    stdio_init_all();
    printf("Main\n");
    static StackType_t stack[configMINIMAL_STACK_SIZE * 2];
    static StaticTask_t task;
    TaskHandle_t handle = xTaskCreateStatic(main_task, "main_task", configMINIMAL_STACK_SIZE * 2, NULL, 5, stack, &task);
    vTaskStartScheduler();
    for (;;) { }
}
