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
