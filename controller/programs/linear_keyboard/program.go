package linear_keyboard

import (
	"github.com/marcozaccari/lunatic-midi/devices"
	"github.com/marcozaccari/lunatic-midi/events"
)

type Program struct {
	chans events.Channels

	midi MIDI

	leds *devices.LedStripDevice

	splitKey int

	stopChan chan struct{}
}

type MIDI interface {
	SendKey(ch byte, key byte, state bool, vel byte)
	SendPitchBend(ch byte, vel int16)
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
			ch := 0
			if p.splitKey > 0 && ev.Key >= p.splitKey {
				ch = 1
			}
			p.midi.SendKey(byte(ch), byte(ev.Key), ev.State, ev.Velocity)

		case ev := <-p.chans.Analog:
			p.midi.SendCtrlChange(0, byte(ev.Channel), byte(ev.Value))

		case ev := <-p.chans.MIDI:
			ledCh := devices.LedBlue + devices.LedColor(ev.Ch)
			if ledCh > devices.LedWhite {
				ledCh = devices.LedWhite
			}

			switch {
			case ev.NoteOn > 0:
				p.leds.Set(ev.NoteOn, ledCh)
			case ev.NoteOff > 0:
				p.leds.Set(ev.NoteOff, ledCh)
			}

		case ev := <-p.chans.Buttons:
			if ev.Button == 1 && ev.State {
				// Split
				p.splitKey = p.getSplit()
			}

		case <-p.stopChan:
			return
		}
	}

}

func (p *Program) Stop() {
	p.stopChan <- struct{}{}
	<-p.stopChan
}
