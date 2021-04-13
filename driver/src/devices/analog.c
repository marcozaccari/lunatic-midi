// ADS1115 Driver

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> // open
#include <inttypes.h>  // uint8_t, etc
#include <sys/ioctl.h> 
#include <linux/i2c-dev.h> // I2C bus definitions

#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/select.h>

#include "../libs/utils.h"
#include "../libs/i2c.h"

#include "analog.h"


//#define ADS_SPS 8
//#define ADS_SPS 16
//#define ADS_SPS 32
//#define ADS_SPS 64
//#define ADS_SPS 128
//#define ADS_SPS 250
#define ADS_SPS 475
//#define ADS_SPS 860

//#define ADS_DIFFERENTIAL
//#define ADS_REAL_BITS 12

//#define ADS_DIFFERENTIAL
#define ADS_REAL_BITS 12

typedef enum {
	GAIN_6V   = 0,   // +-6.144V
	GAIN_4V   = 2,   // +-4.096V
	GAIN_2V   = 4,   // +-2.048V
	GAIN_1V   = 6,   // +-1.024V
	GAIN_05V  = 8,   // +-0.512V
	GAIN_025V = 0xA, // +-0.256V
} config_gain_t;

typedef enum {
	REF_GND   = 0,     // not differential
	REF_AIN3  = 0x40,  // differential
} config_reference_t;

typedef enum {
	CH_AIN0 = 0,
	CH_AIN1 = 0x10,
	CH_AIN2 = 0x20,
	CH_AIN3 = 0x30
} config_channel_t;

typedef enum {
	CONV_CONTINOUS = 0,
	CONV_ONESHOT  = 1,
} config_conversion_mode_t;

typedef enum {
	SPS_8   = 0,
	SPS_16  = 0x20,
	SPS_32  = 0x40,
	SPS_64  = 0x60,
	SPS_128 = 0x80,
	SPS_250 = 0xA0,
	SPS_475 = 0xC0,
	SPS_860 = 0xE0
} config_speed_t;

typedef enum {
	COMP_TRADITIONAL = 0,
	COMP_WINDOW      = 0x10
} config_comparator_type_t;

typedef enum {
	COMP_POL_LOW  = 0,  // Active low
	COMP_POL_HIGH = 8   // Active high
} config_comparator_polarity_t;

typedef enum {
	COMP_LAT_OFF  = 0,  // Non latching comparator
	COMP_LAT_ON   = 4   // Latching comparator
} config_comparator_latching_t;

typedef enum {
	COMP_QUEUE_1   = 0,  // Assert after one conversion
	COMP_QUEUE_2   = 1,  // Assert after two conversion
	COMP_QUEUE_4   = 2,  // Assert after four conversion
	COMP_QUEUE_OFF = 3   // Disable comparator (ALERT/RDY on high impedance)
} config_comparator_queue_t;

#define STATES_MAX 3
enum states {
	READ_CHAN0 = 0,
	READ_CHAN1 = 1,
	READ_CHAN2 = 2,
	READ_CHAN3 = 3
};


static void write_config(analog_t *self,
	config_channel_t channel, 
	config_reference_t reference,
	config_gain_t gain,
	config_speed_t speed,
	config_conversion_mode_t conversion_mode,
	config_comparator_type_t comparator,
	config_comparator_polarity_t comparator_polarity,
	config_comparator_latching_t comparator_latching,
	config_comparator_queue_t comparator_queue) {

	uint8_t buffer[3];
	
	buffer[0] = 1;  // write to config register
	buffer[1] = reference | channel | gain | speed | conversion_mode;
	buffer[2] = speed | comparator | comparator_polarity | 
					comparator_latching | comparator_queue;
	
	if (conversion_mode == CONV_ONESHOT)
		buffer[1] |= 0x80;  // start conversion now
	else
		self->continous_sampling = true;

 	i2c_write(&self->base->i2c, buffer, 3);
	
	if (self->continous_sampling) {
		buffer[0] = 0;
		i2c_write(&self->base->i2c, buffer, 1);  // prepare to read from conversion register
	}
}

