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
#include <FreeRTOS.h>
#include <S32K148.h>
#include <dc33_fw_spi.pb-c.h>
#include <device_registers.h>
#include <led.h>
#include <spi.h>
#include <task.h>

#include "./LPSPI.h"
#include "./LPUART.h"
#include "./clocks_and_modes.h"
#include "./leds.h"
#include "./script.h"

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

void spi_cb_transmit_byte(uint8_t byte) {
    LPSPI0_transmit_8bits(byte);
    (void)LPSPI0_receive_8bits();
}

__attribute__((noreturn)) static inline void sw_reset(void) {
    S32_SCB->AIRCR = S32_SCB_AIRCR_VECTKEY(0x5FA) | S32_SCB_AIRCR_SYSRESETREQ_MASK;
    for (;;) { }
}

static void delay(size_t ticks) {
    for (volatile size_t delay = 0; delay < ticks; delay++) {
    }
}

static void uart_reader(void* param) {
    (void)param;
    for (;;) {
        if (LPUART1->STAT & LPUART_STAT_RDRF_MASK) {
            // TODO: start an SPI transfer
            char c = LPUART1_receive_char();
            if (c == 'B') {
                sw_reset();
            }
        }
    }
}

static inline void start_uart_reader(void) {
    static StackType_t stack[configMINIMAL_STACK_SIZE * 2];
    static StaticTask_t task;
    (void)xTaskCreateStatic(uart_reader, "uart_reader", sizeof(stack) / sizeof(*stack), NULL, 4, stack, &task);
}

static inline void remote_leds(uint32_t leds) {
    LedControl led_control = LED_CONTROL__INIT;
    led_control.leds_case = LED_CONTROL__LEDS_SET_LEDS;
    led_control.set_leds = leds;
    Message message = MESSAGE__INIT;
    message.message_case = MESSAGE__MESSAGE_LED_CONTROL;
    message.led_control = &led_control;
    spi_transmit_message(&message);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    leds_off(0xFFF);
    leds_on(FRED_LEFT_R | FRED_RIGHT_R);
    remote_leds(DINO_LEFT_R_P | DINO_RIGHT_R_P);
    for (;;) { }
}

struct registers {
    uint16_t registers[16];
    uint16_t instruction;
    uint16_t timer;
};

static inline void set_leds(uint16_t leds) {
    leds_off(0xFFF);
    leds_on(((leds & 0x800) >> 3) | ((leds & 0x600) >> 9) | ((leds & 0x1C0) << 3));
    remote_leds(leds & 0x3F);
}

__attribute__((noreturn)) static inline void invalid_op(void) {
    leds_off(0xFFF);
    leds_on(FRED_LEFT_R | FRED_RIGHT_R);
    remote_leds(DINO_LEFT_R_P | DINO_RIGHT_R_P);
    vTaskDelay(portMAX_DELAY);
    for (;;) { }
}

static void led_control(void* param) {
    (void)param;
    struct registers data;
    for (int i = 0; i < 16; i++) {
        data.registers[i] = i;
    }
    data.instruction = 0x200;
    data.timer = 0;
    for (;;) {
        uint16_t ip = data.instruction - 0x200;
        if (ip >= sizeof(script)) {
            invalid_op();
        }
        data.instruction += 2;
        uint16_t op = script[ip >> 1];
        switch (op >> 12) {
        case 0:
            switch (op) {
            case 0:
                vTaskDelay(portTICK_PERIOD_MS * 17 * data.timer);
                break;
            default:
                invalid_op();
            }
            break;
        case 1:
            data.instruction = op & 0xFFF;
            break;
        case 3: {
            uint8_t lhs = (op >> 8) & 0xF;
            uint16_t rhs = op & 0xFF;
            if (data.registers[lhs] == rhs) {
                data.instruction += 2;
            }
            break;
        }
        case 4: {
            uint8_t lhs = (op >> 8) & 0xF;
            uint16_t rhs = op & 0xFF;
            if (data.registers[lhs] != rhs) {
                data.instruction += 2;
            }
            break;
        }
        case 5: {
            uint8_t lhs = (op >> 8) & 0xF;
            uint8_t rhs = (op >> 4) & 0xF;
            if (data.registers[lhs] == data.registers[rhs]) {
                data.instruction += 2;
            }
            break;
        }
        case 6: {
            uint8_t lhs = (op >> 8) & 0xF;
            uint16_t rhs = op & 0xFF;
            data.registers[lhs] = rhs;
            break;
        }
        case 7: {
            uint8_t lhs = (op >> 8) & 0xF;
            uint16_t rhs = op & 0xFF;
            data.registers[lhs] += rhs;
            break;
        }
        case 8: {
            uint8_t lhs = (op >> 8) & 0xF;
            uint8_t rhs = (op >> 4) & 0xF;
            uint8_t sub_op = op & 0xF;
            switch (sub_op) {
            case 0:
                data.registers[lhs] = data.registers[rhs];
                break;
            case 1:
                data.registers[lhs] |= data.registers[rhs];
                break;
            case 2:
                data.registers[lhs] &= data.registers[rhs];
                break;
            case 3:
                data.registers[lhs] = ((data.registers[lhs] << rhs) | (data.registers[lhs] >> (12 - rhs))) & 0xFFF;
                break;
            case 4:
                data.registers[lhs] += data.registers[rhs];
                data.registers[15] = (data.registers[lhs] >> 12) ? 1 : 0;
                data.registers[lhs] &= 0xFFF;
                break;
            case 5:
                data.registers[lhs] -= data.registers[rhs];
                data.registers[15] = (data.registers[lhs] >> 12) ? 1 : 0;
                data.registers[lhs] &= 0xFFF;
                break;
            default:
                invalid_op();
            }
            break;
        }
        case 9: {
            uint8_t lhs = (op >> 8) & 0xF;
            uint8_t rhs = (op >> 4) & 0xF;
            if (data.registers[lhs] != data.registers[rhs]) {
                data.instruction += 2;
            }
            break;
        }
        case 10:
            set_leds(op & 0xFFF);
            break;
        case 11:
            data.instruction = (op & 0xFFF) + data.registers[0];
            break;
        case 12:
            break;  // TODO
        case 13: {
            uint8_t lhs = (op >> 8) & 0xF;
            uint8_t rhs = (op >> 4) & 0xF;
            set_leds(data.registers[lhs] | data.registers[rhs]);
            break;
        }
        case 15: {
            uint8_t lhs = (op >> 8) & 0xF;
            uint8_t sub_op = op & 0xFF;
            switch (sub_op) {
            case 7:
                break;  // TODO
            case 21:
                data.timer = data.registers[lhs];
                break;
            default:
                invalid_op();
            }
            break;
        }
        default:
            invalid_op();
        }
    }
}

static inline void start_led_control(void) {
    static StackType_t stack[configMINIMAL_STACK_SIZE * 2];
    static StaticTask_t task;
    (void)xTaskCreateStatic(led_control, "led_control", sizeof(stack) / sizeof(*stack), NULL, 4, stack, &task);
}

int main(void) {
    SlowRUNmode_48MHz();
    PORT_init();

    LPUART1_init();
    LPSPI0_init_master();

    // Clear any partial messages before reset
    LPSPI0_transmit_8bits(0x7E);
    (void)LPSPI0_receive_8bits();

    start_uart_reader();
    start_led_control();
    vTaskStartScheduler();
    for (;;) { }
}
