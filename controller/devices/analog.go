package devices

import (
	"fmt"
	"math"
	"time"

	"github.com/marcozaccari/lunatic-midi/devices/hardware"
	"github.com/marcozaccari/lunatic-midi/events"
	"github.com/modulo-srl/sparalog/logs"
)

const (
	AnalogChannels = 4

	ReadADCEveryUs = 1500 // SPS_860 = 1,2ms + 20us reboot time
)

type ChannelType int

const (
	AnalogChannelSlider ChannelType = iota
	AnalogChannelRibbon
)

type AnalogDevice struct {
	enabled bool

	i2cAddr byte
	adc     *hardware.ADS1115

	channels   [AnalogChannels]analogChannel
	curChannel int

	events events.Channel[events.Analog]

	lastRead time.Time
}

type analogChannel struct {
	enabled bool

	typ ChannelType

	rawMin, rawMax uint
	bits           int

	lastValue int

	ema *EMAfilter
}

func NewAnalog(i2cAddr byte, ch events.Channel[events.Analog]) (*AnalogDevice, error) {
	dev := &AnalogDevice{
		events:  ch,
		i2cAddr: i2cAddr,
	}

	var err error
	dev.adc, err = hardware.NewADS1115(i2cAddr)
	if err != nil {
		return nil, err
	}

	for i := 0; i < AnalogChannels; i++ {
		dev.channels[i].lastValue = -100
	}

	return dev, nil
}

func (dev *AnalogDevice) String() string {
	return fmt.Sprintf("analog(0x%x)", dev.i2cAddr)
}

func (dev *AnalogDevice) Done() {
	dev.adc.Done()
}

func (dev *AnalogDevice) SetChannelType(channel int, ctype ChannelType, bits int, rawMin, rawMax uint) {
	dev.channels[channel] = analogChannel{
		enabled: true,
		typ:     ctype,
		bits:    bits,
		rawMin:  rawMin,
		rawMax:  rawMax,
	}

	var emaBits uint
	if ctype == AnalogChannelRibbon {
		emaBits = 14 // pitch bend 128*128 = 16384 (-8192..+8192)
	} else {
		// Slider, CC 0-127
		emaBits = 7
	}
	dev.channels[channel].ema = NewEMAfilter(emaBits)

	logs.Infof("Set channel %d: %+v", channel, dev.channels[channel])

	dev.enabled = true

	dev.curChannel = channel
	dev.setChannel(dev.curChannel)
}

func (dev *AnalogDevice) Work() (bool, error) {
	if time.Since(dev.lastRead).Microseconds() < ReadADCEveryUs {
		//logs.Trace("adc: too early")
		return false, nil
	}

	defer func() {
		dev.lastRead = time.Now()
	}()

	if !dev.enabled {
		// All channels disabled
		return false, nil
	}

	channel := &dev.channels[dev.curChannel]

	// Read sample from current channel.

	ui16, err := dev.adc.ReadSample()
	if err != nil {
		return true, err
	}

	ui16 >>= (16 - channel.bits)
	sample := int(ui16)

	var value int = int(sample)

	if uint(sample) >= channel.rawMax {
		if channel.typ == AnalogChannelRibbon {
			sample = -1
			value = -1
		} else {
			sample = int(channel.rawMax)
		}
	} else if uint(sample) < channel.rawMin {
		sample = int(channel.rawMin)
	}

	if sample >= 0 {
		// Scale up to 16 bits.
		value = int(math.Round(float64(
			float32(sample-int(channel.rawMin)) * 0xFFFF / float32(channel.rawMax-channel.rawMin))))

		// Filter the noisy input signal using an EMA single-pole filter
		// with the pole in z = 0.96875 = 1 - 1/32 = 1 - 1/(2^5).
		value = int(channel.ema.filter(uint(value)))

		/*
			// 0-127 or 0-512
			var max float32 = 127 // Slider
			if channel.typ == AnalogChannelRibbon {
				max = 512
			}
			value := sample - int(channel.rawMin)
			value = int(math.Round(float64(float32(value) * max / float32(channel.rawMax-channel.rawMin))))
		*/
	}

	if channel.lastValue != value {
		if channel.lastValue >= -1 {
			dev.events <- events.Analog{
				Channel: dev.curChannel,
				Type:    int(channel.typ),
				Value:   value,
				Raw:     sample,
			}
		}
		channel.lastValue = value
	}

	// Select next channel to read.

	dev.curChannel = (dev.curChannel + 1) % AnalogChannels
	// find next enabled channel
	for !dev.channels[dev.curChannel].enabled {
		dev.curChannel = (dev.curChannel + 1) % AnalogChannels
	}
	dev.setChannel(dev.curChannel)

	return true, nil
}

func (dev *AnalogDevice) setChannel(channel int) error {
	cfg := hardware.AnalogConfig{
		Channel:            hardware.ConfigChannels[channel],
		Gain:               hardware.GAIN_4V,
		Speed:              hardware.SPS_860,
		ConversionMode:     hardware.CONV_ONESHOT,
		Comparator:         hardware.COMP_TRADITIONAL,
		ComparatorPolarity: hardware.COMP_POL_HIGH,
		ComparatorLatching: hardware.COMP_LAT_OFF,
		ComparatorQueue:    hardware.COMP_QUEUE_OFF,
	}

	return dev.adc.WriteConfig(cfg)
}
