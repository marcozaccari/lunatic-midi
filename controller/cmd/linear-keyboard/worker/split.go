package worker

import (
	"time"

	"github.com/marcozaccari/lunatic-midi/devices"
)

func (w *Worker) getSplit() {
	for {
		select {
		case ev := <-w.events.Keyboard:
			if ev.Key == 1 {
				// No split
				w.splitKey = 0
			} else {
				w.splitKey = ev.Key

				w.devs.Leds.Blink(w.splitKey, devices.LedRed, 2, time.Millisecond*100, time.Millisecond*100)
				w.devs.Leds.Blink(w.splitKey, devices.LedViolet, 1, time.Millisecond*500, 0)
			}
			return

		case <-w.events.Buttons:
			return // cancel

		case <-w.events.Analog:
			// discard

		case <-time.After(time.Second * 10):
			return // timeout
		}
	}
}
