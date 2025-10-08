# Buttons controller firmware

Button and light controller (buttons with built-in lights), compatible with decoder boards.

- 128 independent buttons
- led flashing
- I2C interrupt based

## Measurements

- full buttons scan + lights refresh: ~200us
- buttons antibounce: ~7ms

## I2C protocol

### Receive buttons events

Master: `<ADDRESS> ...` continues reading data if necessary
Slave (this device): `<DATA AVAIL LENGTH> <BUTTON ON/OFF> ...`

Button ON/OFF:
`0bXNNNNNNN`  `X` = Button Off/On   `NNNNNNN` = Button index (0..127)

If Master tries to read more than what is available in the slave's buffer, the slave responds with `0xFF` values.

### Set lights

Master: `<ADDRESS> <SET_LIGHT> <LIGHT_STATE>...` continues writing data if necessary

Set light: `0b1NNNNNNN`  `NNNNNNN` = Light index (0..127)

Light state:  
0 = OFF  
1 = ON  
2 = FLASH HIGH  
3 = FLASH LOW
