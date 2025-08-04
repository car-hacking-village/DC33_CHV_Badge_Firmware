/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "led.h"
#include "spi.h"
#include "dc33_fw_spi.pb-c.h"

enum {
    DINO_LEFT_R = 20,
    DINO_LEFT_G = 19,
    DINO_LEFT_B = 18,

    DINO_RIGHT_R = 28,
    DINO_RIGHT_G = 27,
    DINO_RIGHT_B = 26,
};

int main() {
    stdio_init_all();
    printf("Init\n");
    spi_init(spi1, 1000 * 1000);
    spi_set_slave(spi1, true);
    gpio_set_function(9, GPIO_FUNC_SPI);
    gpio_set_function(10, GPIO_FUNC_SPI);
    gpio_set_function(11, GPIO_FUNC_SPI);
    gpio_set_function(12, GPIO_FUNC_SPI);
    bi_decl(bi_4pins_with_func(9, 10, 11, 12, GPIO_FUNC_SPI));

    // LEDs
    gpio_init(DINO_LEFT_R);
    gpio_init(DINO_LEFT_G);
    gpio_init(DINO_LEFT_B);
    gpio_init(DINO_RIGHT_R);
    gpio_init(DINO_RIGHT_G);
    gpio_init(DINO_RIGHT_B);
    gpio_set_dir(DINO_LEFT_R, true);
    gpio_set_dir(DINO_LEFT_G, true);
    gpio_set_dir(DINO_LEFT_B, true);
    gpio_set_dir(DINO_RIGHT_R, true);
    gpio_set_dir(DINO_RIGHT_G, true);
    gpio_set_dir(DINO_RIGHT_B, true);
    gpio_put(DINO_LEFT_R, true);
    gpio_put(DINO_LEFT_G, true);
    gpio_put(DINO_LEFT_B, true);
    gpio_put(DINO_RIGHT_R, true);
    gpio_put(DINO_RIGHT_G, true);
    gpio_put(DINO_RIGHT_B, true);

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
                struct dino_led leds = dino_led_decode(message->led_control->set_leds);
                gpio_put(DINO_LEFT_R, !leds.left_red);
                gpio_put(DINO_LEFT_G, !leds.left_green);
                gpio_put(DINO_LEFT_B, !leds.left_blue);
                gpio_put(DINO_RIGHT_R, !leds.right_red);
                gpio_put(DINO_RIGHT_G, !leds.right_green);
                gpio_put(DINO_RIGHT_B, !leds.right_blue);
                break;
            default:
                printf("SPI: Unknown message type!\n");
                break;
            }
            message__free_unpacked(message, NULL);
        }
    }
}
