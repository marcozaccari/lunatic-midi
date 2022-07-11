package devices

import (
	"fmt"
	"sync"
	"time"
)

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

	mu sync.RWMutex
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
	buffer := [1]byte{0xFF}
	err = dev.i2c.Write(buffer[:], 1)
	if err != nil {
		return nil, err
	}

	return dev, nil
}

func (dev *LedStripDevice) Done() {
	dev.i2c.Close()
}

func (dev *LedStripDevice) Work() error {
	var buffer [256]byte
	var buffLen int

	dev.mu.RLock()

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

	dev.mu.RUnlock()

	if buffLen > 0 {
		// add repaint command
		buffer[buffLen] = 0x40
		buffLen++

		err := dev.i2c.Write(buffer[:], buffLen)
		if err != nil {
			return fmt.Errorf("%s (%d bytes)", err, buffLen)
		}
	}

	return nil
}

func (dev *LedStripDevice) Fill(color LedColor) {
	dev.mu.Lock()
	defer dev.mu.Unlock()

	for x := 0; x < LedCount; x++ {
		dev.framebuffer[x] = color
	}
}

func (dev *LedStripDevice) Set(index int, color LedColor) {
	dev.mu.Lock()
	defer dev.mu.Unlock()

	if index < 0 || index >= LedCount {
		return
	}

	dev.framebuffer[index] = color
}

func (dev *LedStripDevice) Blink(color LedColor, count int, on, off time.Duration) {
	dev.mu.Lock()
	defer dev.mu.Unlock()

	dev.Fill(LedOff)
	time.Sleep(time.Millisecond * 10)

	for k := 0; k < count; k++ {
		dev.Fill(color)
		time.Sleep(on)

		dev.Fill(LedOff)
		time.Sleep(off)
	}
}
