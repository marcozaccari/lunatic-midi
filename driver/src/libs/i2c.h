#ifndef _I2C_H
#define _I2C_H

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct i2c_tag {
	int file;
	int address;
} i2c_t;

bool i2c_open(i2c_t *i2c, int address);
bool i2c_close(i2c_t *i2c);

bool i2c_read(i2c_t *i2c, uint8_t *buffer, int num_bytes);
bool i2c_write(i2c_t *i2c, uint8_t *buffer, int num_bytes);

#endif