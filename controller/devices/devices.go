package devices

import (
	"fmt"

	"github.com/marcozaccari/lunatic-midi/devices/hardware"
)

type DeviceType int

const (
	DeviceKeyboard DeviceType = iota
	DeviceButtons
	DeviceAnalog
	DeviceLedStrip
)

func (dt DeviceType) String() string {
	switch dt {
	case DeviceKeyboard:
		return "keyboard"
	case DeviceButtons:
		return "buttons"
	case DeviceAnalog:
		return "analog"
	case DeviceLedStrip:
		return "ledstrip"
	default:
		return ""
	}
}

type Device struct {
	Type DeviceType

	i2c hardware.I2C
}

func (dev *Device) GetType() DeviceType {
	return dev.Type
}

func (dev *Device) String() string {
	return fmt.Sprintf("%s(0x%x)", dev.Type.String(), dev.i2c.Address)
}
