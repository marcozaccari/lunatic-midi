package linear_keyboard

import (
	"time"

	"github.com/marcozaccari/lunatic-midi/devices"
)

func (p *Program) getSplit() {
	if p.splitKey == 0 {
		p.leds.BlinkAll(devices.LedRed, 2, time.Millisecond*200, time.Millisecond*100)
		p.leds.BlinkAll(devices.LedViolet, 1, time.Millisecond*500, 0)
	} else {
		p.leds.Blink(p.splitKey, devices.LedRed, 2, time.Millisecond*200, time.Millisecond*100)
		p.leds.Blink(p.splitKey, devices.LedViolet, 1, time.Millisecond*500, 0)
	}

	for {
		select {
		case ev := <-p.chans.Keyboard:
			if ev.Key == 1 {
				// No split
				p.splitKey = 0

				p.leds.BlinkAll(devices.LedRed, 2, time.Millisecond*100, time.Millisecond*100)
				p.leds.BlinkAll(devices.LedViolet, 1, time.Millisecond*500, 0)
			} else {
				p.splitKey = ev.Key

				p.leds.Blink(p.splitKey, devices.LedRed, 2, time.Millisecond*100, time.Millisecond*100)
				p.leds.Blink(p.splitKey, devices.LedViolet, 1, time.Millisecond*500, 0)
			}
			return

		case ev := <-p.chans.Buttons:
			if ev.State {
				return // cancel
			}

		case <-p.chans.Analog:
			// discard

		case <-time.After(time.Second * 10):
			return // timeout
		}
	}
}
