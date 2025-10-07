#ifndef I2C_H
#define	I2C_H

#include <xc.h>

// I2C ADDRESS = BASE + (PIN2 * 2) + PIN1
#define I2C_ADDRESS_BASE	      0x30    // 7bit address
#define I2C_ADDRESS_ADD_PIN1      RC0
#define I2C_ADDRESS_ADD_PIN2      RC5
#define I2C_ADDRESS_ADD_TRIS_PIN1 TRISC0
#define I2C_ADDRESS_ADD_TRIS_PIN2 TRISC5

inline void I2C_isr(void);

void I2C_init(void);
inline void I2C_tx(uint8_t byte);

inline uint8_t I2C_get_rx_buffer_size();
inline uint8_t I2C_get_rx_byte();

#endif
