#ifndef _GPIO_H
#define _GPIO_H

#include "utils.h"

// Zero = 1
//#define RPI2_OR_3

bool gpio_init();

void gpio_set_pin_to_output(int pin);
void gpio_set_pin_to_input(int pin);
void gpio_output(int pin, bool state);
bool gpio_input(int pin);


#endif