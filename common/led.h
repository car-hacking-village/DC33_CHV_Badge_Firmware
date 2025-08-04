#ifndef COMMON_LED_H_
#define COMMON_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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

static inline uint32_t dino_led_encode(const struct dino_led* leds) {
    return ((leds->left_red & 1) << 5)
        | ((leds->left_green & 1) << 4)
        | ((leds->left_blue & 1) << 3)
        | ((leds->right_red & 1) << 2)
        | ((leds->right_green & 1) << 1)
        | ((leds->right_blue & 1) << 0);
}

static inline struct dino_led dino_led_decode(uint32_t leds) {
    return (struct dino_led) {
        .left_red = (leds & 32) >> 5,
        .left_green = (leds & 16) >> 4,
        .left_blue = (leds & 8) >> 3,
        .right_red = (leds & 4) >> 2,
        .right_green = (leds & 2) >> 1,
        .right_blue = leds & 1,
    };
}

#ifdef __cplusplus
}
#endif

#endif
