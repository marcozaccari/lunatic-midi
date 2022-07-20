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

	leds    *LedStripDevice
	buttons *ButtonsDevice

	chans events.Channels

	stopChan chan struct{}
}

type deviceInt interface {
	Work() (bool, error)
	GetType() DeviceType
	Done()
	String() string
}

func NewScheduler(cfg config.Devices, velocityPath string, chans events.Channels) (*Scheduler, error) {
	s := &Scheduler{
		devices: make([]deviceInt, 0, 8),
		chans:   chans,
	}

	err := s.parseDevices(cfg, velocityPath)
	if err != nil {
		return nil, err
	}

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
)

func (s *Scheduler) Work() {
	s.stopChan = make(chan struct{})
	defer close(s.stopChan)

	ticker := time.NewTicker(time.Microsecond * TimeSlices_us)
	defer ticker.Stop()

	curDev := -1

	for {
		select {
		case <-s.stopChan:
			return

		case <-ticker.C:
			hardware.DebugLedOn()

			oldDev := curDev
			for {
				curDev = (curDev + 1) % len(s.devices)

				dev := s.devices[curDev]
				//logs.Tracef("get device %s", dev)

				worked, err := dev.Work()
				if err != nil {
					logs.Errorf("device %s error: %s", dev, err)
				}

				if worked || oldDev == curDev {
					break
				}
			}

			hardware.DebugLedOff()
		}
	}
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
		s.buttons = buttons

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

		for i, cfgAna := range cfg.Analog.Channels {
			if cfgAna.Type == "" || cfgAna.Bits == 0 {
				continue
			}

			var typ ChannelType
			switch cfgAna.Type {
			case "slider":
				typ = AnalogChannelSlider
			case "ribbon":
				typ = AnalogChannelRibbon
			}
			ana.SetChannelType(i, typ, cfgAna.Bits, uint(cfgAna.RawMin), uint(cfgAna.RawMax))
		}

		s.devices = append(s.devices, ana)

		logs.Infof("Analog initialized (%s)", cfg.Analog.I2C)
	}

	if cfg.LedStrip.I2C != "" {
		addr, err := strconv.ParseInt(cfg.LedStrip.I2C, 0, 0)
		if err != nil {
			return err
		}

		leds, err := NewLedStrip(byte(addr), cfg.LedStrip.Offset)
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

func (s *Scheduler) GetButtons() *ButtonsDevice {
	return s.buttons
}
