extern "C" {
#include <clock_manager.h>
#include <pins_driver.h>
}

struct Pin {
    PORT_Type* port;
    GPIO_Type* gpio;
    uint32_t pin;
};

const Pin fred_left[3] = {
    { PORTB, PTB, 8 },  // red
    { PORTA, PTA, 1 },  // green
    { PORTA, PTA, 0 },  // blue
};

const Pin fred_right[3] = {
    { PORTB, PTB, 11 },  // red
    { PORTB, PTB, 10 },  // green
    { PORTB, PTB, 9 },  // blue
};

void delay(volatile int ticks) {
    while (ticks--)
        ;
}

int main() {
    CLOCK_DRV_SetModuleClock(PORTA_CLK, nullptr);
    CLOCK_DRV_SetModuleClock(PORTB_CLK, nullptr);
    for (const auto& pin : fred_left) {
        PINS_DRV_SetMuxModeSel(pin.port, pin.pin, PORT_MUX_AS_GPIO);
    }
    for (const auto& pin : fred_right) {
        PINS_DRV_SetMuxModeSel(pin.port, pin.pin, PORT_MUX_AS_GPIO);
    }
    PINS_DRV_SetPinsDirection(PTA, (1 << 1) | (1 << 0));
    PINS_DRV_SetPinsDirection(PTB, (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8));
    while (true) {
        for (const auto& rpin : fred_right) {
            PINS_DRV_SetPins(rpin.gpio, 1 << rpin.pin);
            for (const auto& lpin : fred_left) {
                PINS_DRV_SetPins(lpin.gpio, 1 << lpin.pin);
                delay(1000000);
                PINS_DRV_ClearPins(lpin.gpio, 1 << lpin.pin);
            }
            PINS_DRV_ClearPins(rpin.gpio, 1 << rpin.pin);
        }
    }
}
