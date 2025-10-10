package worker

import (
	"github.com/marcozaccari/lunatic-midi/devices"
	"github.com/modulo-srl/sparalog/logs"
)

type Worker struct {
	events devices.Events

	devs OutputDevices

	splitKey int

	stopChan chan struct{}
}

type OutputDevices struct {
	Midi    *devices.MidiDevice
	Buttons *devices.ButtonsDevice
	Leds    *devices.LedStripDevice
}

const (
	// GEM WS-1 buttons mapping: 89-0 .. 96-7
	ButLight0 = 89
)

func New(devs OutputDevices) *Worker {
	p := Worker{
		devs: devs,
	}

	return &p
}

func (w *Worker) Start(ev devices.Events) {
	w.events = ev

	go w.work()
}

func (w *Worker) Stop() {
	w.stopChan <- struct{}{}
	<-w.stopChan
}

func (w *Worker) work() {
	w.stopChan = make(chan struct{})
	defer close(w.stopChan)

	for {
		select {
		case ev := <-w.events.Keyboard:
			logs.Debugf("keyboard event: %v", ev)

			ch := 0
			col := devices.LedRed
			if w.splitKey > 0 && ev.Key >= w.splitKey {
				ch = 1
				col = devices.LedViolet
			}
			w.devs.Midi.SendKey(byte(ch), byte(ev.Key), ev.State, ev.Velocity)

			if ev.State {
				w.devs.Leds.Set(ev.Key, col)
			} else {
				w.devs.Leds.Set(ev.Key, devices.LedOff)
			}

		case ev := <-w.events.Analog:
			logs.Debugf("analog event: %v", ev)

			w.devs.Midi.SendCtrlChange(0, byte(ev.Channel), byte(ev.Value))

		case ev := <-w.events.Midi:
			logs.Debugf("midi event: %v", ev)

			ledCh := devices.LedBlue + devices.LedColor(ev.Ch)
			if ledCh > devices.LedWhite {
				ledCh = devices.LedWhite
			}

			switch {
			case ev.NoteOn > 0:
				w.devs.Leds.Set(ev.NoteOn, ledCh)
			case ev.NoteOff > 0:
				w.devs.Leds.Set(ev.NoteOff, ledCh)
			}

		case ev := <-w.events.Buttons:
			logs.Debugf("buttons event: %v", ev)

			if ev.Button == ButLight0+7 {
				// Split
				if w.splitKey == 0 {
					w.devs.Buttons.SetLight(7, true)
					w.getSplit()
				} else {
					w.splitKey = 0
				}

				w.devs.Buttons.SetLight(7, w.splitKey > 0)
			}

		case <-w.stopChan:
			return
		}
	}

}
