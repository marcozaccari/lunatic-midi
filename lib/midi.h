#ifndef _MIDI_H
#define _MIDI_H

#include "utils.h"

char midi_portname[128];

bool midi_init();
void midi_done();

void print_midi_ports();

#endif
