package devices

import (
	"strconv"
	"time"

	"github.com/marcozaccari/lunatic-midi/config"
	"github.com/marcozaccari/lunatic-midi/devices/hardware"
	"github.com/marcozaccari/lunatic-midi/events"
	"github.com/modulo-srl/sparalog/logs"
)

type Scheduler struct {
	devices []deviceInt
	tasks   []deviceInt

	leds *LedStripDevice

	chans events.Channels

	sleepLatency_us int

	stopChan chan struct{}
}

type deviceInt interface {
	Work() error
	GetType() DeviceType
	Done()
	String() string
}

func NewScheduler(cfg config.Devices, velocityPath string, chans events.Channels) (*Scheduler, error) {
	s := &Scheduler{
		devices: make([]deviceInt, 0, 8),
		tasks:   make([]deviceInt, 0, 16),
		chans:   chans,
	}

	err := s.parseDevices(cfg, velocityPath)
	if err != nil {
		return nil, err
	}

	s.calcSleepLatency()
	s.calcTasksList()

	return s, nil
}

func (s *Scheduler) Stop() {
	s.stopChan <- struct{}{}

	for _, dev := range s.devices {
		dev.Done()
	}
}

const (
	// Time window for every task
	TimeSlices_us = 500

	KeyboardPriority = 500  // 300us lag (velocity on,off)
	ButtonsPriority  = 7000 // 7ms antibounce lag
	AdcPriority      = 500
	LedStripPriority = 2500 // needs a delay of 2.5ms after every update
)

func (s *Scheduler) Work() {
	s.stopChan = make(chan struct{})
	defer close(s.stopChan)

	for {
		select {
		case <-s.stopChan:
			return
		default:
			for _, dev := range s.devices {
				hardware.DebugLedOn()

				start := time.Now()

				//logs.Tracef("get device %s", dev)
				err := dev.Work()
				if err != nil {
					logs.Errorf("device %s error: %s", dev, err)
				}

				// Measure elapsed time of current task
				// and calculate remaining waiting time
				// in order to complete current time window.
				duration := time.Since(start)
				remainder_us := TimeSlices_us - int(duration.Microseconds()) - s.sleepLatency_us

				//logs.Tracef("need to sleep %d us", remainder_us)

				if remainder_us > 0 {
					hardware.DebugLedOff()
					time.Sleep(time.Microsecond * time.Duration(remainder_us))
				}

				hardware.DebugLedOn()
				hardware.DebugLedOff()
			}
		}
	}
}

// TODO gestire bene le priorità
func (s *Scheduler) calcTasksList() {
	for _, dev := range s.devices {
		if dev.GetType() == DeviceKeyboard {
			s.tasks = append(s.tasks, dev)
		}
	}

	for _, dev := range s.devices {
		if dev.GetType() != DeviceKeyboard {
			s.tasks = append(s.tasks, dev)
		}
	}

	desc := "Scheduler tasks: "
	for _, task := range s.tasks {
		desc += task.String() + " "
	}
	logs.Info(desc)
}

func (s *Scheduler) calcSleepLatency() {
	tests := 100

	start := time.Now()

	for k := 0; k < tests; k++ {
		time.Sleep(time.Microsecond * 1000)
	}

	duration := time.Since(start)
	elapsedUs := int(duration.Microseconds())

	s.sleepLatency_us = (elapsedUs - tests*1000) / tests

	logs.Infof("usleep() latency = %dus", s.sleepLatency_us)
}

func (s *Scheduler) parseDevices(cfg config.Devices, velocityPath string) error {
	for i, cfgKeyb := range cfg.Keyboards {
		addr, err := strconv.ParseInt(cfgKeyb.I2C, 0, 0)
		if err != nil {
			return err
		}

		keyb, err := NewKeyboard(byte(addr), cfgKeyb.Offset, i, s.chans.Keyboard)
		if err != nil {
			return err
		}

		err = keyb.LoadVelocity(velocityPath + "/" + cfgKeyb.Velocity + ".dat")
		if err != nil {
			return err
		}

		s.devices = append(s.devices, keyb)

		logs.Infof("Keyboard initialized (%s)", cfgKeyb.I2C)
	}

	if cfg.Buttons.I2C != "" {
		addr, err := strconv.ParseInt(cfg.Buttons.I2C, 0, 0)
		if err != nil {
			return err
		}

		buttons, err := NewButtons(byte(addr), s.chans.Buttons)
		if err != nil {
			return err
		}

		s.devices = append(s.devices, buttons)

		logs.Infof("Buttons initialized (%s)", cfg.Buttons.I2C)
	}

	if cfg.Analog.I2C != "" {
		addr, err := strconv.ParseInt(cfg.Analog.I2C, 0, 0)
		if err != nil {
			return err
		}

		ana, err := NewAnalog(byte(addr), s.chans.Analog)
		if err != nil {
			return err
		}

		s.devices = append(s.devices, ana)

		logs.Infof("Analog initialized (%s)", cfg.Analog.I2C)
	}

	if cfg.LedStrip.I2C != "" {
		addr, err := strconv.ParseInt(cfg.LedStrip.I2C, 0, 0)
		if err != nil {
			return err
		}

		leds, err := NewLedStrip(byte(addr))
		if err != nil {
			return err
		}

		s.devices = append(s.devices, leds)
		s.leds = leds

		logs.Infof("LedStrip initialized (%s)", cfg.LedStrip.I2C)
	}

	return nil
}

func (s *Scheduler) GetLedStrip() *LedStripDevice {
	return s.leds
}
