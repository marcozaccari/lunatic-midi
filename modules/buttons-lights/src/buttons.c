#include "buttons.h"
#include "main.h"
#include "device.h"
#include "i2c.h"
#include "timers.h"
#include <stdint.h>

uint8_t buttons_current_state[16];
uint8_t buttons_last_state[16];

#define BUTTONS_ANTIBOUNCE_MS 7
uint16_t buttons_antibounce_counter;

void buttons_init(void) {
    // Set pins as outputs
    BUTTONS_BANK_SELECTOR_PIN_0_TRIS = 0;
    BUTTONS_BANK_SELECTOR_PIN_1_TRIS = 0;
    BUTTONS_BANK_SELECTOR_PIN_2_TRIS = 0;
    BUTTONS_BANK_SELECTOR_PIN_3_TRIS = 0;
    BUTTONS_LED_SELECTOR_PIN_TRIS = 0;
    BUTTONS_LEDS_LATCH_PIN_TRIS = 0;

    BUTTONS_LEDS_LATCH_PIN = 1;  // leds latch high

    // PORT B - decoder status lines (bank buttons state)
    TRISB = 0xFF; // PortB as input
    OPTION_REGbits.nRBPU = 0; // Enable single pullups on PORT B
    WPUB = 0xFF; // Enable all pull-up for PORT B

    // Init internal variables
    for (uint8_t i = 0; i < 16; i++) {
        buttons_current_state[i] = 0;
        buttons_last_state[i] = 0;
    }

    buttons_antibounce_counter = 0;
}

uint16_t buttons_worker_timestamp_ms;

// Scans all buttons, updating buttons_current_state.
inline void buttons_scan(void) {
    TRISB = 0xFF;  // PortB as input

    BUTTONS_LED_SELECTOR_PIN = 0;  // enable buttons/disable leds
    BUTTONS_LEDS_LATCH_PIN = 1;  // leds latch high

    for (uint8_t cur_bank = 0; cur_bank < 16; cur_bank++) {
        // select buttons bank
        BUTTONS_BANK_SELECTOR_PIN_0 = (cur_bank) & 1;
        BUTTONS_BANK_SELECTOR_PIN_1 = (cur_bank >> 1) & 1;
        BUTTONS_BANK_SELECTOR_PIN_2 = (cur_bank >> 2) & 1;
        BUTTONS_BANK_SELECTOR_PIN_3 = (cur_bank >> 3) & 1;

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

    if (buttons_antibounce_counter < BUTTONS_ANTIBOUNCE_MS) {
        buttons_antibounce_counter++;
        return;
    }
    buttons_antibounce_counter = 0;

    buttons_scan();

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
        //uint8_t changes_off = changes & last_state; // 1 -> 0 variations

        buttons_last_state[cur_bank] = current_state;  // Save current state as last

        // Parse changes

        for (uint8_t i = 0; i < 8; i++) {
            if (changes_on & (1 << i)) {
                // Button pressed

                // send button ON event
                I2C_tx(cur_button);
                led_toggle();
            }

            /*if (changes_off & (1 << i)) {
                // Button released
                led_toggle();
            }*/

            cur_button++;
        }
    }
}
