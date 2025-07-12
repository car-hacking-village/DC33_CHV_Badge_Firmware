/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <inttypes.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

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
    sleep_ms(1000);

    uint8_t out_buf[2], in_buf[2];
    while (true) {
        printf("Wait read...\n");
        sleep_ms(1000);
        spi_read_blocking(spi1, 0, in_buf, 2);
        printf("Read from SPI! %02" PRIx8 " %02" PRIx8 "\n", in_buf[0], in_buf[1]);
    }
}
