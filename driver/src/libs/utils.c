#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> // open
#include <inttypes.h>  // uint8_t, etc
#include <sys/ioctl.h>

#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/select.h>

#include "utils.h"


static struct termios old_termios;

void reset_terminal_mode() {
	 tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

void set_terminal_non_canonical() {
	struct termios new_termios;

	tcgetattr(STDIN_FILENO, &old_termios);
	 
	memcpy(&new_termios, &old_termios, sizeof(new_termios));

	/* register cleanup handler, and set the new terminal mode */
	atexit(reset_terminal_mode);
	
	// cfmakeraw(&new_termios);
	/* ICANON normally takes care that one line at a time will be processed
		that means it will return if it sees a "\n" or an EOF or an EOL */
	new_termios.c_lflag &= ~(ICANON | ECHO);
	
	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void get_selfpath(char* path){
	char buff[1024];
	 
	ssize_t len = readlink("/proc/self/exe", buff, sizeof(buff)-1);
	
	if (len != -1){
		char *cutoff = NULL;
				 
		cutoff = strrchr(buff, '/');
		*(cutoff+1) = '\0';      //get rid of the +1 if you don't want the trailing / character
		
	}else{
		buff[0] = '\0';
	}
	
	strcpy(path, buff);
}


bool kbhit() {
	 struct timeval tv = { 0L, 0L };

	 fd_set fds;
	 FD_ZERO(&fds);
	 FD_SET(0, &fds);

	 return select(1, &fds, NULL, NULL, &tv);
}

size_t string_trim(char *out, const char *str) {
	int len = strlen(str);
	if (!len)
		return 0;

	const char *end;
	size_t out_size;

	// Trim leading space
	while ((unsigned char)*str == ' ') 
		str++;

	if (*str == 0) {  // All spaces?
		*out = 0;
		return 0;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && ((unsigned char)*end == ' ')) 
		end--;
	end++;

	out_size = end - str + 1;

	// Copy trimmed string and add null terminator
	memcpy(out, str, out_size);
	out[out_size] = 0;

	return out_size;
}
