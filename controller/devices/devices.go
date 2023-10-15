package devices

import (
	"fmt"
	"time"

	"github.com/marcozaccari/lunatic-midi/devices/hardware"
	"github.com/modulo-srl/sparalog/logs"
	"gitlab.com/gomidi/midi/v2"
)

const (
	// Time window for every device's tasks
	DeviceWindow_us = 500
)

type device interface {
	done()
}
type workerDevice interface {
	work() (bool, error)
	done()
}

var devices []device
var workerDevices []workerDevice
var workerStopChan chan struct{}

var eventsChans Events

func init() {
	eventsChans = Events{
		Midi:     make(MidiEvents, 32),
		Keyboard: make(KeyboardEvents, 32),
		Buttons:  make(ButtonsEvents, 32),
		Analog:   make(AnalogEvents, 32),
	}

	devices = make([]device, 0, 8)
}

// Start the internal management of devices and start sending events to the channels.
func Start() Events {
	go work()

	return eventsChans
}

// Stop the internal management of devices and stop sending events to the channels.
func Stop() {
	workerStopChan <- struct{}{}
	<-workerStopChan

	for _, dev := range devices {
		dev.done()
	}
}

func PrintMidiPorts() {
	fmt.Println("MIDI ports:")

	ports := midi.GetOutPorts()

	for _, p := range ports {
		fmt.Printf("%s\n", p)
	}
}

func registerMidi(dev *MidiDevice) MidiEvents {
	devices = append(devices, dev)

	return eventsChans.Midi
}

func registerKeyboard(dev *KeyboardDevice) KeyboardEvents {
	devices = append(devices, dev)
	workerDevices = append(workerDevices, dev)

	return eventsChans.Keyboard
}

func registerAnalog(dev *AnalogDevice) AnalogEvents {
	devices = append(devices, dev)
	workerDevices = append(workerDevices, dev)

	return eventsChans.Analog
}

func registerButtons(dev *ButtonsDevice) ButtonsEvents {
	devices = append(devices, dev)
	workerDevices = append(workerDevices, dev)

	return eventsChans.Buttons
}

func registerLedStrip(dev *LedStripDevice) {
	devices = append(devices, dev)
	workerDevices = append(workerDevices, dev)
}

func work() {
	workerStopChan = make(chan struct{})
	defer close(workerStopChan)

	ticker := time.NewTicker(time.Microsecond * DeviceWindow_us)
	defer ticker.Stop()

	curDev := -1

	for {
		select {
		case <-ticker.C:
			hardware.DebugLedOn()

			// Looking for a device capable of carrying out his work.
			// There are in fact devices that return immediately
			// if called too quickly, in this case will be invoked
			// the next available device.

			oldDev := curDev
			for {
				curDev = (curDev + 1) % len(workerDevices)

				dev := workerDevices[curDev]
				//logs.Tracef("get device %s", dev)

				worked, err := dev.work()
				if err != nil {
					logs.Errorf("device %s error: %s", dev, err)
				}

				if worked || oldDev == curDev {
					// Work done or no devices available for this tick.
					break
				}
			}

			hardware.DebugLedOff()

		case <-workerStopChan:
			workerStopChan <- struct{}{}
			return
		}
	}
}
