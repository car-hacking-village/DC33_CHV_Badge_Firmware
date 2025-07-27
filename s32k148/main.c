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
#include <LPSPI.h>
#include <LPUART.h>
#include <S32K148.h>
#include <clocks_and_modes.h>
#include <dc33_fw_spi.pb-c.h>
#include <device_registers.h>
#include <led.h>
#include <spi.h>

extern uint32_t RxCODE;
extern uint32_t RxID;
extern uint32_t RxLENGTH;
extern uint32_t RxDATA[2];
extern uint32_t RxTIMESTAMP;

char data = 0;

void PORT_init(void) {
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

    // LEDs
    PCC->PCCn[PCC_PORTA_INDEX] |= PCC_PCCn_CGC_MASK;
    PTA->PDDR |= (1 << 1) | (1 << 0);
    PORTA->PCR[0] = PORT_PCR_MUX(1);
    PORTA->PCR[1] = PORT_PCR_MUX(1);
    PTA->PSOR |= (1 << 1) | (1 << 0);
    PTB->PDDR |= (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8);
    PORTB->PCR[8] = PORT_PCR_MUX(1);
    PORTB->PCR[9] = PORT_PCR_MUX(1);
    PORTB->PCR[10] = PORT_PCR_MUX(1);
    PORTB->PCR[11] = PORT_PCR_MUX(1);
    PTB->PSOR |= (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8);
}

extern char* hex(uint32_t val);

void spi_cb_transmit_byte(uint8_t byte) {
    LPSPI0_transmit_8bits(byte);
    (void)LPSPI0_receive_8bits();
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

    // Flash LEDs
    volatile size_t delay = 0;
    PTA->PCOR |= (1 << 1) | (1 << 0);
    PTB->PCOR |= 1 << 8;
    for (delay = 0; delay < 5000000; delay++)
        ;
    PTA->PSOR |= (1 << 1) | (1 << 0);
    PTB->PSOR |= 1 << 8;
    PTB->PCOR |= (1 << 11) | (1 << 10) | (1 << 9);
    for (delay = 0; delay < 5000000; delay++)
        ;
    PTB->PSOR |= (1 << 11) | (1 << 10) | (1 << 9);
    struct dino_led leds = {
        .left_red = true,
        .left_green = true,
        .left_blue = true,
    };
    LedControl led_control = LED_CONTROL__INIT;
    led_control.leds_case = LED_CONTROL__LEDS_SET_LEDS;
    led_control.set_leds = dino_led_encode(&leds);
    Message message = MESSAGE__INIT;
    message.message_case = MESSAGE__MESSAGE_LED_CONTROL;
    message.led_control = &led_control;
    LPSPI0_transmit_8bits(0x7E);
    (void)LPSPI0_receive_8bits();
    spi_transmit_message(&message);
    for (delay = 0; delay < 5000000; delay++)
        ;
    leds.left_red = false;
    leds.left_green = false;
    leds.left_blue = false;
    leds.right_red = true;
    leds.right_green = true;
    leds.right_blue = true;
    led_control.set_leds = dino_led_encode(&leds);
    spi_transmit_message(&message);
    for (delay = 0; delay < 5000000; delay++)
        ;
    leds.right_red = false;
    leds.right_green = false;
    leds.right_blue = false;
    led_control.set_leds = dino_led_encode(&leds);
    spi_transmit_message(&message);

    for (;;) {
        uint32_t flags = CAN0->IFLAG1;
        if (flags) {
            LPUART1_transmit_string(hex(flags));
        }
        if ((flags >> 4) & 1) {
            LPUART1_transmit_string("CAN frame received!\n\r");
            FLEXCAN0_receive_msg();
            FLEXCAN0_transmit_msg();
            CanFrame can_frame = CAN_FRAME__INIT;
            can_frame.arbitration_id = RxID;
            can_frame.dlc = RxLENGTH;
            can_frame.data.data = (uint8_t*)RxDATA;
            can_frame.data.len = RxLENGTH;
            message.message_case = MESSAGE__MESSAGE_CAN_FRAME;
            message.can_frame = &can_frame;
            spi_transmit_message(&message);
        }
    }
}
