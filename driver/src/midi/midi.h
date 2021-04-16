#ifndef _MIDI_H
#define _MIDI_H

#include <stdbool.h>


// Port format "hw:0,0,0"
bool midi_init(const char *port_name);
void midi_done();

bool midi_note_on(int note, int velocity);
bool midi_note_off(int note, int velocity);

void print_midi_ports();

#endif
