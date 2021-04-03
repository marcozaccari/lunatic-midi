#ifndef _BUTTONS_H
#define _BUTTONS_H

#include "../lib/utils.h"

bool buttons_init(int i2c_address);
bool buttons_done();

void buttons_debug();
bool buttons_do();

#endif
