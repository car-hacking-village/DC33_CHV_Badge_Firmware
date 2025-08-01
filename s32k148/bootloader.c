/*
 * Copyright (c) 2014 - 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016 - 2018, NXP.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <LPUART.h>
#include <clocks_and_modes.h>
#include <device_registers.h>
#include <fsl_flash_driver_c90tfs.h>

enum {
    APP_START_ADDRESS = 0x1000,
};

static void PORT_init(void) {
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;  // Enable clock for PORTC
    PORTC->PCR[6] |= PORT_PCR_MUX(2);  // Port C6: MUX = ALT2, UART1 RX
    PORTC->PCR[7] |= PORT_PCR_MUX(2);  // Port C7: MUX = ALT2, UART1 TX
}

static void JumpToUserApplication(unsigned int userSP, unsigned int userStartup) {
    // Check if Entry address is erased and return if erased
    if (userSP == 0xFFFFFFFF) {
        return;
    }

    // Set up stack pointer
    __asm("msr msp, r0");
    __asm("msr psp, r0");

    // Relocate vector table
    S32_SCB->VTOR = (uint32_t)APP_START_ADDRESS;

    // Jump to application PC (r1)
    __asm("mov pc, r1");
}

static void junk(void) {
    for (;;) {
        switch (LPUART1_receive_char()) {
        case '\r':
        case '\n':
            LPUART1_transmit_char('!');
            return;
        default:
            break;
        }
    }
}

static void ignore(void) {
    for (;;) {
        switch (LPUART1_receive_char()) {
        case '\r':
        case '\n':
            LPUART1_transmit_char('.');
            return;
        default:
            break;
        }
    }
}

static unsigned char buf[255];

static int hex(void) {
    uint8_t val = 0;
    char c = LPUART1_receive_char();
    switch (c) {
    case '0' ... '9':
        val = (c - '0') << 4;
        break;
    case 'A' ... 'F':
        val = (c - 'A' + 10) << 4;
        break;
    case '\r':
    case '\n':
        LPUART1_transmit_char('!');
        return -1;
    default:
        junk();
        return -1;
    }
    c = LPUART1_receive_char();
    switch (c) {
    case '0' ... '9':
        val |= (c - '0') & 0xF;
        break;
    case 'A' ... 'F':
        val |= (c - 'A' + 10) & 0xF;
        break;
    case '\r':
    case '\n':
        LPUART1_transmit_char('!');
        return -1;
    default:
        junk();
        return -1;
    }
    return val;
}

static void flash(uintptr_t ptr, uint8_t* buf, size_t len) {
    static const flash_user_config_t user_config = {
        .PFlashBase = 0x00000000U,
        .PFlashSize = 0x00180000U,
        .DFlashBase = 0x10000000U,
        .EERAMBase = 0x14000000U,
        .CallBack = NULL_CALLBACK,
    };
    static bool is_mem_init = false;
    static flash_ssd_config_t ssd_config;
    if (!is_mem_init) {
        FlashInit(&user_config, &ssd_config);
    }
    if (ptr < 0x1000) {
        // don't allow flashing over bootloader!
        return;
    }
    if (ptr < 0x10001000 && (ptr + len) >= 0x10000000) {
        // don't allow flashing over bootloader!
        return;
    }
    if ((ptr + len) >= 0x00180000 && ptr < 0x10000000) {
        // invalid flash memory address
        return;
    }
    if ((ptr + len) >= 0x10080000) {
        // invalid flash memory address
        return;
    }
    if (ptr < 0x00180000) {
        static uint8_t erased[48] = { 0 };
        uintptr_t page = ptr >> 12;
        if (!(erased[page >> 3] & (1 << (page & 7)))) {
            FlashEraseSector(&ssd_config, ptr & (~(uintptr_t)0xFFF), 4096, FlashCommandSequence);
            erased[page >> 3] |= (1 << (page & 7));
        }
        if ((ptr + len) >> 12 != page) {
            page += 1;
            if (!(erased[page >> 3] & (1 << (page & 7)))) {
                FlashEraseSector(&ssd_config, ptr & (~(uintptr_t)0xFFF), 4096, FlashCommandSequence);
                erased[page >> 3] |= (1 << (page & 7));
            }
        }
    }
    if (ptr >= 0x10000000) {
        static uint8_t erased[16] = { 0 };
        uintptr_t page = (ptr & 0x0FFFFFFF) >> 12;
        if (!(erased[page >> 3] & (1 << (page & 7)))) {
            FlashEraseSector(&ssd_config, ptr & (~(uintptr_t)0xFFF), 4096, FlashCommandSequence);
            erased[page >> 3] |= (1 << (page & 7));
        }
        if ((ptr + len) >> 12 != page) {
            page += 1;
            if (!(erased[page >> 3] & (1 << (page & 7)))) {
                FlashEraseSector(&ssd_config, ptr & (~(uintptr_t)0xFFF), 4096, FlashCommandSequence);
                erased[page >> 3] |= (1 << (page & 7));
            }
        }
    }
    FlashProgram(&ssd_config, ptr, len, buf, FlashCommandSequence);
}

static void handle_s1(void) {
    uint8_t recsz = 0;
    uintptr_t ptr = 0;
    uint8_t check = 0;
    int max = 256;
    for (int pos = 0; pos < max; pos++) {
        int h = 0;
        if ((h = hex()) < 0) {
            return;
        }
        uint8_t val = h & 0xFF;
        switch (pos) {
        case 0:
            recsz = val;
            if (recsz < 3) {
                junk();
                return;
            }
            max = recsz + 1;
            check += val;
            break;
        case 1:
            ptr = val << 8;
            check += val;
            break;
        case 2:
            ptr |= val;
            check += val;
            break;
        default:
            if (pos < recsz) {
                buf[pos - 3] = val;
                check += val;
            } else if (pos == recsz) {
                if ((~check & 0xFF) != val) {
                    junk();
                    return;
                }
                break;
            } else {
                junk();
                return;
            }
            break;
        }
    }
    switch (LPUART1_receive_char()) {
    case '\r':
    case '\n':
        flash(ptr, buf, recsz - 3);
        LPUART1_transmit_char('.');
        break;
    default:
        junk();
        break;
    }
}

static void handle_s2(void) {
    uint8_t recsz = 0;
    uintptr_t ptr = 0;
    uint8_t check = 0;
    int max = 256;
    for (int pos = 0; pos < max; pos++) {
        int h = 0;
        if ((h = hex()) < 0) {
            return;
        }
        uint8_t val = h & 0xFF;
        switch (pos) {
        case 0:
            recsz = val;
            if (recsz < 4) {
                junk();
                return;
            }
            max = recsz + 1;
            check += val;
            break;
        case 1:
            ptr = val << 16;
            check += val;
            break;
        case 2:
            ptr |= val << 8;
            check += val;
            break;
        case 3:
            ptr |= val;
            check += val;
            break;
        default:
            if (pos < recsz) {
                buf[pos - 4] = val;
                check += val;
            } else if (pos == recsz) {
                if ((~check & 0xFF) != val) {
                    junk();
                    return;
                }
                break;
            } else {
                junk();
                return;
            }
            break;
        }
    }
    switch (LPUART1_receive_char()) {
    case '\r':
    case '\n':
        flash(ptr, buf, recsz - 4);
        LPUART1_transmit_char('.');
        break;
    default:
        junk();
        break;
    }
}

static void handle_s3(void) {
    uint8_t recsz = 0;
    uintptr_t ptr = 0;
    uint8_t check = 0;
    int max = 256;
    for (int pos = 0; pos < max; pos++) {
        int h = 0;
        if ((h = hex()) < 0) {
            return;
        }
        uint8_t val = h & 0xFF;
        switch (pos) {
        case 0:
            recsz = val;
            if (recsz < 5) {
                junk();
                return;
            }
            max = recsz + 1;
            check += val;
            break;
        case 1:
            ptr = val << 24;
            check += val;
            break;
        case 2:
            ptr |= val << 16;
            check += val;
            break;
        case 3:
            ptr |= val << 8;
            check += val;
            break;
        case 4:
            ptr |= val;
            check += val;
            break;
        default:
            if (pos < recsz) {
                buf[pos - 5] = val;
                check += val;
            } else if (pos == recsz) {
                if ((~check & 0xFF) != val) {
                    junk();
                    return;
                }
                break;
            } else {
                junk();
                return;
            }
            break;
        }
    }
    switch (LPUART1_receive_char()) {
    case '\r':
    case '\n':
        flash(ptr, buf, recsz - 5);
        LPUART1_transmit_char('.');
        break;
    default:
        junk();
        break;
    }
}

static bool handle_s(void) {
    switch (LPUART1_receive_char()) {
    case '0':
        ignore();
        break;
    case '1':
        handle_s1();
        break;
    case '2':
        handle_s2();
        break;
    case '3':
        handle_s3();
        break;
    case '4':
        junk();
        break;
    case '5':
    case '6':
        ignore();
        break;
    case '7':
    case '8':
    case '9':
        ignore();
        return true;
    case '\r':
    case '\n':
        LPUART1_transmit_char('!');
        break;
    default:
        junk();
        break;
    }
    return false;
}

int main(void) {
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    PORT_init();

    LPUART1_init();
    LPUART1_transmit_char('?');

    for (volatile size_t delay = 0; delay < 5000000; delay++) {
        if ((LPUART1->STAT & LPUART_STAT_RDRF_MASK) >> LPUART_STAT_RDRF_SHIFT) {
            for (;;) {
                switch (LPUART1_receive_char()) {
                case 'S':
                    if (handle_s()) {
                        LPUART1_transmit_char('=');
                        JumpToUserApplication(*((uint32_t*)APP_START_ADDRESS), *((uint32_t*)(APP_START_ADDRESS + 4)));
                        for (;;) { };
                    }
                    break;
                case '\r':
                case '\n':
                    break;
                default:
                    junk();
                    break;
                }
            }
        }
    }

    LPUART1_transmit_char('=');
    JumpToUserApplication(*((uint32_t*)APP_START_ADDRESS), *((uint32_t*)(APP_START_ADDRESS + 4)));
    for (;;) { };
    return 0;
}
