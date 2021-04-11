#ifndef _MIDI_H
#define _MIDI_H

#include <stdint.h>
#include "utils.h"


char midi_portname[128];

bool midi_init();
void midi_done();

void print_midi_ports();

bool midi_note_on(uint8_t note, uint8_t velocity);
bool midi_note_off(uint8_t note, uint8_t velocity);

#endif
