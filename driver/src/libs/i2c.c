#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h> // I2C bus definitions

#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#include "i2c.h"


bool i2c_open(i2c_t *i2c, int address) {
   #if defined(__arm__)
	if (i2c->file)
		return false;

	i2c->address = address;

	i2c->file = open("/dev/i2c-1", O_RDWR);
	if (!i2c->file)
		return false;

	ioctl(i2c->file, I2C_SLAVE, i2c->address);
   
   #else
   UNUSED(i2c);
   UNUSED(address);
   #endif
   
	return true;
}

bool i2c_close(i2c_t *i2c) {
   #if defined(__arm__)
	if (!i2c->file)
		return true;

	close(i2c->file);
	i2c->file = 0;

   #else
   UNUSED(i2c);
   #endif

	return true;
}

bool i2c_read(i2c_t *i2c, uint8_t *buffer, int num_bytes) {
   #if defined(__arm__)
	int ret;

	if (!i2c->file)
		return false;

	ret = read(i2c->file, buffer, num_bytes);

	return (ret == num_bytes);

   #else
   UNUSED(i2c);
   UNUSED(buffer);
   UNUSED(num_bytes);

   return true;
   #endif
}

bool i2c_write(i2c_t *i2c, uint8_t *buffer, int num_bytes) {
   #if defined(__arm__)
	int ret;

	if (!i2c->file)
		return false;

	ret = write(i2c->file, buffer, num_bytes);

	return (ret == num_bytes);

   #else
   UNUSED(i2c);
   UNUSED(buffer);
   UNUSED(num_bytes);

   return true;
   #endif
}

