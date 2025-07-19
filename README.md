## Building

To build the full firmware package, run `nix-build`.

To build only firmware for RP2040 or S32K148, change directory into one
of the `rp2040` or `s32k148` directories and run `nix-build` in there.

## Dev Board Setup

Use a Raspberry Pi Pico (1) board and an NXP S32K148-Q176 board. Ensure
that your NXP board is running on 3V3 Vcc (jumper 7 should be flipped
from its default position). Jumpers 8 and 18 (the two surrounding ones)
should also be flipped from their default positions if you are going to
be powering the board from USB. (If you are using a 12V barrel power
supply, you do not need to flip jumpers 8 and 18, but sill need to flip
7.)

If you do not flip jumper 7, it's possible you could damage your
Raspberry Pi Pico.

### LEDs

Fred's LEDs are connected to the S32K148, and Dino's LEDs are connected
to the RP2040. All LEDs are held up with resistors and pulled down by
the microcontroller (inverted logic: 1=off, 0=on). Connect the anode to
3V3 and the cathode to the pin (but be sure to include a resistor on
either side).

- Fred
  - Left red: Q176 pin J5-4 (PTB8)
  - Left green: Q176 pin J4-18 (PTA1)
  - Left blue: Q176 pin J4-15 (PTA0)
  - Right red: Q176 pin J2-17 (PTB11)
  - Right green: Q176 pin J2-20 (PTB10)
  - Right blue: Q176 pin J6-10 (PTB9)
- Dino
  - Left red: Pico pin 26 (GPIO20)
  - Left green: Pico pin 25 (GPIO19)
  - Left blue: Pico pin 24 (GPIO18)
  - Right red: Pico pin 34 (GPIO28)
  - Right green: Pico pin 32 (GPIO27)
  - Right blue: Pico pin 31 (GPIO26)

### SPI

Wire the following SPI pins between the Q176 and Pico boards:

- CS
  - Q176 pin J2-7 (PTB0)
  - Pico pin 12 (GPIO9)
- S32 TX (RP RX)
  - Q176 pin J2-10 (PTB1)
  - Pico pin 16 (GPIO12)
- S32 RX (RP TX)
  - Q176 pin J2-13 (PTB3)
  - Pico pin 15 (GPIO11)
- SCK
  - Q176 pin J2-16 (PTB2)
  - Pico pin 14 (GPIO10)
- GND
  - Q176 pin J2-19 (GND)
  - Pico pin 13 (GND)
