package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
	"time"

	"github.com/marcozaccari/lunatic-midi/devices/hardware"
)

var i2c hardware.I2C

const (
	colRed   = "\033[31m"
	colReset = "\033[0m"
)

var adc *hardware.ADS1115

func work() {
	v, err := adc.ReadSample()
	if err != nil {
		fmt.Printf(colRed+"ERROR reading sample: %s\n"+colReset, err)
		return
	}

	fmt.Printf("ADC 0x%02X %d\n", v, v)
}

func main() {
	fI2CAddr := flag.String("i2c-address", "", "Set I2C address (ex: 0x30)")
	flag.Parse()

	if *fI2CAddr == "" {
		fmt.Println("ERROR: i2c-address params not set")
		flag.Usage()
		os.Exit(1)
	}
	addr, err := strconv.ParseUint(*fI2CAddr, 0, 32)
	if err != nil {
		panic(err)
	}

	adc, err = hardware.NewADS1115(byte(addr))
	if err != nil {
		panic(err)
	}

	cfg := hardware.AnalogConfig{
		Channel:            1,
		Gain:               hardware.GAIN_4V,
		Speed:              hardware.SPS_860,
		ConversionMode:     hardware.CONV_ONESHOT,
		Comparator:         hardware.COMP_TRADITIONAL,
		ComparatorPolarity: hardware.COMP_POL_HIGH,
		ComparatorLatching: hardware.COMP_LAT_OFF,
		ComparatorQueue:    hardware.COMP_QUEUE_OFF,
	}
	adc.WriteConfig(cfg)

	// Call every 10 ms
	ticker := time.NewTicker(10 * time.Millisecond)
	defer ticker.Stop()

	for range ticker.C {
		work()
	}
}
