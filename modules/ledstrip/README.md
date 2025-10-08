# Led strip controller firmware

Controller compatible with WS2812 RGB digital strip led.

- 60 independent RGB leds (59 useable)
- 7 colors
- I2C interrupt based

## Measurements

- Full leds update: ~3ms

Note: `cmake/ledstrip-c/default/user.cmake` has been added to change the "-O1" compile option.
The LED update functions that communicate with the WS2812 have very tight timing, and redundant jumps should be avoided as much as possible.

## I2C protocol

Master: `<ADDRESS> <COMMAND>...` continues writing data if necessary

Commands:

    0b11111111 (0xFF)       Reset (all leds off, reset led index = 0)
                            NB: please wait at least 3ms after reset request

    0b01IIIIII (0x40..0x7F) Set led index to I (00..59)
    0b00000RGB (0x00..0x07) Set (current index) led color 

    0b11001RGB (0xC8..0xCF) Fill all leds with CC color

    0b11000001 (0xC1)       Repaint request (confirm updates and reset led index = 0)
                            NB: please wait at least 3ms after repaint request

    0b1000RRRR (0x80..0x8F) Tune R (color will be set to RRRR0000)
    0b1001GGGG (0x90..0x9F) Tune G (color will be set to GGGG0000)
    0b1010BBBB (0xA0..0xAF) Tune B (color will be set to BBBB0000)
