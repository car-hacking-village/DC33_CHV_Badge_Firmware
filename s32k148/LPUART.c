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

#include "./LPUART.h"

#include <device_registers.h>

void LPUART1_init(void) {
    PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(0b010) | PCC_PCCn_CGC_MASK;

    LPUART1->BAUD = LPUART_BAUD_SBR(0xd) | LPUART_BAUD_OSR(15);
    LPUART1->CTRL = LPUART_CTRL_RE_MASK | LPUART_CTRL_TE_MASK;
}

void LPUART1_transmit_char(char send) {
    while ((LPUART1->STAT & LPUART_STAT_TDRE_MASK) >> LPUART_STAT_TDRE_SHIFT == 0) { }
    LPUART1->DATA = send;
}

void LPUART1_transmit_string(char data_string[]) {
    uint32_t i = 0;
    while (data_string[i] != '\0') {
        LPUART1_transmit_char(data_string[i]);
        i++;
    }
}

char LPUART1_receive_char(void) {
    char receive;
    if (LPUART1->STAT & LPUART_STAT_OR_MASK) {
        PTB->PCOR = 1 << 9;
    }
    while ((LPUART1->STAT & LPUART_STAT_RDRF_MASK) >> LPUART_STAT_RDRF_SHIFT == 0) { }
    receive = LPUART1->DATA;
    PTB->PTOR = 1 << 11;
    return receive;
}

void LPUART1_receive_and_echo_char(void) {
    char send = LPUART1_receive_char();
    LPUART1_transmit_char(send);
    LPUART1_transmit_char('\n');
    LPUART1_transmit_char('\r');
}
