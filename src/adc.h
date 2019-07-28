#ifndef _ADC_H
#define _ADC_H

#include "../lib/utils.h"

#define ADC_ADDRESS 

unsigned int adc_ribbon_value;
unsigned int adc_pedal_value[3];

bool adc_init(int i2c_address);
bool adc_done();

bool adc_do();

#endif
