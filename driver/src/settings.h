#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "libs/utils.h"


typedef struct settings_tag {
    char pid_file[STR_MAXSIZE];
} settings_t;
settings_t settings;

bool load_ini_settings(char *filename);

#endif
