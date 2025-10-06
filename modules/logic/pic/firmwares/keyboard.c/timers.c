#include "timers.h"
#include "device.h"

volatile uint16_t Timer_count;

void timers_init(void) {
    // Init internal variables
    Timer_count = 0;

    PIE1bits.TMR2IE = 1; // interrupt on Timer2 overflow

    // 1 / ( Fosc / 4 / prescaler ) * postscaler * PR2
    T2CON &= 0b10000100;  // mask scaler bits
    T2CON |= 0b00000011;  // prescaler = 16
    T2CON |= 0b00001000; // postscaler = 2 (1ms)
    PR2 = 155; // 1ms  <-- BEST

    T2CONbits.TMR2ON = 1;  // enable timer (counter in TMR2)
}

inline void timers_isr(void) {
    Timer_count++;
    //led_toggle();
}

inline uint16_t get_elapsed(uint16_t since) {
    uint16_t now = Timer_count;

    if (since <= now)
        return now - since;
    else
        return (0xFFFF - since) + now;
}
