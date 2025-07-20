#ifndef COMMON_LED_H_
#define COMMON_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** Pins for RP2040 LEDs */
enum {
    DINO_LEFT_R = 20,
    DINO_LEFT_G = 19,
    DINO_LEFT_B = 18,

    DINO_RIGHT_R = 28,
    DINO_RIGHT_G = 27,
    DINO_RIGHT_B = 26,
};

/*
 * LED bits are encoded into a byte as follows:
 *
 * |--------+--------+------+-------+------+-------+-------+-------|
 * | 7      | 6      | 5    | 4     | 3    | 2     | 1     | 0     |
 * |--------+--------+------+-------+------+-------+-------+-------|
 * | Unused | Unused | Dino | Dino  | Dino | Dino  | Dino  | Dino  |
 * | (0)    | (0)    | Left | Left  | Left | Right | Right | Right |
 * |        |        | Red  | Green | Blue | Red   | Green | Blue  |
 * |--------+--------+------+-------+------+-------+-------+-------|
 */

struct dino_led {
    int left_red : 1;
    int left_green : 1;
    int left_blue : 1;
    int right_red : 1;
    int right_green : 1;
    int right_blue : 1;
};

uint32_t dino_led_encode(struct dino_led const* leds);
struct dino_led dino_led_decode(uint32_t leds);

#ifdef __cplusplus
}
#endif

#endif
