package example

import (
	"github.com/marcozaccari/lunatic-midi/devices"
	"github.com/marcozaccari/lunatic-midi/events"
	"github.com/modulo-srl/sparalog/logs"
)

type Program struct {
	chans events.Channels

	midi MIDI

	leds *devices.LedStripDevice

	stopChan chan struct{}
}

type MIDI interface {
	SendKey(ch byte, key byte, state bool, vel byte)
	SendCtrlChange(ch byte, ctrl byte, value byte)
}

func NewProgram(chans events.Channels, leds *devices.LedStripDevice, midi MIDI) *Program {
	p := Program{
		leds:  leds,
		chans: chans,
		midi:  midi,
	}

	return &p
}

func (p *Program) Work() {
	p.stopChan = make(chan struct{})
	defer close(p.stopChan)

	for {
		select {
		case ev := <-p.chans.Keyboard:
			logs.Tracef("keyboard event: %v", ev)

			p.midi.SendKey(0, byte(ev.Key), ev.State, ev.Velocity)

			if ev.State {
				p.leds.Set(ev.Key, devices.LedRed)
			} else {
				p.leds.Set(ev.Key, devices.LedOff)
			}

		case ev := <-p.chans.Analog:
			logs.Tracef("analog event: %v", ev)

			p.midi.SendCtrlChange(0, byte(ev.Channel), byte(ev.Value))

		case ev := <-p.chans.MIDI:
			logs.Tracef("midi event: %v", ev)

			switch {
			case ev.NoteOn > 0:
				p.leds.Set(ev.NoteOn, devices.LedBlue)
			case ev.NoteOff > 0:
				p.leds.Set(ev.NoteOff, devices.LedBlue)
			}

		case ev := <-p.chans.Buttons:
			logs.Tracef("buttons event: %v", ev)

		case <-p.stopChan:
			return
		}
	}

}

func (p *Program) Stop() {
	p.stopChan <- struct{}{}
	<-p.stopChan
}
