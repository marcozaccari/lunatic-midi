# Led strip controller firmware

Controller compatible with RGB digital strip led.

- 64 independent RGB leds
- 7 colors
- I2C interrupt based

## Measurements

- full buttons scan + lights refresh: ~200us

## I2C protocol

Master: `<ADDRESS> <SET_LIGHT> <LIGHT_STATE>...` continues writing data if necessary

Set light: `0b1NNNNNNN`  `NNNNNNN` = Light index (0..127)

Light state:  
0 = OFF  
1 = ON  
2 = FLASH HIGH  
3 = FLASH LOW
