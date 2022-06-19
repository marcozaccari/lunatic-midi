package modes

import "github.com/marcozaccari/lunatic-midi/events"

type Split struct {
	Mode

	stopChan chan struct{}
}

func NewSplit(chans events.Channels) *Split {
	m := Split{}

	m.Mode.channels = chans

	return &m
}

func (m *Split) Work() {
	m.stopChan = make(chan struct{})
	defer close(m.stopChan)

	for {
		select {
		case ev := <-m.channels.Keyboard:

		case ev := <-m.channels.Buttons:
			if ev.Button == 100 && ev.State {
				// Split
				return
			}

		case <-m.stopChan:
			return
		}
	}

}

func (m *Split) Stop() {
	m.stopChan <- struct{}{}
	<-m.stopChan
}
