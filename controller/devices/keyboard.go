package devices

import (
	"errors"
	"io"
	"os"

	"github.com/marcozaccari/lunatic-midi/devices/hardware"
)

const (
	MaxKeyboardEvents = 64
)

type KeyboardDevice struct {
	Device

	keyOffset      int
	velocityLookup []byte

	lastKey KeyboardEvent

	events      [MaxKeyboardEvents]KeyboardEvent
	eventsCount int
}

type KeyboardEvent struct {
	Key      int
	State    KeyState
	Velocity byte
}

func NewKeyboard(i2cAddr byte, KeyOffset int) (*KeyboardDevice, error) {
	dev := &KeyboardDevice{
		keyOffset:      KeyOffset,
		velocityLookup: make([]byte, 128),
	}

	dev.Device.Type = DeviceKeyboard

	err := dev.i2c.Open(i2cAddr)
	if err != nil {
		return nil, err
	}

	var buffer hardware.I2CBuffer

	// disable velocity on note-off
	buffer[0] = 0x80
	err = dev.i2c.Write(&buffer, 1)
	if err != nil {
		return nil, err
	}

	// initialize velocity table
	for i := 0; i < 128; i++ {
		dev.velocityLookup[i] = byte(i)
	}

	dev.lastKey.Key = -1

	return dev, nil
}

func (dev *KeyboardDevice) Done() {
	dev.i2c.Close()
}

func (dev *KeyboardDevice) Work() error {
	var buffer hardware.I2CBuffer
	var size int

	dev.eventsCount = 0

	err := dev.i2c.Read(&buffer, 1)
	if err != nil {
		return err
	}

	size = int(buffer[0])
	if size == 0 {
		return nil
	}

	err = dev.i2c.Read(&buffer, size+1)
	if err != nil {
		return err
	}

	for k := 1; k < size+1; k++ {
		if buffer[k] != 0xFF {
			dev.parse(buffer[k])
		}
	}

	return nil
}

func (dev *KeyboardDevice) parse(b byte) {
	var keyAbs byte
	var key int
	var keyOn bool
	var velocity byte

	if (b & 0x80) == 0x80 {
		// key
		keyOn = (b & 0x40) == 0x40

		keyAbs = (b & 0x3F)
		key = int(keyAbs) + dev.keyOffset

		dev.lastKey.Key = key

		if keyOn {
			dev.lastKey.State = KeyOn
		} else {
			dev.lastKey.State = KeyOff
		}

	} else {
		velocity = 127 - b + 1
		velocity = dev.velocityLookup[velocity]
		dev.lastKey.Velocity = velocity

		dev.events[dev.eventsCount] = dev.lastKey
		dev.eventsCount++
	}
}

func (dev *KeyboardDevice) LoadVelocity(filename string) error {
	f, err := os.Open(filename)
	if err != nil {
		return err
	}

	n, err := io.ReadAtLeast(f, dev.velocityLookup, 128)
	if err != nil {
		return err
	}
	if n != 128 {
		return errors.New("invalid velocity file")
	}

	f.Close()

	for k := 0; k < 128; k++ {
		if dev.velocityLookup[k] == 0 {
			dev.velocityLookup[k] = 1
		} else if dev.velocityLookup[k] == 128 {
			dev.velocityLookup[k] = 127
		}
	}

	return nil
}
