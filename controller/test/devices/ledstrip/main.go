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

	LedCount          = 60
	LedsWritesDelayMs = 7 // needs a delay of 3ms after every update
)

type LedColor byte

const (
	LedOff LedColor = iota
	LedRed
	LedGreen
	LedYellow
	LedBlue
	LedViolet
	LedCyan
	LedWhite
)

var framebuffer, framebufferLast [LedCount]LedColor

func Fill(color LedColor) {
	for x := 0; x < LedCount; x++ {
		framebuffer[x] = color
	}
}

func Set(index int, color LedColor) {
	if index < 0 || index >= LedCount {
		return
	}

	framebuffer[index] = color
}

func checkFramebufferUpdates() error {
	var buffer [256]byte
	buffLen := 0

	buffer[buffLen] = 0x40
	buffLen++

	x := 0
	for i := 0; i < LedCount/2; i++ {
		/*if framebuffer[x] != framebufferLast[x] {
			buffer[buffLen] = byte(x) | 0x40
			buffLen++
			buffer[buffLen] = byte(framebuffer[x])
			buffLen++

			framebufferLast[x] = framebuffer[x]
		}*/

		buffer[buffLen] = byte(framebuffer[x])<<3 + byte(framebuffer[x+1])
		buffLen++
		x += 2
	}

	if buffLen == 0 {
		return nil
	}

	// add repaint command
	buffer[buffLen] = 0xC1
	buffLen++

	//fmt.Printf("need repaint: %s\n", arr2str(buffer[:buffLen]))

	err := i2c.Write(buffer[:], buffLen)

	if err != nil {
		return fmt.Errorf("%s (%d bytes)", err, buffLen)
	}

	return nil
}

func arr2str(bb []byte) string {
	s := "[ "

	for _, b := range bb {
		s += fmt.Sprintf("0x%02X ", b)
	}

	s += "]"
	return s
}

var counterX = 0
var colors [14]LedColor = [14]LedColor{1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0}
var color LedColor = 1
var colorIdx int = 0

func work() {
	err := checkFramebufferUpdates()
	if err != nil {
		fmt.Printf(colRed+"ERROR protocol: %s\n"+colReset, err)
	}

	Set(counterX, color)

	if counterX < LedCount {
		counterX++
		return
	}
	counterX = 0

	colorIdx++
	if colorIdx == 14 {
		colorIdx = 0
	}

	color = colors[colorIdx]
}

// blocking
func testFill() {
	for {
		bb := []byte{0xC8 + 1, 0xC1}
		i2c.Write(bb[:], 2)

		time.Sleep(time.Second)

		bb = []byte{0xC8 + 2, 0xC1}
		i2c.Write(bb[:], 2)

		time.Sleep(time.Second)

		bb = []byte{0xC8 + 4, 0xC1}
		i2c.Write(bb[:], 2)

		time.Sleep(time.Second)

		bb = []byte{0xC8, 0xC1}
		i2c.Write(bb[:], 2)
	}
}

func open(addr byte) {
	fmt.Printf("Opening 0x%02X ...\n", addr)

	err := i2c.Open(addr)
	if err != nil {
		panic(err)
	}

	var bb []byte = []byte{0xFF} // reset
	err = i2c.Write(bb[:], 1)
	if err != nil {
		panic(err)
	}

	time.Sleep(10 * time.Millisecond)

	var buffer [256]byte
	for k := 0; k < 1; k++ {
		buffer[k] = 0x55
	}

	err = i2c.Read(buffer[:], 1)
	if err != nil {
		fmt.Printf(colRed+"ERROR I2C reading len: %s\n"+colReset, err)
		return
	}

	size := int(buffer[0])
	if size == 0 {
		fmt.Println(colRed + "ERROR: no firmware version received\n" + colReset)
		return
	}

	err = i2c.Read(buffer[:], size+1)
	if err != nil {
		fmt.Printf(colRed+"ERROR I2C reading data: %s\n"+colReset, err)
		return
	}
	buffer[size] = 0

	version := string(buffer[1:])
	fmt.Printf("Firmware version: %s\n", version)
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

	open(byte(addr))

	// Fill
	//testFill()

	// One leds at time
	for x := 0; x < LedCount; x++ {
		framebuffer[x] = LedOff
		framebufferLast[x] = LedOff
	}

	// Call every LedsWritesDelayMs ms
	ticker := time.NewTicker(LedsWritesDelayMs * time.Millisecond)
	defer ticker.Stop()

	for range ticker.C {
		work()
	}
}
