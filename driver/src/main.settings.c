/* INI settings loader
 */

#include "../lib/zini.h"
#include "../lib/log.h"
#include "main.settings.h"


static void set_default_settings() {
	settings.pid_file[0] = 0;

	settings.keyboard1_i2c_address = 0;
	settings.keyboard2_i2c_address = 0;
	settings.buttons_i2c_address = 0;
	settings.led_monitor_i2c_address = 0;
	settings.adc_i2c_address = 0;
}

static bool load_by_ini(const char* filename, bool should_start){
	if (zini_init(filename)){
		zini_readstring(settings.pid_file, "MAIN", "PID FILE", settings.pid_file);
		
		log_min_level = zini_readinteger("LOGGING", "LOG MIN LEVEL", log_min_level);
		zini_readstring(log_filename, "LOGGING", "LOG FILE", log_filename);
		log_to_syslog = zini_readboolean("LOGGING", "SYSLOG", log_to_syslog);
	  
		settings.keyboard1_i2c_address = zini_readinteger("I2C", "KEYBOARD1", settings.keyboard1_i2c_address);
		settings.keyboard2_i2c_address = zini_readinteger("I2C", "KEYBOARD2", settings.keyboard2_i2c_address);
		settings.buttons_i2c_address = zini_readinteger("I2C", "BUTTONS", settings.buttons_i2c_address);
		settings.led_monitor_i2c_address = zini_readinteger("I2C", "LED MONITOR", settings.led_monitor_i2c_address);
		settings.adc_i2c_address = zini_readinteger("I2C", "ADC", settings.adc_i2c_address);

		if (should_start)
			dlog(_LOG_TERMINAL, "%s settings loaded", filename);
		
	}else{
		if (should_start)
			dlog(_LOG_TERMINAL, "%s file NOT found", filename);
		return false;
	}
	
	return true;
}

bool load_ini_settings(bool should_start) {

	set_default_settings();

	if (!load_by_ini("lunatic-driver.default.ini", should_start))
		return false;

	load_by_ini(settings_filename, should_start);

	if (!should_start) return true;

	return true;
}
