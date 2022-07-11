package devices

import (
	"time"

	"github.com/marcozaccari/lunatic-midi/events"
)

const (
	AnalogChannels = 4
)

type ChannelType int

const (
	AnalogChannelSlider ChannelType = iota
	AnalogChannelRibbon
)

type AnalogDevice struct {
	Device

	channelsType [AnalogChannels]ChannelType

	events events.Channel[events.Analog]

	lastValues        [AnalogChannels]int
	continousSampling bool
}

func NewAnalog(i2cAddr byte, ch events.Channel[events.Analog]) (*AnalogDevice, error) {
	dev := &AnalogDevice{
		events: ch,
	}

	dev.Device.Type = DeviceAnalog

	err := dev.i2c.Open(i2cAddr)
	if err != nil {
		return nil, err
	}

	for i := 0; i < AnalogChannels; i++ {
		dev.lastValues[i] = -1
	}

	dev.continousSampling = false

	err = dev.reset()
	if err != nil {
		return nil, err
	}

	return dev, nil
}

// Reset device
func (dev *AnalogDevice) reset() error {
	buffer := [1]byte{0x06}
	return dev.i2c.Write(buffer[:], 1)
}

func (dev *AnalogDevice) Done() {
	dev.i2c.Close()
}

func (dev *AnalogDevice) SetChannelType(channel int, ctype ChannelType) {
	dev.channelsType[channel] = ctype
}

func (dev *AnalogDevice) Work() error {
	for channel := 0; channel < AnalogChannels; channel++ {
		i16, err := dev.readSample(channel)
		if err != nil {
			return err
		}

		if dev.lastValues[channel] != int(i16) {
			if dev.lastValues[channel] >= 0 {
				dev.events <- events.Analog{
					Channel: channel,
					Value:   int(i16),
				}
			}
			dev.lastValues[channel] = int(i16)
		}
	}

	return nil
}

const (
	ADS_realBits = 12
)

func (dev *AnalogDevice) readSample(channel int) (int16, error) {
	//time.Sleep(time.Millisecond * 4)
	//return 0, nil

	var err error

	/*err = dev.reset()
	if err != nil {
		return 0, err
	}*/

	var cfg analogConfig

	if dev.channelsType[channel] == AnalogChannelSlider {
		// Slider
		// TODO
		cfg = analogConfig{
			Channel:            ConfigChannels[channel],
			Gain:               GAIN_4V,
			Speed:              SPS_475,
			ConversionMode:     CONV_ONESHOT,
			Comparator:         COMP_TRADITIONAL,
			ComparatorPolarity: COMP_POL_HIGH,
			ComparatorLatching: COMP_LAT_OFF,
			ComparatorQueue:    COMP_QUEUE_OFF,
		}
	} else {
		// Ribbon
		// TODO
		cfg = analogConfig{
			Channel:            ConfigChannels[channel],
			Gain:               GAIN_4V,
			Speed:              SPS_475,
			ConversionMode:     CONV_ONESHOT,
			Comparator:         COMP_TRADITIONAL,
			ComparatorPolarity: COMP_POL_HIGH,
			ComparatorLatching: COMP_LAT_OFF,
			ComparatorQueue:    COMP_QUEUE_OFF,
		}
	}

	err = dev.writeConfig(cfg)
	if err != nil {
		return 0, err
	}

	// TODO
	time.Sleep(time.Millisecond * 4)

	buffer := [2]byte{0, 0}

	if !dev.continousSampling {
		// read from conversion register
		err := dev.i2c.Write(buffer[:], 1)
		if err != nil {
			return 0, err
		}
	}

	err = dev.i2c.Read(buffer[:], 2)
	if err != nil {
		return 0, err
	}

	val := int16(buffer[0])<<8 | int16(buffer[1]&0xF0)
	if val < 0 {
		val = 0
	}

	return val, nil
	//return (val >> (15 - ADS_realBits)), nil
}

type ConfigChannel byte

const (
	// Not differential channels
	CH_AIN0 = 0x40
	CH_AIN1 = 0x50
	CH_AIN2 = 0x60
	CH_AIN3 = 0x70
)

var ConfigChannels [4]ConfigChannel = [4]ConfigChannel{CH_AIN0, CH_AIN1, CH_AIN2, CH_AIN3}

type ConfigGain byte

const (
	GAIN_6V   = 0   // +-6.144V
	GAIN_4V   = 2   // +-4.096V
	GAIN_2V   = 4   // +-2.048V
	GAIN_1V   = 6   // +-1.024V
	GAIN_05V  = 8   // +-0.512V
	GAIN_025V = 0xA // +-0.256V
)

type ConfigSpeed byte

const (
	SPS_8   = 0
	SPS_16  = 0x20
	SPS_32  = 0x40
	SPS_64  = 0x60
	SPS_128 = 0x80
	SPS_250 = 0xA0 // 4ms
	SPS_475 = 0xC0 // 2ms
	SPS_860 = 0xE0
)

type ConfigConversionMode byte

const (
	CONV_CONTINOUS = 0
	CONV_ONESHOT   = 1
)

type ConfigComparatorType byte

const (
	COMP_TRADITIONAL = 0
	COMP_WINDOW      = 0x10
)

type ConfigComparatorPolarity byte

const (
	COMP_POL_LOW  = 0 // Active low
	COMP_POL_HIGH = 8 // Active high
)

type ConfigComparatorLatching byte

const (
	COMP_LAT_OFF = 0 // Non latching comparator
	COMP_LAT_ON  = 4 // Latching comparator
)

type ConfigComparatorQueue byte

const (
	COMP_QUEUE_1   = 0 // Assert after one conversion
	COMP_QUEUE_2   = 1 // Assert after two conversion
	COMP_QUEUE_4   = 2 // Assert after four conversion
	COMP_QUEUE_OFF = 3 // Disable comparator (ALERT/RDY on high impedance)
)

type analogConfig struct {
	Channel            ConfigChannel
	Gain               ConfigGain
	Speed              ConfigSpeed
	ConversionMode     ConfigConversionMode
	Comparator         ConfigComparatorType
	ComparatorPolarity ConfigComparatorPolarity
	ComparatorLatching ConfigComparatorLatching
	ComparatorQueue    ConfigComparatorQueue
}

func (dev *AnalogDevice) writeConfig(config analogConfig) error {
	var buffer [3]byte

	buffer[0] = 1 // write to config register
	buffer[1] = byte(config.Channel) |
		byte(config.Gain) |
		byte(config.ConversionMode)
	buffer[2] = byte(config.Speed) |
		byte(config.Comparator) |
		byte(config.ComparatorPolarity) |
		byte(config.ComparatorLatching) |
		byte(config.ComparatorQueue)

	if config.ConversionMode == CONV_ONESHOT {
		buffer[1] |= 0x80 // start conversion now
	} else {
		dev.continousSampling = true
	}

	err := dev.i2c.Write(buffer[:], 3)
	if err != nil {
		return err
	}

	if dev.continousSampling {
		buffer[0] = 0

		// prepare to read from conversion register
		err := dev.i2c.Write(buffer[:], 1)
		if err != nil {
			return err
		}
	}

	//logs.Tracef("analog: config: %x %x = %x", buffer[1], buffer[2], config)

	return nil
}
