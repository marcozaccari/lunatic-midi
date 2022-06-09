package config

// Depends: go install github.com/marco-sacchi/go2jsonc/cmd/go2jsonc
// go:generate go2jsonc -type=Config -out=default.jsonc

// Depends: go get -u github.com/modulo-srl/mu-config/json2struct
//go:generate json2struct -out=data.go defaults.jsonc

import (
	_ "embed"
)

type Config struct {
	MIDI    MIDI
	Devices Devices
}

type MIDI struct {
	// Get ports names using "amidi -l" or "lunatic-midi midiports"
	// In order to obtain virtual ports, use:
	//   "sudo modprobe snd-virmidi" or
	//   "sudo echo 'snd-virmidi' >> /etc/modules" or
	//   "sudo echo 'snd-virmidi index=n' >> /etc/modprobe.d/snd-virmidi.conf"
	//
	// Raspberry Zero: "hw:2,0,0"
	PortName string
}

type Devices struct {
	Keyboards []DeviceKeyboard
	Buttons   DeviceButtons
	Analogs   []DeviceAnalog
	LedStrip  DeviceLedStrip
}

type DeviceKeyboard struct {
	I2C      string // I2C address (ex "0x30")
	Offset   int    // Notes offset
	Velocity string // Velocity curve (data/velocity/*.dat)
}

type DeviceButtons struct {
	I2C string // I2C address (ex "0x30")
}

type DeviceAnalog struct {
	I2C string

	// Channel type: "slider" (default) / "ribbon"
	Channel1 string
	Channel2 string
	Channel3 string
	Channel4 string
}

type DeviceLedStrip struct {
	I2C string // I2C address (ex "0x30")
}

func ConfigDefaults() *Config {
	return &Config{

		MIDI: MIDI{
			PortName: "hw:0,0,0",
		},

		Devices: Devices{},
	}
}

//go:embed defaults.jsonc
var ConfigDefaultsRaw string
