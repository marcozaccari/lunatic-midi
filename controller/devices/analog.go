package devices

import (
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
		dev.lastValues[i] = 0
	}

	dev.continousSampling = false

	// TODO
	err = dev.writeConfig(analogConfig{
		Channel:            0,
		Reference:          0,
		Gain:               0,
		Speed:              0,
		ConversionMode:     CONV_CONTINOUS,
		Comparator:         COMP_TRADITIONAL,
		ComparatorPolarity: COMP_POL_HIGH,
		ComparatorLatching: COMP_LAT_OFF,
		ComparatorQueue:    COMP_QUEUE_OFF,
	})
	if err != nil {
		return nil, err
	}

	return dev, nil
}

func (dev *AnalogDevice) Done() {
	dev.i2c.Close()
}

func (dev *AnalogDevice) SetChannelType(channel int, ctype ChannelType) {
	dev.channelsType[channel] = ctype
}

func (dev *AnalogDevice) Work() error {
	var event events.Analog

	for channel := 0; channel < AnalogChannels; channel++ {
		event.Channel = channel

		ui16, err := dev.readSample()
		if err != nil {
			return err
		}

		event.Value = int(ui16)

		if dev.lastValues[event.Channel] != event.Value {
			dev.events <- event
		}
	}

	return nil
}

const (
	ADS_realBits = 12
)

func (dev *AnalogDevice) readSample() (uint16, error) {
	buffer := [2]byte{0, 0}

	if !dev.continousSampling {
		// read from conversion register
		err := dev.i2c.Write(buffer[:], 1)
		if err != nil {
			return 0, err
		}
	}

	err := dev.i2c.Read(buffer[:], 2)
	if err != nil {
		return 0, err
	}

	val := uint16(buffer[0])<<8 | uint16(buffer[1])

	return (val >> (15 - ADS_realBits)), nil
}

type ConfigChannel byte

const (
	CH_AIN0 = 0
	CH_AIN1 = 0x10
	CH_AIN2 = 0x20
	CH_AIN3 = 0x30
)

type ConfigReference byte

const (
	REF_GND  = 0    // not differential
	REF_AIN3 = 0x40 // differential
)

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
	SPS_250 = 0xA0
	SPS_475 = 0xC0
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
	Reference          ConfigReference
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
	buffer[1] = byte(config.Reference) |
		byte(config.Channel) |
		byte(config.Gain) |
		byte(config.Speed) |
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

	return nil
}
