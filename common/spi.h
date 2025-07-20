#ifndef COMMON_SPI_H_
#define COMMON_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "dc33_fw_spi.pb-c.h"

struct spi_message_data {
    uint8_t* data;
    size_t size;
};

bool spi_handle_byte(uint8_t byte);
struct spi_message_data spi_get_message(void);

void spi_transmit_message(Message const* msg);

#ifdef __cplusplus
}
#endif

#endif
