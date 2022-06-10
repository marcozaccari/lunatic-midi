package devices

import "github.com/marcozaccari/lunatic-midi/devices/hardware"

type DeviceType int

const (
	DeviceKeyboard DeviceType = iota
	DeviceButtons
	DeviceAnalog
	DeviceLedStrip
)

type Device struct {
	Type DeviceType

	i2c hardware.I2C
}

type KeyState bool

const (
	KeyOff KeyState = false
	KeyOn  KeyState = true
)
