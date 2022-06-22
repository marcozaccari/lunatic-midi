package linear_keyboard

import (
	"time"

	"github.com/marcozaccari/lunatic-midi/devices"
)

func (p *Program) getSplit() int {
	p.leds.Blink(devices.LedRed, 2, time.Millisecond*100, time.Millisecond*50)

	select {
	case ev := <-p.chans.Keyboard:
		return ev.Key

	case <-p.chans.Buttons:
		break // cancel

	case <-p.chans.Analog:
		// discard

	case <-time.After(time.Second * 10):
		break // timeout
	}

	return 0
}
