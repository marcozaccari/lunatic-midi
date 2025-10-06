package devices

// I2C @ 320KHz: ~46us per bytes, 300/370us reading single note (~100 for query and ~200 to get 2 bytes of data)

import (
	"embed"
	"errors"
	"fmt"
	"io"
	"sync"
	"time"

	"github.com/marcozaccari/lunatic-midi/devices/hardware"
	"github.com/modulo-srl/sparalog/logs"
)

const (
	MaxKeyboardKeys   = 128
	MaxKeyboardEvents = 64

	ReadKeyboardEveryUs = 400 // 300us lag (velocity on+off)
)

type KeyboardDevice struct {
	i2c hardware.I2C

	velocityMu     sync.RWMutex
	velocityLookup []byte

	lastRead time.Time

	lastState [MaxKeyboardKeys]bool
	lastKey   KeyboardEvent

	keyOffset int
	events    KeyboardEvents
}

func NewKeyboard(i2cAddr byte, KeyOffset int) (*KeyboardDevice, error) {
	dev := &KeyboardDevice{
		keyOffset:      KeyOffset,
		velocityLookup: make([]byte, 128),
	}

	err := dev.i2c.Open(i2cAddr)
	if err != nil {
		return nil, err
	}

	// disable velocity on note-off
	buffer := [1]byte{0x80}
	err = dev.i2c.Write(buffer[:], 1)
	if err != nil {
		return nil, err
	}

	dev.resetVelocity()

	dev.lastKey.Key = -1

	dev.events = registerKeyboard(dev)

	return dev, nil
}

func (dev *KeyboardDevice) String() string {
	return fmt.Sprintf("keyboard(0x%x)", dev.i2c.Address)
}

//go:generate sh -c "cp ../bin/velocity/*.dat velocity"
//go:embed velocity
var velocityFiles embed.FS

func (dev *KeyboardDevice) LoadVelocity(table string) error {
	dev.velocityMu.Lock()
	defer dev.velocityMu.Unlock()

	dev.resetVelocity()

	if table == "" {
		logs.Info("Reset velocity table")
		return nil
	}

	logs.Info("Load velocity table " + table)

	f, err := velocityFiles.Open(table + ".dat")
	if err != nil {
		return err
	}

	defer f.Close()

	n, err := io.ReadAtLeast(f, dev.velocityLookup, 128)
	if err != nil {
		return err
	}

	if n != 128 {
		return errors.New("invalid velocity file")
	}

	for k := 0; k < 128; k++ {
		if dev.velocityLookup[k] == 0 {
			dev.velocityLookup[k] = 1
		} else if dev.velocityLookup[k] == 128 {
			dev.velocityLookup[k] = 127
		}
	}

	return nil
}

func (dev *KeyboardDevice) resetVelocity() {
	for i := 0; i < 128; i++ {
		dev.velocityLookup[i] = byte(i)
	}
}

func (dev *KeyboardDevice) done() {
	dev.i2c.Close()
}

func (dev *KeyboardDevice) work() (bool, error) {
	if time.Since(dev.lastRead).Microseconds() < ReadKeyboardEveryUs {
		//logs.Trace("keyboard: too early")
		return false, nil
	}
	dev.lastRead = time.Now()

	var buffer [256]byte
	var size int

	err := dev.i2c.Read(buffer[:], 1)
	if err != nil {
		return true, err
	}

	size = int(buffer[0])
	if size == 0 {
		return true, nil
	}

	//logs.Tracef("keyboard received %d bytes", size)

	err = dev.i2c.Read(buffer[:], size+1)
	if err != nil {
		return true, err
	}

	for k := 1; k < size+1; k++ {
		if buffer[k] != 0xFF {
			dev.parse(buffer[k])
		}
	}

	return true, nil
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
		key = int(keyAbs)

		dev.lastKey.Key = key + 1 // 1..

		if keyOn {
			dev.lastKey.State = true
		} else {
			dev.lastKey.State = false
		}

	} else {
		velocity = 127 - b + 1
		if velocity > 127 {
			velocity = 127
		}

		dev.velocityMu.RLock()
		velocity = dev.velocityLookup[velocity]
		dev.velocityMu.RUnlock()

		dev.lastKey.Velocity = velocity

		if dev.lastState[dev.lastKey.Key] == dev.lastKey.State {
			// Firmware or hardware error
			logs.Warningf("keyboard: ignoring invalid %v", dev.lastKey)
			return
		}
		dev.lastState[dev.lastKey.Key] = dev.lastKey.State

		dev.events <- KeyboardEvent{
			Key:      dev.keyOffset + dev.lastKey.Key, // 1..MaxKeys
			State:    dev.lastKey.State,
			Velocity: dev.lastKey.Velocity,
		}
	}
}
