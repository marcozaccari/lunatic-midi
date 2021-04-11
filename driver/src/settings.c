/* INI settings loader
 */

#include <string.h>

#include "libs/utils.h"
#include "libs/ini.h"
#include "libs/log.h"

#include "devices/keyboard.h"
#include "devices/buttons.h"
#include "devices/analog.h"
#include "devices/ledstrip.h"

#include "settings.h"


static void set_default_settings() {
	settings.pid_file[0] = 0;
}

static bool get_string(ini_t *cfg, const char *section, const char *key, char *dest) {
	char *s = (char*)ini_get(cfg, section, key);
	if (!s)
		return false;

	strcpy(dest, s);

	return true;
}

static bool get_integer(ini_t *cfg, const char *section, const char *key, bool hex, int *dest) {
	const char *pattern_int = "%d";
	const char *pattern_hex = "0x%2x";
	const char *pattern;

	if (hex)
		pattern = pattern_hex;
	else
		pattern = pattern_int;

	int i;
	if (!ini_sget(cfg, section, key, pattern, &i))
		return false;

	*dest = i;

	return true;
}

static bool get_boolean(ini_t *cfg, const char *section, const char *key, bool *dest) {
	char *s = (char*)ini_get(cfg, section, key);
	if (!s)
		return false;

	*dest = (strcmp(s, "true") == 0);

	return true;
}

static bool load_device(ini_t *cfg, char *device_name) {
	static int keyboard_offset = 0;

	char section[STR_MAXSIZE];
	strcpy(section, "device ");
	strcat(section, device_name);

	int i2c_address;
	if (!get_integer(cfg, section, "i2c", true, &i2c_address)) {
		dlog(_LOG_WARNING, "I2C address not specified in section \"[%s]\"", section);
		return false;
	}
	if (!i2c_address) {
		dlog(_LOG_WARNING, "Invalid I2C address: 0x%02X", i2c_address);
		return false;
	}
	dlog(_LOG_TERMINAL, "I2C address: 0x%02X", i2c_address);

	char type[STR_MAXSIZE];
	if (!get_string(cfg, section, "type", type)) {
		dlog(_LOG_WARNING, "Cannot load device \"%s\"", device_name);
		return false;
	}

	if (strcmp(type, "keyboard") == 0) {
		if (!get_integer(cfg, section, "offset", false, &keyboard_offset))
			keyboard_offset += 128;

		dlog(_LOG_TERMINAL, "Keyboard offset: %d", keyboard_offset);

		new_device_keyboard(device_name, i2c_address, keyboard_offset);
	
	} else if (strcmp(type, "buttons") == 0) {
		new_device_buttons(device_name, i2c_address);

	} else if (strcmp(type, "analog") == 0) {
		new_device_analog(device_name, i2c_address);

	} else if (strcmp(type, "led strip") == 0) {
		new_device_ledstrip(device_name, i2c_address);

	} else {
		dlog(_LOG_WARNING, "Device type unknown \"%s\"", type);
		return false;
	}

	return true;
}

static bool load_by_ini(const char* filename) {
	ini_t *cfg = ini_load(filename);

	if (!cfg) {
		dlog(_LOG_ERROR, "File not loaded \"%s\"", filename);
		return false;
	}
	dlog(_LOG_TERMINAL, "Load settings from \"%s\"", filename);

	get_string(cfg, "main", "pid file", settings.pid_file);

	// Logging
	get_string(cfg, "logging", "log file", log_filename);
	get_integer(cfg, "logging", "min level", false, (int*)&log_min_level);
	get_boolean(cfg, "logging", "syslog", &log_to_syslog);
	
	// Devices
    char s[STR_MAXSIZE];
	if (get_string(cfg, "main", "devices", s)) {
		char *ptr = strtok(s, ",");
		while (ptr != NULL) {
			char trimmed[STR_MAXSIZE];
			string_trim(trimmed, ptr);

			if (strlen(trimmed)) {
				dlog(_LOG_TERMINAL, "Load device: %s", trimmed);
				load_device(cfg, trimmed);
			}

			ptr = strtok(NULL, ",");
		}
	}

	return true;
}

bool load_ini_settings(char *filename) {

	set_default_settings();

	if (!load_by_ini("lunatic-driver.default.ini"))
		return false;

	load_by_ini(filename);

	return true;
}
