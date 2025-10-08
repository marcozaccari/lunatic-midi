#include "i2c.h"
#include "device.h"

// Circular buffers
#define TXRX_BUFFER_SIZE 32

volatile uint8_t tx_buffer[TXRX_BUFFER_SIZE];
volatile uint8_t tx_buffer_head;
volatile uint8_t tx_buffer_tail;

volatile uint8_t rx_buffer[TXRX_BUFFER_SIZE];
volatile uint8_t rx_buffer_head;
volatile uint8_t rx_buffer_tail;

// Add a byte to the TX buffer.
inline void I2C_tx(uint8_t byte) {
    tx_buffer[tx_buffer_head] = byte;
    tx_buffer_head = (tx_buffer_head + 1) % TXRX_BUFFER_SIZE;
}

// Add a byte to the RX buffer.
inline void I2C_rx(uint8_t byte) {
    rx_buffer[rx_buffer_head] = byte;
    rx_buffer_head = (rx_buffer_head + 1) % TXRX_BUFFER_SIZE;
}

// Get the number of bytes in the TX buffer.
inline uint8_t I2C_get_tx_buffer_size() {
    if (tx_buffer_head >= tx_buffer_tail) {
        return tx_buffer_head - tx_buffer_tail;
    } else {
        return TXRX_BUFFER_SIZE - (tx_buffer_tail - tx_buffer_head);
    }    
}

// Get the number of bytes in the RX buffer.
inline uint8_t I2C_get_rx_buffer_size() {
    if (rx_buffer_head >= rx_buffer_tail) {
        return rx_buffer_head - rx_buffer_tail;
    } else {
        return TXRX_BUFFER_SIZE - (rx_buffer_tail - rx_buffer_head);
    }    
}

// Get the first byte from the TX buffer.
// Returns 0xFF when buffer is empty.
inline uint8_t I2C_get_tx_byte() {
    if (tx_buffer_head == tx_buffer_tail) {
        // buffer empty, return 0xFF
        return 0xFF;
    }

    uint8_t byte = tx_buffer[tx_buffer_tail];
    tx_buffer_tail = (tx_buffer_tail + 1) % TXRX_BUFFER_SIZE;

    return byte;
}

// Get the first byte from the RX buffer.
// Returns 0xFF when buffer is empty.
inline uint8_t I2C_get_rx_byte() {
    if (rx_buffer_head == rx_buffer_tail) {
        // buffer empty, return 0xFF
        return 0xFF;
    }

    uint8_t byte = rx_buffer[rx_buffer_tail];
    rx_buffer_tail = (rx_buffer_tail + 1) % TXRX_BUFFER_SIZE;

    return byte;
}

void I2C_reset(void) {
    tx_buffer_head = 0;
    tx_buffer_tail = 0;
    
    rx_buffer_head = 0;
    rx_buffer_tail = 0;
}

void I2C_init(void) {
    // Init internal variables
    I2C_reset();

    // Set slave address
    uint8_t address = I2C_ADDRESS_BASE;

    I2C_ADDRESS_ADD_TRIS_PIN1 = 1; // set pin as input
    I2C_ADDRESS_ADD_TRIS_PIN2 = 1; // set pin as input

    address = I2C_ADDRESS_ADD_PIN2;
    address <<= 1;
    address += I2C_ADDRESS_ADD_PIN1;
    address += 0x30;

    SSPADD = (uint8_t)(address << 1); // slave address (7 bit) + R/W = 0

    SSPSTATbits.SMP = 0; // Slew rate enabled for high speed (400kHz)
    //SSPSTATbits.SMP = 1; // Slew rate control disabled for standard speed mode (100 kHz and 1 MHz)

    // Configure the port
    SSPCON2bits.GCEN = 0; // General call address disabled

    SSPCON = 0b00000110; // I2C slave mode, 7-bit address

    SSPCONbits.SSPEN = 1; // Enables the serial port and configures the SDA and SCL pins as the source of the serial port pins

    PIE1bits.SSPIE = 1; // Enable interrupts for I2C
}

volatile unsigned char master_wants_read;

inline void I2C_isr(void) {
    if (SSPCONbits.SSPOV || SSPCONbits.WCOL) {
        // Overflow or collision
        // WCOL: The SSPBUF register is written while it is still transmitting the previous word (must be cleared in software)
        // SSPOV: A byte is received while the SSPBUF register is still holding the previous byte. SSPOV is a “don’t care” in Transmit mode (must be cleared in software)
        SSPCONbits.SSPOV = 0;
        SSPCONbits.WCOL = 0;
        //SSPCONbits.CKP = 1; // assure no clock stretch
        //led_toggle();
        return;
    }
    /*if (SSPCONbits.SSPOV) {
        // Overflow
        SSPCONbits.SSPOV = 0; // clear overflow
        SSPCONbits.CKP = 1; // assure no clock stretch
    }*/

    // received data or address?
    if (!SSPSTATbits.D_nA) { 
        // received address

        (void)SSPBUF; // dummy read

        // receive or write request?
        if (!SSPSTATbits.R_nW) {
            // write request
            master_wants_read = 0;
            SSPCONbits.CKP = 1; // release clock
            return;
        }

        // read request

        master_wants_read = 1;  // set to master-wants-to-read

        if (SSPCONbits.CKP) {
            //led_toggle();
            return;  // discard interrupt if CKP is up (master end without reading)
        }

        SSPBUF = I2C_get_tx_buffer_size();  // send buffer size
        SSPCONbits.CKP = 1; // release clock
        return;
    }

    // data

    if (!master_wants_read) { // master wants to write data
        I2C_rx(SSPBUF);  // add new data to the RX buffer
        return;
    }
    
    (void)SSPBUF; // dummy read

    if (SSPCONbits.CKP) {
        //led_toggle();
        return;  // discard interrupt if CKP is up (master end without reading)
    }

    // Send next TX buffer byte
    SSPBUF = I2C_get_tx_byte();
    SSPCONbits.CKP = 1; // release clock
}
