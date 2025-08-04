#ifndef COMMON_LED_H_
#define COMMON_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

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

enum {
    DINO_LEFT_R_P = 1 << 5,
    DINO_LEFT_G_P = 1 << 4,
    DINO_LEFT_B_P = 1 << 3,
    DINO_RIGHT_R_P = 1 << 2,
    DINO_RIGHT_G_P = 1 << 1,
    DINO_RIGHT_B_P = 1 << 0,
};

#ifdef __cplusplus
}
#endif

#endif
