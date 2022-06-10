package devices

import "github.com/marcozaccari/lunatic-midi/devices/hardware"

const (
	MaxButtonsEvents = 32
)

type ButtonsDevice struct {
	Device

	events      [MaxButtonsEvents]ButtonsEvent
	eventsCount int
}

type ButtonsEvent struct {
	Button int
	State  KeyState
}

func NewButtons(i2cAddr byte) (*ButtonsDevice, error) {
	dev := &ButtonsDevice{}

	dev.Device.Type = DeviceButtons

	err := dev.i2c.Open(i2cAddr)
	if err != nil {
		return nil, err
	}

	var buffer hardware.I2CBuffer

	// Reset led
	buffer[0] = 0xFF
	err = dev.i2c.Write(&buffer, 1)
	if err != nil {
		return nil, err
	}

	return dev, nil
}

func (dev *ButtonsDevice) Done() {
	dev.i2c.Close()
}

func (dev *ButtonsDevice) Work() error {
	var buffer hardware.I2CBuffer
	var size int
	var b byte
	var event ButtonsEvent

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
			b = buffer[k]

			if (b & 0x80) == 0x80 {
				b = b & 0x7F

				event.Button = int(b)
				event.State = KeyOn
			} else {
				event.Button = int(b)
				event.State = KeyOff
			}

			dev.events[dev.eventsCount] = event
			dev.eventsCount++
		}
	}

	return nil
}
