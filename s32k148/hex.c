#include <stdint.h>

static char const chrs[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static char buf[11];

char* hex(uint32_t val) {
    for (int i = 7; i >= 0; i--) {
        buf[i] = chrs[val & 0xF];
        val >>= 4;
    }
    buf[8] = '\n';
    buf[9] = '\r';
    buf[10] = 0;
    return buf;
}
