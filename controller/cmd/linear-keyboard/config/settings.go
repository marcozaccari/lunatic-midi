package config

// Depends: go install github.com/marco-sacchi/go2jsonc/cmd/go2jsonc@latest
//go:generate go2jsonc -type=Settings -out=defaults.jsonc -doc-types=NotFields

import (
	_ "embed"
)

type Settings struct {
	MIDI    MIDI
	Devices Devices

	Logging Log
}

type MIDI struct {
	// Get ports names using "amidi -l" or "linear-keyboard midiports"
	// In order to obtain virtual ports, use:
	//   "sudo modprobe snd-virmidi" or
	//   "sudo echo 'snd-virmidi' >> /etc/modules" or
	//   "sudo echo 'snd-virmidi index=n' >> /etc/modprobe.d/snd-virmidi.conf"
	//
	// Raspberry Zero: "hw:2,0,0"
	PortName string
}

type Devices struct {
	Keyboard DeviceKeyboard
	Buttons  DeviceButtons
	Analog   DeviceAnalog
	LedStrip DeviceLedStrip
}

type DeviceKeyboard struct {
	I2C string // I2C address (ex "0x30")

	Velocity string // Velocity curve (see bin/velocity/*.dat): "hard1", "soft2"..
}

type DeviceButtons struct {
	I2C string // I2C address (ex "0x30")
}

type DeviceAnalog struct {
	I2C string

	Channels [4]AnalogChannel
}

type AnalogChannel struct {
	// Channel type: "slider" / "ribbon"
	Type string

	Bits int

	RawMin int
	RawMax int
}

type DeviceLedStrip struct {
	I2C string // I2C address (ex "0x30")
}

type Log struct {
	TCPPort int // 0 = disabled
	File    string
	Stdout  bool
	Syslog  bool
}

func SettingsDefaults() *Settings {
	return &Settings{

		MIDI: MIDI{
			PortName: "hw:0,0,0",
		},

		Devices: Devices{
			Keyboard: DeviceKeyboard{},
			Buttons:  DeviceButtons{},
			Analog: DeviceAnalog{
				Channels: [4]AnalogChannel{{}, {}, {}, {}},
			},
			LedStrip: DeviceLedStrip{},
		},

		Logging: Log{
			TCPPort: 0,
			File:    "",
			Stdout:  false,
			Syslog:  false,
		},
	}
}

//go:embed defaults.jsonc
var SettingsDefaultsRaw string
