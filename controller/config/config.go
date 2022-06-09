package config

import (
	"time"

	"github.com/modulo-srl/mu-config/settings"
)

const Version = "lunatic-midi 0.1"

var Cfg Data

var configCtrl *settings.Settings

const saveAfter = 5 * time.Second

func SaveConfig() {
	configCtrl.SaveSettingsDebounce(saveAfter)
}

func SaveConfigNow() error {
	return configCtrl.SaveSettings()
}

func LoadConfig() error {
	err := configCtrl.LoadSettings(false)
	if err != nil {
		return err
	}

	return nil
}

func GetFilename() string {
	return configCtrl.GetFilename()
}

func GetDefaultConfig() string {
	return ConfigDefaultsRaw
}

// Inizializza le impostazioni.
func Init(filename string) error {
	var err error

	configCtrl, err = settings.New(filename, &Cfg, ConfigDefaults(), false)
	if err != nil {
		return err
	}

	return nil
}

func Done() {
	SaveConfigNow()
}
