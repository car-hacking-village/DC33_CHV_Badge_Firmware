#include "./spi.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static uint8_t buffer[256];
static size_t length = 0;
static bool ready = false;
static bool escape = false;

static void reset(void) {
    length = 0;
    ready = false;
    escape = false;
}

bool spi_handle_byte(uint8_t byte) {
    if (ready) {
        reset();
    }
    switch (byte) {
    case 0x7e:
        escape = false;
        if (length > 2 && length <= sizeof(buffer)) {
            // TODO: check CRC
            length -= 2;
            ready = true;
            return true;
        }
        reset();
        return false;
    case 0x7d:
        escape = true;
        return false;
    default:
        if (length < sizeof(buffer)) {
            buffer[length++] = byte ^ (escape ? 0x20 : 0);
            escape = false;
        }
        return false;
    }
}

struct spi_message_data spi_get_message(void) {
    struct spi_message_data result = { NULL, 0 };
    if (ready) {
        result.data = buffer;
        result.size = length;
    }
    return result;
}

void spi_cb_transmit_byte(uint8_t byte);

void spi_transmit_message(Message const* msg) {
    size_t size = message__get_packed_size(msg);
    if (size <= 256) {
        message__pack(msg, buffer);
    }
    // TODO: calculate CRC
    for (size_t i = 0; i < size; i++) {
        switch (buffer[i]) {
        case 0x7D:
            spi_cb_transmit_byte(0x7D);
            spi_cb_transmit_byte(0x5D);
            break;
        case 0x7E:
            spi_cb_transmit_byte(0x7D);
            spi_cb_transmit_byte(0x5E);
            break;
        default:
            spi_cb_transmit_byte(buffer[i]);
            break;
        }
    }
    spi_cb_transmit_byte(0);
    spi_cb_transmit_byte(0);
    spi_cb_transmit_byte(0x7E);
}
