#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "libs/utils.h"


typedef struct settings_tag {
    char pid_file[STR_MAXSIZE];

    int ipc_port;
    
    char midi_portname[STR_MAXSIZE];
} settings_t;

extern settings_t settings;

bool load_ini_settings(char *filename);

#endif
