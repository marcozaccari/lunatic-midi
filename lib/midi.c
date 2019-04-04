// On Debian/Ubuntu:
// sudo apt install libasound2-dev
//
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


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


snd_rawmidi_t* midiport;

// Velocity lookup table
uint8_t velocity_table[128];


bool midi_init() {
   int k;
   int status;
   int mode = SND_RAWMIDI_SYNC;
   //const char* portname = "hw:0,0,0";  // see alsarawportlist.c example program

   midiport = NULL;

   if ((status = snd_rawmidi_open(NULL, &midiport, midi_portname, mode)) < 0) {
      dlog(_LOG_ERROR, "Problem opening MIDI port %s: %s", midi_portname, snd_strerror(status));
      return false;
   }

   // initialize velocity table
   for (k=0; k < 128; k++)
	velocity_table[k] = k;
  
   dlog(_LOG_NOTICE, "MIDI opened on port: %s", midi_portname);

   return true;
}

bool midi_note_on(uint8_t note, uint8_t velocity, uint8_t raw) {
   int status;
   char msg[3];
   uint8_t calc_velocity;

   calc_velocity = velocity_table[velocity];

   msg[0] = 0x90;
   msg[1] = note;
   msg[2] = calc_velocity;

   printf("Note ON: %u %u 0x%02X [0x%02X] (%u 0x%02X "BYTE_TO_BINARY_PATTERN")\n", note, velocity, velocity, calc_velocity, raw, raw, BYTE_TO_BINARY(raw));
   if ((status = snd_rawmidi_write(midiport, msg, 3)) < 0)
	return false; // snd_strerror(status)

   return true;
}


bool midi_note_off(uint8_t note, uint8_t velocity) {
   int status;
   char msg[3];

   msg[0] = 0x90;
   msg[1] = note;
   msg[2] = 0;

//    printf("Note OFF: %u %u\n", note, velocity);

   if ((status = snd_rawmidi_write(midiport, msg, 3)) < 0)
	return false; // snd_strerror(status)

   return true;
}

void midi_done() {
   snd_rawmidi_close(midiport);
   midiport = NULL;    // snd_rawmidi_close() does not clear invalid pointer, so might be a good idea to erase it after closing.
}

bool midi_load_velocity_table(char *filename) {
   FILE *fp;
   int res;
   int k;

   fp = fopen(filename, "r");
   if (!fp)
	return false;

   res = fread(velocity_table, 1, 128, fp);
   if (res < 128)
	return false;

   fclose(fp);

   for (k=0; k < 128; k++) {
      if (velocity_table[k] == 0)
         velocity_table[k] = 1;
      else if (velocity_table[k] == 128)
         velocity_table[k] = 127;
   }

   return true;
}


// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// Print MIDI ports
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************


//////////////////////////////
//
// error -- print error message
//

void error(const char *format, ...) {
   va_list ap;
   va_start(ap, format);
   vfprintf(stderr, format, ap);
   va_end(ap);
   putc('\n', stderr);
}

//////////////////////////////
//
// is_input -- returns true if specified card/device/sub can output MIDI data.
//

int is_input(snd_ctl_t *ctl, int device, int sub) {
   snd_rawmidi_info_t *info;
   int status;

   snd_rawmidi_info_alloca(&info);
   snd_rawmidi_info_set_device(info, device);
   snd_rawmidi_info_set_subdevice(info, sub);
   snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
   
   if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0 && status != -ENXIO) {
      return status;
   } else if (status == 0) {
      return 1;
   }

   return 0;
}

//////////////////////////////
//
// is_output -- returns true if specified card/device/sub can output MIDI data.
//

int is_output(snd_ctl_t *ctl, int device, int sub) {
   snd_rawmidi_info_t *info;
   int status;

   snd_rawmidi_info_alloca(&info);
   snd_rawmidi_info_set_device(info, device);
   snd_rawmidi_info_set_subdevice(info, sub);
   snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
   
   if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0 && status != -ENXIO) {
      return status;
   } else if (status == 0) {
      return 1;
   }

   return 0;
}

//////////////////////////////
//
// list_subdevice_info -- Print information about a subdevice
//   of a device of a card if it can handle MIDI input and/or output.
//

