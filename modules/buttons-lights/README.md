# Buttons controller firmware

Button and light controller (buttons with built-in lights), compatible with decoder boards.

- 128 independent buttons and lights
- lights flashing
- encoder: left, right, push button
- I2C interrupt based

## Measurements

- full buttons scan + lights refresh: ~200us
- buttons antibounce: ~7ms

## I2C protocol

### Receive buttons events

Master: `<ADDRESS> ...` continues reading data if necessary
Slave (this device): `<DATA AVAIL LENGTH> <BUTTON ON> ...`

Buttons mapping:

    0..127 = Normal button
    128    = Encoder left
    129    = Encoder right
    130    = Encoder button

If Master tries to read more than what is available in the slave's buffer, the slave responds with `0xFF` values.

### Send commands

Master: `<ADDRESS> <COMMAND> ...` continues writing data if necessary

Commands:

    0b11111111 (0xFF)       Reset
    0b1IIIIIII (0x80..0xFE) Set light index (0..126)
    0b000000SS (0x00..0x03) Light state:
                            0 = OFF  
                            1 = ON  
                            2 = FLASH HIGH  
                            3 = FLASH LOW

#### Reset command

Master: `<ADDRESS> 0xFF` (and wait at least 10ms)
Slave: `<STR_LEN> <FIRMWARE_VERSION> 0x00`

After the reset command the Master waits at least 10ms and receive the firmware version.
