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

#include "./LPSPI.h"

#include <device_registers.h>

void LPSPI0_init_master(void) {
    PCC->PCCn[PCC_LPSPI0_INDEX] = 0;
    PCC->PCCn[PCC_LPSPI0_INDEX] = PCC_PCCn_PR_MASK | PCC_PCCn_CGC_MASK | PCC_PCCn_PCS(6);
    LPSPI0->CR = 0x00000000;
    LPSPI0->IER = 0x00000000;
    LPSPI0->DER = 0x00000000;
    LPSPI0->CFGR0 = 0x00000000;
    LPSPI0->CFGR1 = LPSPI_CFGR1_MASTER_MASK;
    LPSPI0->TCR = LPSPI_TCR_CPHA_MASK | LPSPI_TCR_PRESCALE(2) | LPSPI_TCR_PCS(0) | LPSPI_TCR_FRAMESZ(7);
    LPSPI0->CCR = LPSPI_CCR_SCKPCS(4) | LPSPI_CCR_PCSSCK(4) | LPSPI_CCR_DBT(8) | LPSPI_CCR_SCKDIV(8);
    LPSPI0->FCR = LPSPI_FCR_TXWATER(3);
    LPSPI0->CR = LPSPI_CR_MEN_MASK | LPSPI_CR_DBGEN_MASK;
}

void LPSPI0_transmit_8bits(uint8_t send) {
    while ((LPSPI0->SR & LPSPI_SR_TDF_MASK) >> LPSPI_SR_TDF_SHIFT == 0)
        ;
    LPSPI0->TDR = send;
    LPSPI0->SR |= LPSPI_SR_TDF_MASK;
}

uint8_t LPSPI0_receive_8bits(void) {
    while ((LPSPI0->SR & LPSPI_SR_RDF_MASK) >> LPSPI_SR_RDF_SHIFT == 0)
        ;
    uint8_t recieve = LPSPI0->RDR;
    LPSPI0->SR |= LPSPI_SR_RDF_MASK;
    return recieve;
}
