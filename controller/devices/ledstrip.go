package devices

import (
	"fmt"
	"sync"
	"time"

	"github.com/marcozaccari/lunatic-midi/devices/hardware"
)

const (
	LedCount = 60

	LedsMaxSendBytesPerLoop = 16 // 16*8*2.5 = 320us

	LedsWritesDelayMs = 3 // needs a delay of 3ms after every update
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
	i2c hardware.I2C

	offset int

	mu                                     sync.RWMutex
	needsUpdate                            bool
	needsUpdateStartLed, needsUpdateEndLed int
	framebuffer,

	framebufferLast [LedCount]LedColor

	nextWrite time.Time
}

func NewLedStrip(i2cAddr byte, offset int) (*LedStripDevice, error) {
	dev := &LedStripDevice{
		offset: offset,
	}

	err := dev.i2c.Open(i2cAddr)
	if err != nil {
		return nil, err
	}

	for x := 0; x < LedCount; x++ {
		dev.framebuffer[x] = LedOff
		dev.framebufferLast[x] = LedOff
	}

	dev.needsUpdate = false
	dev.needsUpdateStartLed = 0xFF
	dev.needsUpdateEndLed = 0

	// reset controller
	buffer := [1]byte{0xFF}
	err = dev.i2c.Write(buffer[:], 1)
	if err != nil {
		return nil, err
	}

	registerLedStrip(dev)

	return dev, nil
}

func (dev *LedStripDevice) String() string {
	return fmt.Sprintf("ledstrip(0x%x)", dev.i2c.Address)
}

func (dev *LedStripDevice) Fill(color LedColor) {
	dev.mu.Lock()
	defer dev.mu.Unlock()

	for x := 0; x < LedCount; x++ {
		dev.framebuffer[x] = color
	}

	dev.needsUpdate = true
	dev.needsUpdateStartLed = 0
	dev.needsUpdateEndLed = LedCount
}

func (dev *LedStripDevice) Set(index int, color LedColor) {
	dev.mu.Lock()
	defer dev.mu.Unlock()

	index = index - 1 - dev.offset

	if index < 0 || index >= LedCount {
		return
	}

	dev.framebuffer[index] = color

	dev.needsUpdate = true
	if index < dev.needsUpdateStartLed {
		dev.needsUpdateStartLed = index
	}
	if index > dev.needsUpdateEndLed {
		dev.needsUpdateEndLed = index
	}
}

func (dev *LedStripDevice) Blink(index int, color LedColor, count int, on, off time.Duration) {
	dev.Fill(LedOff)
	time.Sleep(time.Millisecond * 10)

	for k := 0; k < count; k++ {
		dev.Set(index, color)
		time.Sleep(on)

		dev.Set(index, LedOff)
		time.Sleep(off)
	}
}

func (dev *LedStripDevice) BlinkAll(color LedColor, count int, on, off time.Duration) {
	dev.Fill(LedOff)
	time.Sleep(time.Millisecond * 10)

	for k := 0; k < count; k++ {
		dev.Fill(color)
		time.Sleep(on)

		dev.Fill(LedOff)
		time.Sleep(off)
	}
}

func (dev *LedStripDevice) done() {
	dev.i2c.Close()
}

func (dev *LedStripDevice) work() (bool, error) {
	if time.Now().Before(dev.nextWrite) {
		//logs.Trace("leds: too early")
		return false, nil
	}

	var buffer [256]byte
	var buffLen int

	dev.mu.RLock()

	if !dev.needsUpdate {
		dev.mu.RUnlock()
		return true, nil
	}

	ledsToUpdate := dev.needsUpdateEndLed - dev.needsUpdateStartLed + 1
	if ledsToUpdate%2 == 1 {
		ledsToUpdate++
	}

	buffer[buffLen] = 0x40 + byte(dev.needsUpdateStartLed)
	buffLen++

	x := dev.needsUpdateStartLed
	for i := 0; i < ledsToUpdate/2; i++ {
		buffer[buffLen] = byte(dev.framebuffer[x])<<3 + byte(dev.framebuffer[x+1])
		buffLen++
		x += 2
	}

	dev.mu.RUnlock()

	// add repaint command
	buffer[buffLen] = 0xC1
	buffLen++

	err := dev.i2c.Write(buffer[:], buffLen)

	dev.nextWrite = time.Now().Add(time.Millisecond * LedsWritesDelayMs)

	if err != nil {
		return true, fmt.Errorf("%s (%d bytes)", err, buffLen)
	}

	dev.needsUpdate = false
	dev.needsUpdateStartLed = 0xFF
	dev.needsUpdateEndLed = 0

	return true, nil
}
