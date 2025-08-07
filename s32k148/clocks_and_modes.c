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

#include "./clocks_and_modes.h"

#include <device_registers.h>

void SOSC_init_8MHz(void) {
    SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1) | SCG_SOSCDIV_SOSCDIV2(1);
    SCG->SOSCCFG = SCG_SOSCCFG_RANGE(2);
    while (SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK) { }
    SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK;
    while (!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK)) { }
}

void SPLL_init_160MHz(void) {
    while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK) { }
    SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;
    SCG->SPLLDIV |= SCG_SPLLDIV_SPLLDIV1(2) | SCG_SPLLDIV_SPLLDIV2(3);
    SCG->SPLLCFG = SCG_SPLLCFG_MULT(24);
    while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK) { }
    SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;
    while (!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)) { }
}

void NormalRUNmode_80MHz(void) {
    SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(1) | SCG_SIRCDIV_SIRCDIV2(1);
    SCG->RCCR = SCG_RCCR_SCS(6) | SCG_RCCR_DIVCORE(0b01) | SCG_RCCR_DIVBUS(0b01) | SCG_RCCR_DIVSLOW(0b10);
    while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT) != 6) { }
}
