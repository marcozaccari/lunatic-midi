#include "buttons.h"
#include "main.h"
#include "device.h"
#include "i2c.h"
#include "timers.h"
#include <stdint.h>

uint8_t buttons_current_state[16];
uint8_t buttons_last_state[16];

#define ANTIBOUNCE_MS 7
uint16_t antibounce_counter;

/*******************************************************************************
 * A0..A3 = bank selector (0..15)
 * A4     = buttons / leds selector 
 * A5     = leds latch (off/on - default)
 * B0..B7 = bank buttons state (0 = button released, 1 = button pressed)
*/
void buttons_init(void) {
    // PORT A - decoder address (bank selection)
    TRISA0 = 0; // A0 as output
    TRISA1 = 0; // A1 as output
    TRISA2 = 0; // A2 as output
    TRISA3 = 0; // A3 as output
    TRISA4 = 0; // A4 as output
    TRISA5 = 0; // A5 as output

    PORTA = 0b00100000; // LEDS latch high

    // PORT B - decoder status lines (bank buttons state)
    TRISB = 0xFF; // PortB as input
    OPTION_REGbits.nRBPU = 0; // Enable single pullups on PORT B
    WPUB = 0xFF; // Enable all pull-up for PORT B

    // Init internal variables
    for (uint8_t i = 0; i < 16; i++) {
        buttons_current_state[i] = 0;
        buttons_last_state[i] = 0;
    }

    antibounce_counter = 0;
}

uint16_t buttons_worker_timestamp_ms;

// Scans all buttons, updating buttons_current_state.
inline void buttons_scan(void) {
    TRISB = 0xFF;  // PortB as input

    for (uint8_t cur_bank = 0; cur_bank < 16; cur_bank++) {
        // select buttons bank
        uint8_t addr = cur_bank;
        addr |= 0b00100000; // leds latch high
        PORTA = addr;
        
        __delay_us(4);  // wait 4us in order to discarge the pull-ups

        uint8_t current_state = ~PORTB;  // get buttons status by PORTB (inverted)
        #ifdef PROGRAMMER_CONNECTED
        current_state &= 0b00111111; // remove upper 2 bits (used by programmer)
        #endif

        buttons_current_state[cur_bank] |= current_state;
    }
}

inline void buttons_worker(void) {
    uint16_t timestamp = Timer_count;

    if (timestamp == buttons_worker_timestamp_ms)
        return;
    buttons_worker_timestamp_ms = timestamp;

    buttons_scan();

    if (antibounce_counter < ANTIBOUNCE_MS) {
        antibounce_counter++;
        return;
    }
    antibounce_counter = 0;

    // Check for buttons changes

    uint8_t cur_button = 0;  // 0..127
    for (uint8_t cur_bank = 0; cur_bank < 16; cur_bank++) {
        uint8_t current_state = buttons_current_state[cur_bank];
        buttons_current_state[cur_bank] = 0;  // Reset current state for next scans

        uint8_t last_state = buttons_last_state[cur_bank];

        // current state XOR last state (all variations: 0 -> 1, 1 -> 0)
        uint8_t changes = current_state ^ last_state;

        if (!changes) {
            cur_button += 8;
            continue;
        }

        //led_toggle();

        uint8_t changes_on = changes & current_state; // 0 -> 1 variations
        uint8_t changes_off = changes & last_state; // 1 -> 0 variations

        buttons_last_state[cur_bank] = current_state;  // Save current state as last

        // Parse changes

        for (uint8_t i = 0; i < 8; i++) {
            if (changes_on & (1 << i)) {
                // Button pressed

                // send button ON event
                I2C_tx(cur_button | 0b10000000);  // button index = 1XXXXXXX
                led_toggle();
            }

            if (changes_off & (1 << i)) {
                // Button released

                // send button OFF event
                I2C_tx(cur_button);  // button index = 0XXXXXXX
                led_toggle();
            }

            cur_button++;
        }
    }
}
