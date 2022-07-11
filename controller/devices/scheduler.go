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

	ticker := time.NewTicker(time.Microsecond * TimeSlices_us)
	defer ticker.Stop()

	curTask := -1

	for {
		select {
		case <-s.stopChan:
			return

		case <-ticker.C:
			hardware.DebugLedOn()

			curTask = (curTask + 1) % len(s.tasks)

			dev := s.tasks[curTask]
			//logs.Tracef("get device %s", dev)

			err := dev.Work()
			if err != nil {
				logs.Errorf("device %s error: %s", dev, err)
			}

			hardware.DebugLedOff()
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

		stype := cfg.Analog.Channel1
		switch stype {
		case "slider":
			ana.SetChannelType(0, AnalogChannelSlider)
		case "ribbon":
			ana.SetChannelType(0, AnalogChannelRibbon)
		}

		stype = cfg.Analog.Channel2
		switch stype {
		case "slider":
			ana.SetChannelType(1, AnalogChannelSlider)
		case "ribbon":
			ana.SetChannelType(1, AnalogChannelRibbon)
		}

		stype = cfg.Analog.Channel3
		switch stype {
		case "slider":
			ana.SetChannelType(2, AnalogChannelSlider)
		case "ribbon":
			ana.SetChannelType(2, AnalogChannelRibbon)
		}

		stype = cfg.Analog.Channel4
		switch stype {
		case "slider":
			ana.SetChannelType(3, AnalogChannelSlider)
		case "ribbon":
			ana.SetChannelType(3, AnalogChannelRibbon)
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
