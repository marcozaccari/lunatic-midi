//#include <pic16f886.h>
#include "device.h"
#include "i2c.h"

// Variabile globale per il dato ricevuto
volatile unsigned char i2c_data = 0;

// ===== INTERRUPT =====
void __interrupt() ISR(void) {
    if (PIR1bits.SSPIF) {   // Se interrupt da I2C
        I2C_isr();
        PIR1bits.SSPIF = 0;  // clear interrupt flag
    }
}

int main(void) {
    device_init();
    
    I2C_init();

    for (;;) {
        led_on();
        for (int k = 0; k < 10000; k++);
        led_off();
        for (int k = 0; k < 10000; k++);
    }
    
    /*for (;;) {
        led_on();
        __delay_ms(1000); 
        led_off();
        __delay_ms(1000); 

        led_on();
        __delay_ms(1000); 
        led_off();
        __delay_ms(1000); 

        led_on();
        __delay_ms(1000); 
        led_off();
        __delay_ms(1000); 

        led_on();
        __delay_ms(1000); 
        led_off();
        __delay_ms(1000); 

        led_on();
    }*/

    return 0;
};
