package devices

import (
	"time"

	"github.com/marcozaccari/lunatic-midi/events"
	"github.com/modulo-srl/sparalog/logs"
)

const (
	MaxButtons       = 128
	MaxButtonsEvents = 32

	ReadButtonsEveryMs = 10 // 7ms antibounce lag
)

type ButtonsDevice struct {
	Device

	lastState [MaxButtons]bool

	lastRead time.Time

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

func (dev *ButtonsDevice) Work() (bool, error) {
	if time.Since(dev.lastRead).Milliseconds() < ReadButtonsEveryMs {
		//logs.Trace("buttons: too early")
		return false, nil
	}
	dev.lastRead = time.Now()

	var buffer [256]byte
	var size int
	var b byte
	var event events.Buttons

	err := dev.i2c.Read(buffer[:], 1)
	if err != nil {
		return true, err
	}

	size = int(buffer[0])
	if size == 0 {
		return true, nil
	}

	err = dev.i2c.Read(buffer[:], size+1)
	if err != nil {
		return true, err
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

	return true, nil
}