static uint16_t read_sample(analog_t *self) {
	uint8_t buffer[2];
	uint16_t val;
	
	buffer[0] = 0;
	buffer[1] = 0;
	
	if (!self->continous_sampling)
		i2c_write(&self->base->i2c, buffer, 1);  // read from conversion register
	
	i2c_read(&self->base->i2c, buffer, 2);
	
	val = (buffer[0] << 8 | buffer[1]);
	
	return (val >> (15-ADS_REAL_BITS));
}

/*int adc_main() {
	 
  set_conio_terminal_mode();

  init_ads();
  
  //uint32_t val;
//uint32_t sample;
uint8_t k;
//uint32_t val_old;
unsigned int sample, val, val_old;
  float valf;

int filter_count = 40;
int overflow_value = (1 << ADS_REAL_BITS);

  //val = ads_one_shot();
  //valf = (float)val*4.096/32767.0;  

  ads_begin_sampling();


//#define COUNT 10000
//uint32_t k;
//uint8_t vals[COUNT];

//  for (k=0; k<COUNT; k++){

  while (!kbhit()) {
	 val = 0;

	 for (k=0; k<filter_count; k++){
	sample = ads_get_sample();
	usleep(780);  // get_sample + usleep = 1ms
//usleep(1600);
	//if (sample >= 1000) {
	//    val = 0;
	//    break;
	//}

	val += sample;
	 }

	 val = val / filter_count;
//val = val >> 1;
//val = ads_get_sample();


//    usleep(8000);
//    val += ads_get_sample();
//    usleep(8000);
//    val += ads_get_sample();
//    usleep(8000);
//    val += ads_get_sample();
//    usleep(8000);

//    val = val >> 2;


if ((val > val_old+1) || (val < val_old-1)) {
	 valf = (float)val*4.096/32767.0;  

	 printf("Voltage Reading "INT_TO_BINARY_PATTERN" %u %f (V) \r\n", INT_TO_BINARY(val), (uint16_t)val, valf);	// Print the result to terminal, first convert from binary value to mV

	val_old = val;
}
	 usleep(800);
//    usleep(10000);

//vals[k] = (uint8_t)(val & 0x0F);
//vals[k] = (uint16_t)val;
  }

//  (void)getch();

  close(I2CFile);
  
//printf("writing...\r\n");

//FILE *fp;
//fp=fopen("mh.raw", "w");
//fwrite(vals, 1, COUNT, fp);
//fclose(fp);

  return 0;

}*/

static bool init(analog_t *self) {
	if (!i2c_open(&self->base->i2c, self->base->i2c_address)) {
		//printf("Cannot open ADC\r\n");
		return false;
	}

	self->continous_sampling = false;
	self->state_machine = 0;

	for (int i=0; i<ANALOG_CHANNELS; i++) {
		self->last_values[i] = 0;
	}

	return true;
}

static bool done(analog_t *self) {
	return i2c_close(&self->base->i2c);
}

static bool work(analog_t *self) {
	analog_value_t analog_value;

	analog_value.channel = self->state_machine;

	switch (self->state_machine) {
		case READ_CHAN0:
			analog_value.value = 0;
			break;

		case READ_CHAN1:
			analog_value.value = 0;
			break;
			
		case READ_CHAN2:
			analog_value.value = 0;
			break;
			
		case READ_CHAN3:
			analog_value.value = 0;
			break;
	}
	
	self->state_machine++;
	self->state_machine %= STATES_MAX;

	if (self->last_values[analog_value.channel] != analog_value.value) {
		self->analog_buffer[self->analog_buffer_head] = analog_value;
		self->analog_buffer_head++;
	}

	return true;
}

analog_t* new_device_analog(char *name, int i2c_address) {
	analog_t* analog = malloc(sizeof(analog_t));

	analog->base = new_device(i2c_address, name, DEVICE_ANALOG, analog);

	analog->init = &init;
	analog->work = &work;
	analog->done = &done;

	return analog;
}
