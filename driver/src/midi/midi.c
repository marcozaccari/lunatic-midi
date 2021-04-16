// http://www.alsa-project.org/alsa-doc/alsa-lib/rawmidi.html
//
// Using SND_RAWMIDI_NONBLOCK flag for snd_rawmidi_open() or
// snd_rawmidi_open_lconf() instruct device driver to return the -EBUSY
// error when device is already occupied with another application. This
// flag also changes behaviour of snd_rawmidi_write() and snd_rawmidi_read()
// returning -EAGAIN when no more bytes can be processed.
//
// Using SND_RAWMIDI_APPEND flag (output only) instruct device driver to
// append contents of written buffer - passed by snd_rawmidi_write() -
// atomically to output ring buffer in the kernel space. This flag also
// means that the device is not opened exclusively, so more applications can
// share given rawmidi device. Note that applications must send the whole
// MIDI message including the running status, because another writting
// application might break the MIDI message in the output buffer.
//
// Using SND_RAWMIDI_SYNC flag (output only) assures that the contents of
// output buffer specified using snd_rawmidi_write() is always drained before
// the function exits. This behaviour is the same as snd_rawmidi_write()
// followed immediately by snd_rawmidi_drain().
//
// http://www.alsa-project.org/alsa-doc/alsa-lib/group___raw_midi.html
//
// int snd_rawmidi_open(snd_rawmidi_t** input, snd_rawmidi_t output, 
//                                             const char* name, int mode)
//    intput   == returned input handle (NULL if not wanted)
//    output   == returned output handle (NULL if not wanted)
//    name     == ASCII identifier of the rawmidi handle, such as "hw:1,0,0"
//    mode     == open mode (see mode descriptions above):
//                SND_RAWMIDI_NONBLOCK, SND_RAWMIDI_APPEND, SND_RAWMIDI_SYNC
//
// int snd_rawmidi_close(snd_rawmidi_t* rawmidi)
//    Close a deviced opended by snd_rawmidi_open().  Returns an negative 
//    error code if there was an error closing the device.
//
// int snd_rawmidi_write(snd_rawmidi_t* output, char* data, int datasize)
//    output   == midi output pointer setup with snd_rawmidi_open().
//    data     == array of bytes to send.
//    datasize == number of bytes in the data array to write to MIDI output.
//
// const char* snd_strerror(int errornum)
//    errornum == error number returned by an ALSA snd__* function.
//    Returns a string explaining the error number.
//

#include <alsa/asoundlib.h>     /* Interface to the ALSA system */
#include <unistd.h>             /* for sleep() function */
#include <stdint.h>
#include <stdbool.h>
#include "midi.h"
#include "log.h"


/*#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 
*/

snd_rawmidi_t* midiport;

bool midi_init(const char *port_name) {
	int k;
	int status;
	int mode = SND_RAWMIDI_SYNC;
	//const char* portname = "hw:0,0,0";  // see alsarawportlist.c example program

	midiport = NULL;

	if ((status = snd_rawmidi_open(NULL, &midiport, port_name, mode)) < 0) {
		dlog(_LOG_ERROR, "MIDI error: %s", snd_strerror(status));
		return false;
	}

	return true;
}

bool midi_note_on(int note, int velocity) {
	int status;
	char msg[3];

	msg[0] = 0x90;
	msg[1] = note;
	msg[2] = velocity;

	//printf("Note ON: %u %u 0x%02X [0x%02X] (%u 0x%02X "BYTE_TO_BINARY_PATTERN")\n", note, velocity, velocity, calc_velocity, raw, raw, BYTE_TO_BINARY(raw));
	if ((status = snd_rawmidi_write(midiport, msg, 3)) < 0)
		return false; // snd_strerror(status)

	return true;
}

// velocity not used (forced to zero)
bool midi_note_off(int note, int velocity) {
	int status;
	char msg[3];

	msg[0] = 0x90;
	msg[1] = note;
	msg[2] = 0;

	//printf("Note OFF: %u %u\n", note, velocity);

	if ((status = snd_rawmidi_write(midiport, msg, 3)) < 0)
		return false; // snd_strerror(status)

	return true;
}

void midi_done() {
	snd_rawmidi_close(midiport);

	// snd_rawmidi_close() does not clear invalid pointer, 
	// so might be a good idea to erase it after closing.
	midiport = NULL;
}
