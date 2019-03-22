    /*-------------------------------------------------------------*/
    /* termios structure -  /usr/include/asm-generic/termbits.h    */
    /* use "man termios" to get more info about  termios structure */
    /*-------------------------------------------------------------*/

	#include <stdio.h>
	#include <fcntl.h>   // File Control Definitions
	#include <termios.h> // POSIX Terminal Control Definitions
	#include <unistd.h>  // UNIX Standard Definitions
	#include <errno.h>   // ERROR Number Definitions
	#include <stdbool.h>
	#include <stdint.h>

	#include "midi.c"

    int uart;

    bool uart_init() {
	uart = open("/dev/ttyS0", O_RDONLY | O_NOCTTY /*| O_NONBLOCK*/ | O_SYNC);  // no canonical

	if(uart == -1)
	    printf("Error opening serial port\n");

	struct termios SerialPortSettings;
	tcgetattr(uart, &SerialPortSettings);  // get the current attributes of the port

	cfsetispeed(&SerialPortSettings,B115200);  // read speed
	cfsetospeed(&SerialPortSettings,B115200);  // write speed

	// 8N1 mode
	SerialPortSettings.c_cflag &= ~PARENB;  // disables parity enabling bit PARENB
	SerialPortSettings.c_cflag &= ~CSTOPB;  // CSTOPB = 2 stop bits, here it is cleared so 1 stop bit
	SerialPortSettings.c_cflag &= ~CSIZE;  // clears the mask for setting the data size
	SerialPortSettings.c_cflag |=  CS8;  // set the data bits = 8
	SerialPortSettings.c_cflag &= ~CRTSCTS;  // no hardware flow control
	SerialPortSettings.c_cflag &= ~IGNBRK;  // disable break processing
	SerialPortSettings.c_cflag |= CREAD | CLOCAL;  // enable receiver, ignore modem control lines
	SerialPortSettings.c_oflag = 0; // no remapping, no delays
	//SerialPortSettings.c_oflag &= ~OPOST; // no output processing

	SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);  // disable XON/XOFF flow control both i/p and o/p
	SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // non cannonical mode

	SerialPortSettings.c_lflag = 0;  // no signaling chars, no echo, no canonical processing
	
	// setting timeouts (valid in blocking mode only)
	SerialPortSettings.c_cc[VMIN] = 1;
	SerialPortSettings.c_cc[VTIME] = 5;

	if ((tcsetattr(uart,TCSANOW, &SerialPortSettings)) != 0)  // set the attributes to the termios structure
	    printf("ERROR setting attributes");
	
	tcflush(uart, TCIFLUSH);  // discards old data in the rx buffer
    }

    void uart_done() {
	close(uart);
    }



    uint8_t keyb_key;
    bool keyb_keyon;
    uint8_t keyb_velocity;

    void process_keyboard(uint8_t b) {
	if ((b & 0x80) == 0x80) {
	    // key
	    keyb_keyon = (b & 0x40) == 0x40;
	    keyb_key = (b & 0x3F) + 48;

	} else {
	    // velocity
	    keyb_velocity = 127-b+1;

	    if (keyb_keyon)
		midi_note_on(keyb_key, keyb_velocity, b);
	    else
		midi_note_off(keyb_key, keyb_velocity);
	}
    }

    int main(int argc, char **argv) {
	char velocity_filename[256];

	printf("MIDI controller v.1.00\n");

	midi_init();
	uart_init();

	if (argc > 1) {
	    strcpy(velocity_filename, argv[1]);
	    printf("Velocity table: %s\n", velocity_filename);

	    if (!midi_load_velocity_table(velocity_filename)) {
		printf("Error loading velocity table\n");
		return 1;
	    }
	}

	char serbuffer[256];
	char c;
	int  count, k;

	while (1) {
	    count = read(uart, serbuffer, 1);
	    if (count > 0) {
		for(k=0; k < count; k++) {
		    c = serbuffer[k];

		    process_keyboard(c);
		    //printf("%02X ", c);
		}
	    } else {
		//printf(".");
		//usleep(100);
	    }
	}

	midi_done();
	uart_done();
    }
