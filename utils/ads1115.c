/* 
    ADS1115_sample.c - 12/9/2013. Written by David Purdie as part of the openlabtools initiative
    Initiates and reads a single sample from the ADS1115 (without error handling)
*/

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



//#define ADS_SPS 8
//#define ADS_SPS 16
//#define ADS_SPS 32
//#define ADS_SPS 64
//#define ADS_SPS 128
//#define ADS_SPS 250
#define ADS_SPS 475
//#define ADS_SPS 860

//#define ADS_DIFFERENTIAL
//#define REAL_BITS 12

//#define ADS_DIFFERENTIAL
#define REAL_BITS 12


struct termios orig_termios;

void reset_terminal_mode() {
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode() {
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch() {
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}





#define INT_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define INT_TO_BINARY(value)  \
  (value & 0x8000 ? '1' : '0'), \
  (value & 0x4000 ? '1' : '0'), \
  (value & 0x2000 ? '1' : '0'), \
  (value & 0x1000 ? '1' : '0'), \
  (value & 0x0800 ? '1' : '0'), \
  (value & 0x0400 ? '1' : '0'), \
  (value & 0x0200 ? '1' : '0'), \
  (value & 0x0100 ? '1' : '0'), \
  (value & 0x0080 ? '1' : '0'), \
  (value & 0x0040 ? '1' : '0'), \
  (value & 0x0020 ? '1' : '0'), \
  (value & 0x0010 ? '1' : '0'), \
  (value & 0x0008 ? '1' : '0'), \
  (value & 0x0004 ? '1' : '0'), \
  (value & 0x0002 ? '1' : '0'), \
  (value & 0x0001 ? '1' : '0') 




int I2CFile;

void init_ads() {
  int ADS_address = 0x48;  // Address of our device on the I2C bus

  I2CFile = open("/dev/i2c-1", O_RDWR);		// Open the I2C device
  
  ioctl(I2CFile, I2C_SLAVE, ADS_address);   // Specify the address of the I2C Slave to communicate with
}

int16_t ads_one_shot() {
  uint8_t writeBuf[3];		// Buffer to store the 3 bytes that we write to the I2C device
  uint8_t readBuf[2];		// 2 byte buffer to store the data read from the I2C device
  int16_t val;				// Stores the 16 bit value of our ADC conversion

  // These three bytes are written to the ADS1115 to set the config register and start a conversion 
  writeBuf[0] = 1;			// This sets the pointer register so that the following two bytes write to the config register
  //  writeBuf[1] = 0xC3;   	// This sets the 8 MSBs of the config register (bits 15-8) to 11000011
  writeBuf[1] = 0x93;   	// This sets the 8 MSBs of the config register (bits 15-8) to 10010011
  writeBuf[2] = 0x03;  		// This sets the 8 LSBs of the config register (bits 7-0) to 00000011

  // Initialize the buffer used to read data from the ADS1115 to 0
  readBuf[0]= 0;		
  readBuf[1]= 0;
      
  // Write writeBuf to the ADS1115, the 3 specifies the number of bytes we are writing,
  // this begins a single conversion
  write(I2CFile, writeBuf, 3);	

  // Wait for the conversion to complete, this requires bit 15 to change from 0->1
  while ((readBuf[0] & 0x80) == 0)	// readBuf[0] contains 8 MSBs of config register, AND with 10000000 to select bit 15
  {
      read(I2CFile, readBuf, 2);	// Read the config register into readBuf
  }

  writeBuf[0] = 0;					// Set pointer register to 0 to read from the conversion register

    write(I2CFile, writeBuf, 1);
  
    read(I2CFile, readBuf, 2);		// Read the contents of the conversion register into readBuf

    val = readBuf[0] << 8 | readBuf[1];	// Combine the two bytes of readBuf into a single 16 bit result 

    return val;
}

void ads_begin_sampling() {
  uint8_t writeBuf[3];		// Buffer to store the 3 bytes that we write to the I2C device

  // These three bytes are written to the ADS1115 to set the config register and start a conversion 
  writeBuf[0] = 1;			// This sets the pointer register so that the following two bytes write to the config register
    
 // #ifdef ADS_DIFFERENTIAL
  //writeBuf[1] = 0x12;  // 0 001 001 0 = AIN3 - AIN0, +-4,096V, continous conversion
  //#else
  writeBuf[1] = 0x72;  // 0 111 001 0 = AIN0, +-4,096V, continous conversion
  //#endif

 // #ifdef ADS_DIFFERENTIAL
 // writeBuf[1] = 0x14;  // 0 001 010 0 = AIN3 - AIN0, +-2,048V, continous conversion
 // #else
  writeBuf[1] = 0x74;  // 0 111 010 0 = AIN0, +-2,048V, continous conversion
 // #endif

  #if (ADS_SPS == 8)
  writeBuf[2] = 0x03;  // 010 000 11 = 8 SPS, no comparator
  #endif
  #if (ADS_SPS == 16)
  writeBuf[2] = 0x23;  // 010 000 11 = 16 SPS, no comparator
  #endif
  #if (ADS_SPS == 32)
  writeBuf[2] = 0x43;  // 010 000 11 = 32 SPS, no comparator
  #endif
  #if (ADS_SPS == 64)
  writeBuf[2] = 0x63;  // 011 000 11 = 64 SPS, no comparator
  #endif
  #if (ADS_SPS == 128)
  writeBuf[2] = 0x83;  // 100 000 11 = 128 SPS, no comparator
  #endif
  #if (ADS_SPS == 250)
  writeBuf[2] = 0xA3;  // 101 000 11 = 250 SPS, no comparator
  #endif
  #if (ADS_SPS == 475)
  writeBuf[2] = 0xC3;  // 110 000 11 = 250 SPS, no comparator
  #endif
  #if (ADS_SPS == 860)
  writeBuf[2] = 0xE3;  // 111 000 11 = 250 SPS, no comparator
  #endif

  // Write writeBuf to the ADS1115, the 3 specifies the number of bytes we are writing,
  // this begins a single conversion
  write(I2CFile, writeBuf, 3);

  writeBuf[0] = 0;					// Set pointer register to 0 to read from the conversion register
  write(I2CFile, writeBuf, 1);
}

int16_t ads_get_sample() {
  //uint8_t writeBuf[1];		// Buffer to store the 3 bytes that we write to the I2C device
  uint8_t readBuf[2];		// 2 byte buffer to store the data read from the I2C device
  uint16_t val;				// Stores the 16 bit value of our ADC conversion

  read(I2CFile, readBuf, 2);		// Read the contents of the conversion register into readBuf

  val = readBuf[0] << 8 | readBuf[1];

  return (val >> (15-REAL_BITS));
}



int main() {
    
  set_conio_terminal_mode();

  init_ads();
  
  //uint32_t val;
//uint32_t sample;
uint8_t k;
//uint32_t val_old;
unsigned int sample, val, val_old;
  float valf;

int filter_count = 40;
int overflow_value = (1 << REAL_BITS);

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
//val = val >> 1;*/
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

}
