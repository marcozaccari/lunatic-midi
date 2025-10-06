#include "keyboard.h"
#include "timers.h"
#include "device.h"
#include "i2c.h"
#include <stdint.h>

uint8_t keys_switch1_last[8];
uint8_t keys_switch2_last[8];

uint8_t keys_state[8];  // General state of the keys (fully released/fully pressed)

uint8_t key_timers_h[64];
uint8_t key_timers_l[64];

uint16_t velocity_max_ms = 5000;

/*******************************************************************************
 * A1..A4 = bank selector (0..7, A4 always 0)
 * A0     = key switch selector for velocity measure (0 = key press start, 1 = key press end)
 * B0..B7 = bank key state (0 = switch start, 1 = switch end)
 */
void keyboard_init(void) {
    // PORT A - decoder address (keys bank selection)
    TRISA0 = 0; // A0 as output
    TRISA1 = 0; // A1 as output
    TRISA2 = 0; // A2 as output
    TRISA3 = 0; // A3 as output
    TRISA4 = 0; // A4 as output (always 0)

    PORTA = 0; // clear output data latches on port

    // PORT B - decoder status lines (keys bank state)
    TRISB = 0xFF; // PortB as input
    OPTION_REGbits.nRBPU = 0; // Enable single pullups on PORT B
    WPUB = 0xFF; // Enable all pull-up for PORT B

    // Init internal variables
    for (uint8_t i = 0; i < 8; i++) {
        keys_switch1_last[i] = 0;
        keys_switch2_last[i] = 0;
        keys_state[i] = 0;
    }
    for (uint8_t i = 0; i < 64; i++) {
        key_timers_h[i] = 0;
        key_timers_l[i] = 0;
    }
}

// Scans all keys relative to the first switch (starting of key press).
inline void keyboard_scan_switch1(void) {
    uint8_t cur_bank; // 0..7
    uint8_t cur_key = 0;  // 0..64

    for (cur_bank = 0; cur_bank < 8; cur_bank++) {
        uint8_t keys_switches, keys_switches_last;
        uint8_t changes, changes_on;
        uint8_t changes_off;

        PORTA = (uint8_t)(cur_bank << 1);  // select bank

        // wait 2us in order to discarge the pull-ups
        //__delay_us(2);  
        asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");

        keys_switches = ~PORTB;  // get keys status by PORTB (inverted)
        #ifdef PROGRAMMER_CONNECTED
        keys_switches &= 0b00111111; // remove upper 2 bits (used by programmer)
        #endif

        keys_switches_last = keys_switch1_last[cur_bank];

        // current state XOR last state (all variations: 0 -> 1, 1 -> 0)
        changes = keys_switches ^ keys_switches_last;

        if (!changes) {
            cur_key += 8;
            continue;
        }

        changes_on = changes & keys_switches; // 0 -> 1 variations
        changes_off = changes & keys_switches_last; // 1 -> 0 variations

        // Save current state as last
        keys_switch1_last[cur_bank] = keys_switches;

        // Parse changes

        if (changes_on) {
            for (uint8_t i = 0; i < 8; i++) {
                if (changes_on & (1 << i)) {
                    // Key pressing

                    //led_toggle();

                    // Save the timestamp for this key (antibounce: reset timestamps)
                    uint16_t timestamp = Timer_count;
                    key_timers_h[cur_key + i] = timestamp >> 8;
                    key_timers_l[cur_key + i] = timestamp & 0xFF;
                }
            }
        }
        
        if (changes_off) {
            uint8_t keys_state_bank = keys_state[cur_bank];

            for (uint8_t i = 0; i < 8; i++) {
                if (changes_off & (1 << i)) {
                    // Total key release

                    if (keys_state_bank & (1 << i)) {  // (antibounce) was the key actually pressed?
                        // Send key OFF event
                        I2C_tx((cur_key + i) | 0b10000000);  // key index = 10XXXXXX

                        keys_state_bank &= ~(1 << i); // reset key bit
                    }
                }
            }
 
            // Update keys state
            keys_state[cur_bank] = keys_state_bank;
        }

        cur_key += 8;
    }
}

// Scan all keys relative to the second switch (keys pressed fully down)
inline void keyboard_scan_switch2(void) {
    uint8_t cur_bank; // 0..7
    uint8_t cur_key = 0;  // 0..64

    for (cur_bank = 0; cur_bank < 8; cur_bank++) {
        uint8_t keys_switches, keys_switches_last;
        uint8_t changes, changes_on;
        uint8_t changes_off;

        PORTA = (uint8_t)(cur_bank << 1) + 1;  // select bank

        // wait 2us in order to discarge the pull-ups
        //__delay_us(2);  
        asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");

        keys_switches = PORTB;  // get keys status by PORTB (inverted)
        keys_switches = ~keys_switches;
        #ifdef PROGRAMMER_CONNECTED
        keys_switches &= 0b00111111; // remove upper 2 bits (used by programmer)
        #endif

        keys_switches_last = keys_switch2_last[cur_bank];

        // current state XOR last state (all variations: 0 -> 1, 1 -> 0)
        changes = keys_switches ^ keys_switches_last;
        changes_on = changes & keys_switches; // 0 -> 1 variations

        if (!changes) {
            cur_key += 8;
            continue;
        }

        keys_switch2_last[cur_bank] = keys_switches;  // save current state as last

        if (!changes_on) {
            cur_key += 8;
            continue;
        }

        uint8_t keys_state_bank = keys_state[cur_bank];

        for (uint8_t i = 0; i < 8; i++) {
            if (changes_on & (1 << i)) {
                if (!(keys_state_bank & (1 << i))) {  // (antibounce) was the key actually released?
                    //led_toggle();

                    // calculate time elapsed since key press start
                    uint16_t timestamp = (uint16_t)(key_timers_h[cur_key] << 8) + key_timers_l[cur_key];

                    uint16_t elapsed = get_elapsed(timestamp);
                    uint8_t velocity;

                    if (elapsed >= 255)  
                        velocity = 127;  // max 256ms
                    else
                        velocity = (uint8_t)(elapsed >> 1);
                    
                    // send key ON event
                    I2C_tx(cur_key | 0b11000000);  // key index = 11XXXXXX
                    I2C_tx(velocity);

                    keys_state_bank |= (1 << i); // set key bit
                }
            }

            cur_key++;
        }

        // Update keys state
        keys_state[cur_bank] = keys_state_bank;
    }
}

inline void keyboard_scan(void) {
    keyboard_scan_switch1();
    keyboard_scan_switch2();
}

