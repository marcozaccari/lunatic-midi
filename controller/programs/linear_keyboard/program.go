package linear_keyboard

import (
	"github.com/marcozaccari/lunatic-midi/devices"
	"github.com/marcozaccari/lunatic-midi/events"
	"github.com/modulo-srl/sparalog/logs"
)

type Program struct {
	chans events.Channels

	midi MIDI

	leds    *devices.LedStripDevice
	buttons *devices.ButtonsDevice

	splitKey int

	stopChan chan struct{}
}

type MIDI interface {
	SendKey(ch byte, key byte, state bool, vel byte)
	SendPitchBend(ch byte, vel int16)
	SendCtrlChange(ch byte, ctrl byte, value byte)
}

const (
	// GEM WS-1 buttons mapping: 89-0 .. 96-7
	ButLight0 = 89
)

func NewProgram(chans events.Channels, leds *devices.LedStripDevice, btn *devices.ButtonsDevice, midi MIDI) *Program {
	p := Program{
		leds:    leds,
		buttons: btn,
		chans:   chans,
		midi:    midi,
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

			ch := 0
			col := devices.LedRed
			if p.splitKey > 0 && ev.Key >= p.splitKey {
				ch = 1
				col = devices.LedViolet
			}
			p.midi.SendKey(byte(ch), byte(ev.Key), ev.State, ev.Velocity)

			if ev.State {
				p.leds.Set(ev.Key, col)
			} else {
				p.leds.Set(ev.Key, devices.LedOff)
			}

		case ev := <-p.chans.Analog:
			logs.Tracef("analog event: %v", ev)

			p.midi.SendCtrlChange(0, byte(ev.Channel), byte(ev.Value))

		case ev := <-p.chans.MIDI:
			logs.Tracef("midi event: %v", ev)

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
			logs.Tracef("buttons event: %v", ev)

			if ev.Button == ButLight0+7 && ev.State {
				// Split
				if p.splitKey == 0 {
					p.buttons.SetLight(7, true)
					p.getSplit()
				} else {
					p.splitKey = 0
				}

				p.buttons.SetLight(7, p.splitKey > 0)
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
