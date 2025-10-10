#include "lights.h"
#include "main.h"
#include "buttons.h"
#include "timers.h"
#include "i2c.h"
#include "device.h"

uint8_t banks_lights_state[16];

void update() {
    TRISB = 0; // PortB as output

    for (uint8_t cur_bank = 0; cur_bank < 16; cur_bank++) {
        BUTTONS_LED_SELECTOR_PIN = 1;  // enable leds/disable buttons
        BUTTONS_LEDS_LATCH_PIN = 0;  // leds latch low

        // select leds bank
        BUTTONS_BANK_SELECTOR_PIN_0 = (cur_bank) & 1;
        BUTTONS_BANK_SELECTOR_PIN_1 = (cur_bank >> 1) & 1;
        BUTTONS_BANK_SELECTOR_PIN_2 = (cur_bank >> 2) & 1;
        BUTTONS_BANK_SELECTOR_PIN_3 = (cur_bank >> 3) & 1;

        uint8_t bank_state = banks_lights_state[cur_bank];
        #ifdef PROGRAMMER_CONNECTED
    	bank_state &= 0b00111111; // remove upper 2 bits (used by programmer)
        #endif

        // output bank state to PORTB (inverted)
        PORTB = ~bank_state;
        __delay_us(2);  

        BUTTONS_LEDS_LATCH_PIN = 1;  // leds latch high
        __delay_us(2);  
    }

    TRISB = 0xFF;  // PortB as input
}

uint16_t flash_counter_ms;
uint8_t flash_counter_step;

void lights_init(void) {
    flash_counter_ms = 0;
    flash_counter_step = 0;

    lights_reset();
}

light_state_t lights_state_h[64];
light_state_t lights_state_l[64];

void lights_reset(void) {
    for (uint8_t i = 0; i < 64; i++) {
        lights_state_h[i] = LIGHT_OFF;
        lights_state_l[i] = LIGHT_OFF;
    }

    for (uint8_t i = 0; i < 16; i++)
        banks_lights_state[i] = 0;

    update();
}

void light_set(uint8_t light, light_state_t state) {
    if (light >= 64)
        lights_state_h[light-64] = state;
    else
        lights_state_h[light] = state;

    uint8_t bank = light >> 3;
    uint8_t bank_state = banks_lights_state[bank];
    uint8_t light_bit = light & 0b111;

    if (state == LIGHT_OFF)
        bank_state &= ~(1 << light_bit);
    else
        bank_state |= (1 << light_bit);

    banks_lights_state[bank] = bank_state;

    update();
}

uint16_t lights_worker_timestamp_ms;

void lights_worker(void) {
    uint16_t timestamp = Timer_count;

    if (timestamp == lights_worker_timestamp_ms)
        return;
    lights_worker_timestamp_ms = timestamp;

    //led_toggle();

    flash_counter_ms++;
    if (flash_counter_ms < LIGHTS_FLASH_HIGH_MS)
        return;
    flash_counter_ms  = 0;

    uint8_t cur_light = 0;
    for (uint8_t cur_bank = 0; cur_bank < 16; cur_bank++) {
        uint8_t bank_state = banks_lights_state[cur_bank];

        for (uint8_t cur_bit = 0; cur_bit < 8; cur_bit++) {
            light_state_t light_state;
            if (cur_light >= 64)
                light_state = lights_state_h[cur_light-64];
            else
                light_state = lights_state_l[cur_light];
            
            switch (light_state) {
                case LIGHT_OFF: 
                    bank_state &= ~(1 << cur_bit);
                    break;

                case LIGHT_ON:
                    bank_state |= (1 << cur_bit);
                    break;

                case LIGHT_FLASH_LOW:
                    if (flash_counter_step < 2)
                        bank_state |= (1 << cur_bit);
                    else
                        bank_state &= ~(1 << cur_bit);
                    break;

                case LIGHT_FLASH_HIGH:
                    if (flash_counter_step & 1)
                        bank_state &= ~(1 << cur_bit);
                    else
                        bank_state |= (1 << cur_bit);
                    break;
            }

            cur_light++;
        }

        banks_lights_state[cur_bank] = bank_state;
    }

    flash_counter_step++;
    if (flash_counter_step == 4)
        flash_counter_step = 0;

    update();
}