void list_subdevice_info(snd_ctl_t *ctl, int card, int device) {
   snd_rawmidi_info_t *info;
   const char *name;
   const char *sub_name;
   int subs, subs_in, subs_out;
   int sub, in, out;
   int status;

   snd_rawmidi_info_alloca(&info);
   snd_rawmidi_info_set_device(info, device);

   snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
   snd_ctl_rawmidi_info(ctl, info);
   subs_in = snd_rawmidi_info_get_subdevices_count(info);
   snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
   snd_ctl_rawmidi_info(ctl, info);
   subs_out = snd_rawmidi_info_get_subdevices_count(info);
   subs = subs_in > subs_out ? subs_in : subs_out;

   sub = 0;
   in = out = 0;
   if ((status = is_output(ctl, device, sub)) < 0) {
      error("cannot get rawmidi information %d:%d: %s",
            card, device, snd_strerror(status));
      return;
   } else if (status)
      out = 1;

   if (status == 0) {
      if ((status = is_input(ctl, device, sub)) < 0) {
         error("cannot get rawmidi information %d:%d: %s",
               card, device, snd_strerror(status));
         return;
      }
   } else if (status) 
      in = 1;

   if (status == 0)
      return;

   name = snd_rawmidi_info_get_name(info);
   sub_name = snd_rawmidi_info_get_subdevice_name(info);
   if (sub_name[0] == '\0') {
      if (subs == 1) {
         printf("%c%c  hw:%d,%d    %s\n", 
                in  ? 'I' : ' ', 
                out ? 'O' : ' ',
                card, device, name);
      } else
         printf("%c%c  hw:%d,%d    %s (%d subdevices)\n",
                in  ? 'I' : ' ', 
                out ? 'O' : ' ',
                card, device, name, subs);
   } else {
      sub = 0;
      for (;;) {
         printf("%c%c  hw:%d,%d,%d  %s\n",
                in ? 'I' : ' ', out ? 'O' : ' ',
                card, device, sub, sub_name);
         if (++sub >= subs)
            break;

         in = is_input(ctl, device, sub);
         out = is_output(ctl, device, sub);
         snd_rawmidi_info_set_subdevice(info, sub);
         if (out) {
            snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
            if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0) {
               error("cannot get rawmidi information %d:%d:%d: %s",
                     card, device, sub, snd_strerror(status));
               break;
            } 
         } else {
            snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
            if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0) {
               error("cannot get rawmidi information %d:%d:%d: %s",
                     card, device, sub, snd_strerror(status));
               break;
            }
         }
         sub_name = snd_rawmidi_info_get_subdevice_name(info);
      }
   }
}

//////////////////////////////
//
// list_midi_devices_on_card -- For a particular "card" look at all
//   of the "devices/subdevices" on it and print information about it
//   if it can handle MIDI input and/or output.
//

void list_midi_devices_on_card(int card) {
   snd_ctl_t *ctl;
   char name[32];
   int device = -1;
   int status;
   sprintf(name, "hw:%d", card);
   if ((status = snd_ctl_open(&ctl, name, 0)) < 0) {
      error("cannot open control for card %d: %s", card, snd_strerror(status));
      return;
   }
   do {
      status = snd_ctl_rawmidi_next_device(ctl, &device);
      if (status < 0) {
         error("cannot determine device number: %s", snd_strerror(status));
         break;
      }
      if (device >= 0) {
         list_subdevice_info(ctl, card, device);
      }
   } while (device >= 0);
   snd_ctl_close(ctl);
}

//////////////////////////////
//
// print_midi_ports -- go through the list of available "soundcards",
//   checking them to see if there are devices/subdevices on them which
//   can read/write MIDI data.
//

void print_midi_ports(void) {
   int status;
   int card = -1;  // use -1 to prime the pump of iterating through card list

   if ((status = snd_card_next(&card)) < 0) {
      error("cannot determine card number: %s", snd_strerror(status));
      return;
   }
   if (card < 0) {
      error("no sound cards found");
      return;
   }
   printf("\nDir Device    Name\n");
   printf("====================================\n");
   while (card >= 0) {
      list_midi_devices_on_card(card);
      if ((status = snd_card_next(&card)) < 0) {
         error("cannot determine card number: %s", snd_strerror(status));
         break;
      }
   } 
   printf("\n");
}
