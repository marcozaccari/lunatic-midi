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
	SendKey(ch int, key int, state bool, vel byte)
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
			p.midi.SendKey(ch, ev.Key, ev.State, ev.Velocity)

		// TODO
		//case ev := <-m.channels.Analog:

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
