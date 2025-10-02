#ifndef DEVICE_H
#define	DEVICE_H

#include <xc.h>

// Enable when PicKit debugger is connected to the board
#define PROGRAMMER_CONNECTED

void device_init(void);

inline void led_on();
inline void led_off();
inline void led_toggle();

// Hardware bridge flag
#define HARDWARE_FLAG_PIN  PORTAbits.RA5;

// Device configuration

// Reference https://developerhelp.microchip.com/xwiki/bin/view/products/mcu-mpu/8bit-pic/enhanced-family/emconfig/

// CONFIG1
#pragma config FOSC = HS       // External High Speed Crystal Oscillator
#pragma config WDTE = ON       // Watchdog timer enable
#pragma config PWRTE = ON      // Power-up Timer enable: nominal 72 ms delay after a Power-on Reset or a Brown-out Reset to allow Vdd to stabilize.
#pragma config MCLRE = ON      // MCRL/Vpp as the RESET pin
#pragma config CP = OFF        // Program Memory READ Protection disabled
#pragma config CPD = OFF       // Data EEPROM Read Protection disabled
#pragma config FCMEN = OFF     // Fail Safe Clock Monitor disabled: guarantees that the clock is always and only external
#pragma config IESO = ON       // Internal/External Switch-over (?)
#pragma config BOREN = ON      // Brown-out Reset Enabled: allows an MCU RESET to occur if Vdd drops below a preset value
#pragma config LVP = OFF       // Low Voltage programming disabled
#pragma config DEBUG = OFF     // Set by MPLAB IDE (?)

// CONFIG2
#pragma config BOR4V = BOR40V  // Brown-out Reset set to 4.0V
#pragma config WRT = OFF       // Flash Self Write enabled (unused)

#define _XTAL_FREQ 20000000  // 20 MHz

#endif
