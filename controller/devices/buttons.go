package devices

import (
	"fmt"
	"sync"
	"time"

	"github.com/marcozaccari/lunatic-midi/devices/hardware"
	"github.com/modulo-srl/sparalog/logs"
)

const (
	MaxButtons       = 128
	MaxButtonsEvents = 32

	ButtonsMaxSendBytesPerLoop = 16 // 16*8*2.5 = 320us

	ReadButtonsEveryMs = 10 // 7ms antibounce lag
)

type ButtonsDevice struct {
	i2c hardware.I2C

	lastState [MaxButtons]bool

	lastRead time.Time

	btnOffset int
	events    ButtonsEvents

	mu sync.RWMutex
	lightState,

	lightStateLast [MaxButtons]bool
}

func NewButtons(i2cAddr byte, BtnOffset int) (*ButtonsDevice, error) {
	dev := &ButtonsDevice{
		btnOffset: BtnOffset,
	}

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

	dev.events = registerButtons(dev)

	return dev, nil
}

func (dev *ButtonsDevice) String() string {
	return fmt.Sprintf("buttons(0x%x)", dev.i2c.Address)
}

func (dev *ButtonsDevice) SetLight(index int, state bool) {
	dev.mu.Lock()
	defer dev.mu.Unlock()

	if index < 0 || index >= MaxButtons {
		return
	}

	dev.lightState[index] = state
}

func (dev *ButtonsDevice) done() {
	dev.i2c.Close()
}

func (dev *ButtonsDevice) work() (bool, error) {
	if time.Since(dev.lastRead).Milliseconds() < ReadButtonsEveryMs {
		//logs.Trace("buttons: too early")
		return false, nil
	}
	dev.lastRead = time.Now()

	var buffer [256]byte
	var size int
	var b byte

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

			var state bool

			if (b & 0x80) == 0x80 {
				state = true
				b = b & 0x7F
			} else {
				state = false
			}
			button := int(b) + 1 // 1..

			if dev.lastState[button] == state {
				// Firmware or hardware error
				logs.Warnf("buttons: ignoring invalid button %i = %v", button, state)
				continue
			}
			dev.lastState[button] = state

			dev.events <- ButtonEvent{
				Button: dev.btnOffset + button, // 1..MaxButtons
				State:  state,
			}
		}
	}

	// Lights
	size = 0

	dev.mu.RLock()

	for x := 0; x < MaxButtons; x++ {
		if dev.lightState[x] != dev.lightStateLast[x] {
			b := byte(x)

			if dev.lightState[x] {
				b |= 0x80
			}

			buffer[size] = b
			size++

			dev.lightStateLast[x] = dev.lightState[x]

			if size >= ButtonsMaxSendBytesPerLoop {
				break
			}
		}
	}

	dev.mu.RUnlock()

	if size > 0 {
		err := dev.i2c.Write(buffer[:], size)
		if err != nil {
			return true, fmt.Errorf("%s (%d bytes)", err, size)
		}
	}

	return true, nil
}
