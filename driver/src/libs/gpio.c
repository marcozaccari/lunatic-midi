//  https://elinux.org/RPi_GPIO_Code_Samples#Direct_register_access
//  https://github.com/recalbox/mk_arcade_joystick_rpi/blob/master/mk_arcade_joystick_rpi.c#L203

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "gpio.h"
#include "log.h"


#ifdef RPI2_OR_3
    #define BCM2708_PERI_BASE        0x3F000000
#else
    #define BCM2708_PERI_BASE        0x20000000
#endif

#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#if defined(__arm__)
static int  mem_fd;
static void *gpio_map;

// I/O access
static volatile unsigned *gpio;


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock
#endif


/** Set up a memory regions to access GPIO
 * 
 */
bool gpio_init() {
   #if defined(__arm__)
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      dlog(_LOG_ERROR, "GPIO cannot open /dev/mem");
      return false;
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      dlog(_LOG_ERROR, "GPIO mmap error %d: %s", errno, strerror(errno));
      return false;
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;
   #endif

   return true;
}

void gpio_set_pin_to_output(int pin) {
   #if defined(__arm__)
   INP_GPIO(pin); // must use INP_GPIO before we can use OUT_GPIO
   OUT_GPIO(pin);
   
   #else
   UNUSED(pin);
   #endif
}

void gpio_set_pin_to_input(int pin) {
   #if defined(__arm__)
   INP_GPIO(pin);
   
   #else
   UNUSED(pin);
   #endif
}

void gpio_output(int pin, bool state) {
   #if defined(__arm__)
   if (state)
      GPIO_SET = 1 << pin;
   else
      GPIO_CLR = 1 << pin;
   
   #else
   UNUSED(pin);
   UNUSED(state);
   #endif
}

bool gpio_input(int pin) {
   #if defined(__arm__)
   return (bool)(GET_GPIO(pin));
   
   #else
   UNUSED(pin);
   return false;
   #endif
}


/*
  // enable pull-up on GPIO24&25
   GPIO_PULL = 2;
   //short_wait();
   udelay(10);
   // clock on GPIO 24 & 25 (bit 24 & 25 set)
   GPIO_PULLCLK0 = 0x03000000;
   udelay(10);
   //short_wait();
   GPIO_PULL = 0;
   GPIO_PULLCLK0 = 0;
*/

/**
http://www.hamzakilic.com/2015/04/12/raspberrypi-gpio-examples-with-c-part4-pull-updown/
1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither
to remove the current Pull-up/down)
2. Wait 150 cycles this provides the required set-up time for the control signal
3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to
modify NOTE only the pads which receive a clock will be modified, all others will
retain their previous state.
4. Wait 150 cycles this provides the required hold time for the control signal
5. Write to GPPUD to remove the control signal
6. Write to GPPUDCLK0/1 to remove the clock

*/