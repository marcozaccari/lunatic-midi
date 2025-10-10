#include "encoder.h"
#include "timers.h"
#include "device.h"
#include "i2c.h"

#define ENCODER_ANTIBOUNCE_MS 30
uint16_t encoder_antibounce_counter;
uint8_t encoder_push_state, encoder_push_last_state;
uint8_t encoder_state, encoder_last_state;

void encoder_init(void) {
    // Set pins as inputs
    ENCODER_A_PIN_TRIS = 1;
    ENCODER_B_PIN_TRIS = 1;
    ENCODER_PUSH_PIN_TRIS = 1;
    
    encoder_antibounce_counter = 0;
    encoder_last_state = 0;
    encoder_push_last_state = 0;
}

inline void check_encoder() {
    if (encoder_state == encoder_last_state)
        return;

    if (((encoder_last_state == 0b11) && (encoder_state == 0b01)) ||
        ((encoder_last_state == 0b01) && (encoder_state == 0b00)) ||
        ((encoder_last_state == 0b00) && (encoder_state == 0b10)) ||
        ((encoder_last_state == 0b10) && (encoder_state == 0b11))) {
        // Rotated right
        led_toggle();
        I2C_tx(ENCODER_LEFT_BUTTON);

    } else {
        // Rotated left
        led_toggle();
        I2C_tx(ENCODER_RIGHT_BUTTON);
    }

    encoder_last_state = encoder_state;
}

inline void check_encoder_button() {
    if (encoder_push_state == encoder_push_last_state)
        return;

led_toggle();        
    encoder_push_last_state = encoder_push_state;

    if (encoder_push_state) {
        // Button pressed

        // send button ON event
        I2C_tx(ENCODER_PUSH_BUTTON);
        led_toggle();
    }
}

uint16_t encoder_worker_timestamp_ms;

inline void encoder_worker(void) {
    uint16_t timestamp = Timer_count;

    if (timestamp == encoder_worker_timestamp_ms)
        return;
    encoder_worker_timestamp_ms = timestamp;

    // Check encoder rotation

    encoder_state = ENCODER_A_PIN;
    encoder_state <<= 1;
    encoder_state += ENCODER_B_PIN;
    check_encoder();

    // Check encoder push button

    encoder_push_state = !ENCODER_PUSH_PIN;

    if (encoder_antibounce_counter < ENCODER_ANTIBOUNCE_MS) {
        encoder_antibounce_counter++;
        return;
    }
    encoder_antibounce_counter = 0;

    encoder_push_state |= !ENCODER_PUSH_PIN;

    check_encoder_button();
}
