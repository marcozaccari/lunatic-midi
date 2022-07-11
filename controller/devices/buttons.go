package devices

import (
	"github.com/marcozaccari/lunatic-midi/events"
	"github.com/modulo-srl/sparalog/logs"
)

const (
	MaxButtons       = 128
	MaxButtonsEvents = 32
)

type ButtonsDevice struct {
	Device

	lastState [MaxButtons]bool

	events events.Channel[events.Buttons]
}

func NewButtons(i2cAddr byte, ch events.Channel[events.Buttons]) (*ButtonsDevice, error) {
	dev := &ButtonsDevice{
		events: ch,
	}

	dev.Device.Type = DeviceButtons

	err := dev.i2c.Open(i2cAddr)
	if err != nil {
		return nil, err
	}

	// Reset led
	buffer := [1]byte{0xFF}
	err = dev.i2c.Write(buffer[:], 1)
	if err != nil {
		return nil, err
	}

	return dev, nil
}

func (dev *ButtonsDevice) Done() {
	dev.i2c.Close()
}

func (dev *ButtonsDevice) Work() error {
	var buffer [256]byte
	var size int
	var b byte
	var event events.Buttons

	err := dev.i2c.Read(buffer[:], 1)
	if err != nil {
		return err
	}

	size = int(buffer[0])
	if size == 0 {
		return nil
	}

	err = dev.i2c.Read(buffer[:], size+1)
	if err != nil {
		return err
	}

	for k := 1; k < size+1; k++ {
		if buffer[k] != 0xFF {
			b = buffer[k]

			if (b & 0x80) == 0x80 {
				event.State = true

				b = b & 0x7F
				event.Button = int(b)
			} else {
				event.State = false

				event.Button = int(b)
			}

			if dev.lastState[event.Button] == event.State {
				// Firmware or hardware error
				logs.Warnf("buttons: ignoring invalid %v", event)
				continue
			}
			dev.lastState[event.Button] = event.State

			dev.events <- event
		}
	}

	return nil
}
