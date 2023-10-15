package config

import (
	"fmt"
	"strconv"

	"github.com/marcozaccari/lunatic-midi/cmd/linear-keyboard/worker"
	"github.com/marcozaccari/lunatic-midi/devices"
	"github.com/modulo-srl/mu-config/settings"
	"github.com/modulo-srl/sparalog/logs"
)

const Version = "lunatic linear keyboard 0.2"

var Cfg = SettingsDefaults()

func LoadConfig(filename string) error {
	return settings.LoadFile(filename, &Cfg)
}

func SaveConfig(filename string) error {
	return settings.SaveFile(filename, Cfg, SettingsDefaults())
}

func GetDefaultConfig() string {
	return SettingsDefaultsRaw
}

func ParseConfig() (devs worker.OutputDevices, err error) {
	// Midi.
	portName := Cfg.MIDI.PortName

	midi, err := devices.NewMidi(&portName)
	if err != nil {
		err = fmt.Errorf("initializing MIDI port \"%s\": %s", portName, err)
		return
	}

	logs.Infof("MIDI initialized (%s)", portName)

	// Keyboard.
	velocityPath := ""

	addr, err := strconv.ParseInt(Cfg.Devices.Keyboard.I2C, 0, 0)
	if err != nil {
		return
	}

	keyb, err := devices.NewKeyboard(byte(addr), 0)
	if err != nil {
		return
	}

	if Cfg.Devices.Keyboard.Velocity != "" {
		err = keyb.LoadVelocity(velocityPath + "/" + Cfg.Devices.Keyboard.Velocity + ".dat")
		if err != nil {
			return
		}
	}

	logs.Infof("Keyboard initialized (%s)", Cfg.Devices.Keyboard.I2C)

	// Buttons.
	addr, err = strconv.ParseInt(Cfg.Devices.Buttons.I2C, 0, 0)
	if err != nil {
		return
	}

	buttons, err := devices.NewButtons(byte(addr), 0)
	if err != nil {
		return
	}

	logs.Infof("Buttons initialized (%s)", Cfg.Devices.Keyboard.I2C)

	// Analog.
	addr, err = strconv.ParseInt(Cfg.Devices.Analog.I2C, 0, 0)
	if err != nil {
		return
	}

	ana, err := devices.NewAnalog(byte(addr), 0)
	if err != nil {
		return
	}

	for i, cfgAna := range Cfg.Devices.Analog.Channels {
		if cfgAna.Type == "" || cfgAna.Bits == 0 {
			continue
		}

		var typ devices.AnalogChannelType
		switch cfgAna.Type {
		case "slider":
			typ = devices.AnalogChannelSlider
		case "ribbon":
			typ = devices.AnalogChannelRibbon
		default:
			err = fmt.Errorf("invalid analog channel type: %s", cfgAna.Type)
			return
		}
		ana.SetChannelType(i, typ, cfgAna.Bits, uint(cfgAna.RawMin), uint(cfgAna.RawMax))
	}

	logs.Infof("Analog initialized (%s)", Cfg.Devices.Analog.I2C)

	// Led strip.
	addr, err = strconv.ParseInt(Cfg.Devices.LedStrip.I2C, 0, 0)
	if err != nil {
		return
	}

	leds, err := devices.NewLedStrip(byte(addr), 1)
	if err != nil {
		return
	}

	logs.Infof("LedStrip initialized (%s)", Cfg.Devices.LedStrip.I2C)

	devs.Midi = midi
	devs.Buttons = buttons
	devs.Leds = leds
	return
}
