#ifndef COMMON_SPI_H_
#define COMMON_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "dc33_fw_spi.pb-c.h"

/** Caller must call this when a byte is received. Returns true if a message is ready. */
bool spi_handle_byte(uint8_t byte);

/** Caller must free memory with message__free_unpacked(NULL, msg). */
Message* spi_get_message(void);

/** Queues a message for transmission. Actual transmission is done by spi_cb_transmit_byte callback. */
void spi_transmit_message(Message const* msg);

/** Callback: firmware must implement this function. */
void spi_cb_transmit_byte(uint8_t byte);

#ifdef __cplusplus
}
#endif

#endif
