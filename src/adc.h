#ifndef _ADC_H
#define _ADC_H

#include "utils.h"

#define ADC_ADDRESS 

unsigned int adc_ribbon_value;
unsigned int adc_pedal_value[3];

bool adc_open();
bool adc_close();

bool adc_read();

#endif
