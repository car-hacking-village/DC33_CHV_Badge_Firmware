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

#include <FlexCAN.h>
#include <S32K148.h>
#include <clocks_and_modes.h>
#include <dc33_fw_spi.pb-c.h>
#include <device_registers.h>
#include <led.h>
#include <spi.h>

#include "./LPSPI.h"
#include "./LPUART.h"
#include "./led.h"

extern uint32_t RxCODE;
extern uint32_t RxID;
extern uint32_t RxLENGTH;
extern uint32_t RxDATA[2];
extern uint32_t RxTIMESTAMP;

char data = 0;

static void PORT_init(void) {
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;  // Enable clock for PORTB
    PORTB->PCR[0] |= PORT_PCR_MUX(3);  // Port B0: MUX = ALT3, LPSPI0_PCS0
    PORTB->PCR[1] |= PORT_PCR_MUX(3);  // Port B1: MUX = ALT3, LPSPI0_SOUT
    PORTB->PCR[2] |= PORT_PCR_MUX(3);  // Port B2: MUX = ALT3, LPSPI0_SCK
    PORTB->PCR[3] |= PORT_PCR_MUX(3);  // Port B3: MUX = ALT3, LPSPI0_SIN
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;  // Enable clock for PORTC
    PORTC->PCR[6] |= PORT_PCR_MUX(2);  // Port C6: MUX = ALT2, UART1 RX
    PORTC->PCR[7] |= PORT_PCR_MUX(2);  // Port C7: MUX = ALT2, UART1 TX
    PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK;  // Enable clock for PORTE
    PORTE->PCR[4] |= PORT_PCR_MUX(5);  // Port E4: MUX = ALT5, CAN0_RX
    PORTE->PCR[5] |= PORT_PCR_MUX(5);  // Port E5: MUX = ALT5, CAN0_TX

    leds_init();
}

extern char* hex(uint32_t val);

void spi_cb_transmit_byte(uint8_t byte) {
    LPSPI0_transmit_8bits(byte);
    (void)LPSPI0_receive_8bits();
}

__attribute__((noreturn)) static void sw_reset(void) {
    S32_SCB->AIRCR = S32_SCB_AIRCR_VECTKEY(0x5FA) | S32_SCB_AIRCR_SYSRESETREQ_MASK;
    for (;;) { }
}

int main(void) {
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    FLEXCAN0_init();
    PORT_init();

    FLEXCAN0_transmit_msg();
    LPUART1_init();
    LPUART1_transmit_string("Running LPUART example\n\r");
    LPUART1_transmit_string("Input character to echo...\n\r");
    LPSPI0_init_master();

    // Clear any partial messages before reset
    LPSPI0_transmit_8bits(0x7E);
    (void)LPSPI0_receive_8bits();

    // Flash LEDs
    leds_on(FRED_LEFT_R | FRED_LEFT_G | FRED_LEFT_B);
    for (volatile size_t delay = 0; delay < 5000000; delay++) { };
    leds_off(FRED_LEFT_R | FRED_LEFT_G | FRED_LEFT_B);
    leds_on(FRED_RIGHT_R | FRED_RIGHT_G | FRED_RIGHT_B);
    for (volatile size_t delay = 0; delay < 5000000; delay++) { };
    leds_off(FRED_RIGHT_R | FRED_RIGHT_G | FRED_RIGHT_B);
    LedControl led_control = LED_CONTROL__INIT;
    led_control.leds_case = LED_CONTROL__LEDS_SET_LEDS;
    led_control.set_leds = DINO_LEFT_R_P | DINO_LEFT_G_P | DINO_LEFT_B_P;
    Message message = MESSAGE__INIT;
    message.message_case = MESSAGE__MESSAGE_LED_CONTROL;
    message.led_control = &led_control;
    spi_transmit_message(&message);
    for (volatile size_t delay = 0; delay < 5000000; delay++) { };
    led_control.set_leds = DINO_RIGHT_R_P | DINO_RIGHT_G_P | DINO_RIGHT_B_P;
    spi_transmit_message(&message);
    for (volatile size_t delay = 0; delay < 5000000; delay++) { };
    led_control.set_leds = 0;
    spi_transmit_message(&message);

    for (volatile size_t tick = 0;; tick++) {
        if (LPUART1->STAT & LPUART_STAT_RDRF_MASK) {
            // TODO: start an SPI transfer
            char c = LPUART1_receive_char();
            if (c == 'B') {
                sw_reset();
            }
        }
        if (tick & 0x00100000) {
            leds_on(FRED_LEFT_R | FRED_LEFT_G | FRED_LEFT_B);
        } else {
            leds_off(FRED_LEFT_R | FRED_LEFT_G | FRED_LEFT_B);
        }
    }
}
