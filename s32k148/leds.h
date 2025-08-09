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

#ifndef LEDS_H_
#define LEDS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <S32K148.h>

enum {
    PTA0 = 0,
    PTA1 = 1,
    PTB8 = 8,
    PTB9 = 9,
    PTB10 = 10,
    PTB11 = 11,
    FRED_LEFT_R = 1 << PTB8,
    FRED_LEFT_G = 1 << PTA1,
    FRED_LEFT_B = 1 << PTA0,
    FRED_RIGHT_R = 1 << PTB11,
    FRED_RIGHT_G = 1 << PTB10,
    FRED_RIGHT_B = 1 << PTB9,
};

static inline void leds_init(void) {
    PCC->PCCn[PCC_PORTA_INDEX] |= PCC_PCCn_CGC_MASK;  // Enable clock for PORTA
    PTA->PDDR |= (1 << 1) | (1 << 0);  // Ports A0 & A1: Data Direction = output
    PORTA->PCR[0] = PORT_PCR_MUX(1);  // Port A0: MUX = GPIO
    PORTA->PCR[1] = PORT_PCR_MUX(1);  // Port A1: MUX = GPIO
    PTA->PSOR |= (1 << 1) | (1 << 0);  // Set outputs on ports A0 & A1 (LED off)
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;  // Enable clock for PORTB
    PTB->PDDR |= (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8);  // Ports B8, B9, B10, & B11: Data Direction = output
    PORTB->PCR[8] = PORT_PCR_MUX(1);  // Port B8: MUX = GPIO
    PORTB->PCR[9] = PORT_PCR_MUX(1);  // Port B9: MUX = GPIO
    PORTB->PCR[10] = PORT_PCR_MUX(1);  // Port B10: MUX = GPIO
    PORTB->PCR[11] = PORT_PCR_MUX(1);  // Port B11: MUX = GPIO
    PTB->PSOR |= (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8);  // Set outputs on ports B8, B9, B10, & B11 (LED off)
}

static inline void leds_on(int leds) {
    if (leds & (FRED_LEFT_G | FRED_LEFT_B)) {
        PTA->PCOR = leds & (FRED_LEFT_G | FRED_LEFT_B);
    }
    if (leds & (FRED_LEFT_R | FRED_RIGHT_R | FRED_RIGHT_G | FRED_RIGHT_B)) {
        PTB->PCOR = leds & (FRED_LEFT_R | FRED_RIGHT_R | FRED_RIGHT_G | FRED_RIGHT_B);
    }
}

static inline void leds_off(int leds) {
    if (leds & (FRED_LEFT_G | FRED_LEFT_B)) {
        PTA->PSOR = leds & (FRED_LEFT_G | FRED_LEFT_B);
    }
    if (leds & (FRED_LEFT_R | FRED_RIGHT_R | FRED_RIGHT_G | FRED_RIGHT_B)) {
        PTB->PSOR = leds & (FRED_LEFT_R | FRED_RIGHT_R | FRED_RIGHT_G | FRED_RIGHT_B);
    }
}

static inline void leds_toggle(int leds) {
    if (leds & (FRED_LEFT_G | FRED_LEFT_B)) {
        PTA->PTOR = leds & (FRED_LEFT_G | FRED_LEFT_B);
    }
    if (leds & (FRED_LEFT_R | FRED_RIGHT_R | FRED_RIGHT_G | FRED_RIGHT_B)) {
        PTB->PTOR = leds & (FRED_LEFT_R | FRED_RIGHT_R | FRED_RIGHT_G | FRED_RIGHT_B);
    }
}

#ifdef __cplusplus
}
#endif

#endif
