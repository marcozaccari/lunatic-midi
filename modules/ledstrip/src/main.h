#ifndef MAIN_H
#define	MAIN_H

#define VERSION "LedStrip v1.0"

#define I2C_TX_BUFFER_SIZE 16
#define I2C_RX_BUFFER_SIZE 64

// Enable when PicKit debugger is connected to the board
//#define PROGRAMMER_CONNECTED

// Flashes the LED at each timer2 overflow
//#define TEST_TIMER

// Enable for I2C testing (see /controller/test/i2c/test_i2c.go)
//#define TEST_I2C

#endif
