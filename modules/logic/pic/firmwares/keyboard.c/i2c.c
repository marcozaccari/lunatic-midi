#include "i2c.h"

void I2C_init(void) {
    // Set slave address
    unsigned char address = I2C_ADDRESS_BASE;

    I2C_ADDRESS_ADD_TRIS_PIN1 = 1; // set pin as input
    I2C_ADDRESS_ADD_TRIS_PIN2 = 1; // set pin as input

    address = I2C_ADDRESS_ADD_PIN2;
    address <<= 1;
    address += I2C_ADDRESS_ADD_PIN1;
    address += 0x30;

    SSPADD = (unsigned char)(address << 1); // slave address (7 bit) + R/W = 0

    // Slew rate enabled for high speed (400kHz)
    SSPSTATbits.SMP = 0;

    // Configure the port
    SSPCON2bits.GCEN = 0; // general address disabled
    SSPCON = 0b00000110; // 7-bit address, slave

    // Enable the port
    SSPCONbits.SSPEN = 1; 

    PIE1bits.SSPIE = 1; // Enable interrupts
    PIR1bits.SSPIF = 0; // Clear interrupt flag

    INTCONbits.PEIE = 1; //Enable peripheral interrupt
    INTCONbits.GIE = 1;  // Enable global interrupt
}

void I2C_isr(void) {
    unsigned char master_want_write;

    PIR1bits.SSPIF = 0;   // Clear flag

    if (SSPCONbits.SSPOV) {
        SSPCONbits.SSPOV = 0; // clear overflow
        SSPCONbits.CKP = 1; // assure no clock stretch
    }

    // received data or address?
    if (SSPSTATbits.D_nA) { 
        // data
        if (master_want_write) {
            return;
        }
        
        if (SSPCONbits.CKP) {
            // discard interrupt if CKP is up (master end)
            return;
        }

        // TODO if buffer empty
        SSPBUF = 0xFF;  // buffer empty
        SSPCONbits.CKP = 1; // clock stretch

        return;

    } else {
        // address
        master_want_write = 1;

        // receive or write request?
        if (!SSPSTATbits.R_nW) {
            // write request, discard
            return;
        }

        // read request

        if (SSPCONbits.CKP) {
            // discard interrupt if CKP is up (master end)
            return;
        }

        master_want_write = 0;  // set to master-wants-to-read

        SSPBUF = 123;
        SSPCONbits.CKP = 1; // clock stretch

        return;
    }

}
