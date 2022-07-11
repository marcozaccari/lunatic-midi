package devices

import (
	"time"

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
	Device

	channelsType [AnalogChannels]ChannelType

	events events.Channel[events.Analog]

	lastValues        [AnalogChannels]int
	continousSampling bool

	curChannel int

	lastRead time.Time
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

	err = dev.setChannel(dev.curChannel)
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
	if time.Since(dev.lastRead).Microseconds() < ReadADCEveryUs {
		logs.Trace("adc: too early")
		return nil
	}
	dev.lastRead = time.Now()

	ui16, err := dev.readSample(dev.curChannel)
	if err != nil {
		return err
	}

	if dev.channelsType[dev.curChannel] == AnalogChannelRibbon {
		ui16 >>= 1
		if ui16 > 500 {
			ui16 = 0
		}
	}

	if dev.lastValues[dev.curChannel] != int(ui16) {
		if dev.lastValues[dev.curChannel] >= 0 {
			dev.events <- events.Analog{
				Channel: dev.curChannel,
				Value:   int(ui16),
			}
		}
		dev.lastValues[dev.curChannel] = int(ui16)
	}

	dev.curChannel = (dev.curChannel + 1) % AnalogChannels
	dev.setChannel(dev.curChannel)

	return nil
}

const (
	ADS_realBits = 12
)

func (dev *AnalogDevice) setChannel(channel int) error {
	/*err := dev.reset()
	if err != nil {
		return err
	}*/

	var cfg analogConfig

	if dev.channelsType[channel] == AnalogChannelSlider {
		// Slider
		// TODO
		cfg = analogConfig{
			Channel:            ConfigChannels[channel],
			Gain:               GAIN_4V,
			Speed:              SPS_860,
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
			Speed:              SPS_860,
			ConversionMode:     CONV_ONESHOT,
			Comparator:         COMP_TRADITIONAL,
			ComparatorPolarity: COMP_POL_HIGH,
			ComparatorLatching: COMP_LAT_OFF,
			ComparatorQueue:    COMP_QUEUE_OFF,
		}
	}

	return dev.writeConfig(cfg)
}

func (dev *AnalogDevice) readSample(channel int) (uint16, error) {
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

	val := int16(buffer[0])<<8 | int16(buffer[1])
	if val < 0 {
		val = 0
	}

	return uint16(uint16(val) >> (16 - ADS_realBits)), nil
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
	SPS_128 = 0x80 // 7,82ms
	SPS_250 = 0xA0 // 4ms
	SPS_475 = 0xC0 // 2,11ms
	SPS_860 = 0xE0 // 1,17ms
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
