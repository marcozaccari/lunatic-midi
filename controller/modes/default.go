package modes

import "github.com/marcozaccari/lunatic-midi/events"

type Default struct {
	Mode

	midi MIDI

	stopChan chan struct{}
}

type MIDI interface {
	SendKey(key int, state bool, vel byte)
}

func NewDefault(chans events.Channels, midi MIDI) *Default {
	m := Default{
		midi: midi,
	}
	m.Mode.channels = chans

	return &m
}

func (m *Default) Work() {
	m.stopChan = make(chan struct{})
	defer close(m.stopChan)

	for {
		select {
		case ev := <-m.channels.Keyboard:
			m.midi.SendKey(ev.Key, ev.State, ev.Velocity)

		// TODO
		//case ev := <-m.channels.Analog:

		case ev := <-m.channels.Buttons:
			if ev.Button == 1 && ev.State {
				// Split

			}

		case <-m.stopChan:
			return
		}
	}

}

func (m *Default) Stop() {
	m.stopChan <- struct{}{}
	<-m.stopChan
}
