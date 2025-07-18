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
#include "spi.h"
#include "dc33_fw_spi.pb-c.h"

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

    uint8_t in_buf;
    while (true) {
        spi_read_blocking(spi1, 0x7e, &in_buf, 1);
        bool message_ready = spi_handle_byte(in_buf);
        if (message_ready) {
            struct spi_message_data raw = spi_get_message();
            Message* message = message__unpack(NULL, raw.size, raw.data);
            switch (message->message_case) {
            case MESSAGE__MESSAGE_CAN_FRAME:
                printf("%03" PRIX32 "  [%" PRIu32 "] ", message->can_frame->arbitration_id, message->can_frame->dlc);
                for (size_t i = 0; i < message->can_frame->data.len; i++) {
                    printf(" %02X", message->can_frame->data.data[i]);
                }
                printf("\n");
                break;
            default:
                printf("SPI: Unknown message type!\n");
                break;
            }
            message__free_unpacked(message, NULL);
        }
    }
}
