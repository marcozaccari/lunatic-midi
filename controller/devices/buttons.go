package devices

import (
	"fmt"
	"sync"
	"time"

	"github.com/marcozaccari/lunatic-midi/events"
	"github.com/modulo-srl/sparalog/logs"
)

const (
	MaxButtons       = 128
	MaxButtonsEvents = 32

	ButtonsMaxSendBytesPerLoop = 16 // 16*8*2.5 = 320us

	ReadButtonsEveryMs = 10 // 7ms antibounce lag
)

type ButtonsDevice struct {
	Device

	lastState [MaxButtons]bool

	lastRead time.Time

	events events.Channel[events.Buttons]

	mu sync.RWMutex
	lightState,

	lightStateLast [MaxButtons]bool
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
			} else {
				event.State = false
			}
			event.Button = int(b) + 1 // 1..MaxButtons

			if dev.lastState[event.Button] == event.State {
				// Firmware or hardware error
				logs.Warnf("buttons: ignoring invalid %v", event)
				continue
			}
			dev.lastState[event.Button] = event.State

			dev.events <- event
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

func (dev *ButtonsDevice) SetLight(index int, state bool) {
	dev.mu.Lock()
	defer dev.mu.Unlock()

	if index < 0 || index >= MaxButtons {
		return
	}

	dev.lightState[index] = state
}
