package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"os/signal"
	"syscall"

	"github.com/marcozaccari/lunatic-midi/devices"
	"github.com/modulo-srl/sparalog/logs"
)

func usage() {
	println("Usage:")
	println("  example <flags>")
	println()

	flag.PrintDefaults()
	println()
}

func main() {
	var err error

	flag.Usage = usage

	fMidiPort := flag.String("midiport", "", "MIDI port to use")
	fShowPorts := flag.Bool("showports", false, "Show system MIDI ports and exit")
	flag.Parse()

	if *fShowPorts {
		devices.PrintMidiPorts()
		return
	}

	if *fMidiPort == "" {
		fmt.Println("No MIDI port specified")
		usage()
		return
	}

	// Init MIDI port.
	portName := *fMidiPort

	midi, err = devices.NewMidi(&portName)
	if err != nil {
		log.Panicf("initializing MIDI port \"%s\": %s", portName, err)
	}
	fmt.Printf("Opened MIDI port \"%s\"", portName)

	// Keyboard device.
	keyb, err := devices.NewKeyboard(0x00, 0)
	if err != nil {
		panic(err)
	}

	err = keyb.LoadVelocity("hard1")
	if err != nil {
		panic(err)
	}

	// Buttons device.
	buttons, err = devices.NewButtons(0x00, 0)
	if err != nil {
		panic(err)
	}

	// Analog device.
	_, err = devices.NewAnalog(0x00, 0)
	if err != nil {
		panic(err)
	}

	// Leds device.
	leds, err = devices.NewLedStrip(0x00, 0)
	if err != nil {
		panic(err)
	}

	// Start.
	events = devices.Start()
	go work()

	sigc := make(chan os.Signal, 1)
	signal.Notify(sigc,
		syscall.SIGINT,
		syscall.SIGTERM,
		syscall.SIGQUIT,
	)

	sig := <-sigc
	fmt.Printf("terminating by signal \"%v\"", sig)

	devices.Stop()
	stopChan <- struct{}{}
}

var events devices.Events
var midi *devices.MidiDevice
var leds *devices.LedStripDevice
var buttons *devices.ButtonsDevice
var stopChan chan struct{}

func work() {
	stopChan = make(chan struct{})
	defer close(stopChan)

	for {
		select {
		case ev := <-events.Keyboard:
			logs.Tracef("keyboard event: %v", ev)

			midi.SendKey(0, byte(ev.Key), ev.State, ev.Velocity)

			if ev.State {
				leds.Set(ev.Key, devices.LedRed)
			} else {
				leds.Set(ev.Key, devices.LedOff)
			}

		case ev := <-events.Analog:
			logs.Tracef("analog event: %v", ev)

			midi.SendCtrlChange(0, byte(ev.Channel), byte(ev.Value))

		case ev := <-events.Midi:
			logs.Tracef("midi event: %v", ev)

			switch {
			case ev.NoteOn > 0:
				leds.Set(ev.NoteOn, devices.LedBlue)
			case ev.NoteOff > 0:
				leds.Set(ev.NoteOff, devices.LedBlue)
			}

		case ev := <-events.Buttons:
			logs.Tracef("buttons event: %v", ev)
			buttons.SetLight(ev.Button, ev.State)

		case <-stopChan:
			return
		}
	}
}
