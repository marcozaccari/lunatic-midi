package devices

import (
	"strconv"
	"time"

	"github.com/marcozaccari/lunatic-midi/config"
	"github.com/marcozaccari/lunatic-midi/devices/hardware"
	"github.com/modulo-srl/sparalog/logs"
)

type Scheduler struct {
	devices []deviceInt
	tasks   []deviceInt

	sleepLatency_us int
}

type deviceInt interface {
	Work() error
	GetType() DeviceType
	Done()
	String() string
}

func NewScheduler(cfg config.Devices) (*Scheduler, error) {
	s := &Scheduler{
		devices: make([]deviceInt, 0, 8),
		tasks:   make([]deviceInt, 0, 16),
	}

	err := s.parseDevices(cfg)
	if err != nil {
		return nil, err
	}

	s.calcSleepLatency()
	s.calcTasksList()

	return s, nil
}

func (s *Scheduler) Done() {
	for _, dev := range s.devices {
		dev.Done()
	}
}

const (
	// Time window for every task
	TimeSlices_us = 500
)

/*enum tasks {
	KEYBOARD_TASK,  // 300us lag (velocity on,off)
	BUTTONS_TASK,  // 7ms antibounce lag
	ADC_TASK,
	LED_MONITOR_TASK  // needs a delay of 2.5ms after every update
};*/

func (s *Scheduler) Work() {
	go func() {
		for _, dev := range s.devices {
			hardware.DebugLedOn()

			start := time.Now()

			// TODO get events
			dev.Work()

			// Measure elapsed time of current task
			// and calculate remaining waiting time
			// in order to complete current time window.
			duration := time.Since(start)
			remainder_us := TimeSlices_us - int(duration.Microseconds()) - s.sleepLatency_us

			if remainder_us > 0 {
				hardware.DebugLedOff()
				time.Sleep(time.Microsecond * time.Duration(remainder_us))
			}

			hardware.DebugLedOn()
			hardware.DebugLedOff()
		}
	}()
}

// TODO frequenza maggiore tastiere, timeout giusto per leds
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

func (s *Scheduler) parseDevices(cfg config.Devices) error {
	for _, cfgKeyb := range cfg.Keyboards {
		addr, err := strconv.ParseInt(cfgKeyb.I2C, 0, 0)
		if err != nil {
			return err
		}

		keyb, err := NewKeyboard(byte(addr), cfgKeyb.Offset)
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

		buttons, err := NewButtons(byte(addr))
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

		ana, err := NewAnalog(byte(addr))
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

		logs.Infof("LedStrip initialized (%s)", cfg.LedStrip.I2C)
	}

	return nil
}
