package devices

import "github.com/marcozaccari/lunatic-midi/devices/hardware"

const (
	LedCount = 60

	MaxSendBytesPerLoop = 16 // 16*8*2.5 = 320us
)

type LedColor byte

const (
	LedOff LedColor = iota
	LedRed
	LedGreen
	LedYellow
	LedBlue
	LedViolet
	LedCyan
	LedWhite
)

type LedStripDevice struct {
	Device

	framebuffer,
	framebufferLast [LedCount]LedColor
}

func NewLedStrip(i2cAddr byte) (*LedStripDevice, error) {
	dev := &LedStripDevice{}

	dev.Device.Type = DeviceLedStrip

	err := dev.i2c.Open(i2cAddr)
	if err != nil {
		return nil, err
	}

	for x := 0; x < LedCount; x++ {
		dev.framebuffer[x] = LedOff
		dev.framebufferLast[x] = LedOff
	}

	// reset led controller
	var buffer hardware.I2CBuffer

	buffer[0] = 0xFF
	err = dev.i2c.Write(&buffer, 1)
	if err != nil {
		return nil, err
	}

	return dev, nil
}

func (dev *LedStripDevice) Done() {
	dev.i2c.Close()
}

func (dev *LedStripDevice) Work() error {
	var buffer hardware.I2CBuffer
	var buffLen int

	for x := 0; x < LedCount; x++ {
		if dev.framebuffer[x] != dev.framebufferLast[x] {
			buffer[buffLen] = byte(x) | 0x80
			buffLen++
			buffer[buffLen] = byte(dev.framebuffer[x])
			buffLen++

			dev.framebufferLast[x] = dev.framebuffer[x]

			if buffLen >= MaxSendBytesPerLoop {
				break
			}
		}
	}

	if buffLen > 0 {
		buffer[buffLen] = 0x40 // repaint command
		buffLen++

		err := dev.i2c.Write(&buffer, buffLen)
		if err != nil {
			return err
		}
	}

	return nil
}

func (dev *LedStripDevice) Fill(color LedColor) {
	for x := 0; x < LedCount; x++ {
		dev.framebuffer[x] = color
	}
}

func (dev *LedStripDevice) Set(index int, color LedColor) {
	if index >= LedCount {
		return
	}

	dev.framebuffer[index] = color
}
