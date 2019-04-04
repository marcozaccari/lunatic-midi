/* INI settings loader
 */

#ifndef __MAIN_SETTINGS
#define __MAIN_SETTINGS

#include "../lib/utils.h"
#include "globals.h"

char settings_filename[STR_MAXSIZE];

typedef struct settings_tag {
    char pid_file[STR_MAXSIZE];
    int ipc_port;
    char midi_portname[STR_MAXSIZE];
} settings_t;
settings_t settings;

#ifdef __cplusplus
extern "C" {
#endif

extern bool load_ini_settings(bool should_start);

#ifdef __cplusplus
}
#endif

#endif
