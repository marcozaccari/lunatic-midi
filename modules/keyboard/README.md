# Keyboard controller firmware

Controller for polyphonic keyboards equipped with double switches for velocity measurement.

- 63 independent keys
- 128-level velocity
- I2C interrupt based

## Measurements

- full keyboard scan: 180us
- key velocity: 1ms..256ms (0..127)

## I2C protocol

Master: `<ADDRESS> ...` continues reading data if necessary
Slave (this device): `<DATA AVAIL LENGTH> <NOTE ON/OFF> <VELOCITY> <NOTE ON/OFF> ...`

Note ON/OFF:
`0b1XNNNNNN`  `X` = Key Off/Key On   `NNNNNN` = Key (0..62)

Velocity:
`0b0NNNNNNN`  `NNNNNNN` = Velocity (0 fastest..127 slowest)

If Master tries to read more than what is available in the slave's buffer, the slave responds with `0xFF` values.
