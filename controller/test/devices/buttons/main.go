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

func work() {
	var buffer [256]byte
	var size int

	err := i2c.Read(buffer[:], 1)
	if err != nil {
		fmt.Printf(colRed+"ERROR I2C reading len: %s\n"+colReset, err)
		return
	}

	size = int(buffer[0])
	if size == 0 {
		//fmt.Printf(".")
		return
	}

	//printf("received %d bytes", size)

	err = i2c.Read(buffer[:], size+1)
	if err != nil {
		fmt.Printf(colRed+"ERROR I2C reading data: %s\n"+colReset, err)
		return
	}

	for k := 1; k < size+1; k++ {
		button := buffer[k]

		fmt.Printf("Button %d\n", button)

		switch button {
		case 88:
			bb := [2]byte{56 | 0x80, 1}
			i2c.Write(bb[:], 2)
		case 89:
			bb := [2]byte{57 | 0x80, 2}
			i2c.Write(bb[:], 2)
		case 90:
			bb := [2]byte{58 | 0x80, 3}
			i2c.Write(bb[:], 2)
		case 91:
			bb := [2]byte{59 | 0x80, 1}
			i2c.Write(bb[:], 2)
		case 92:
			bb := [2]byte{60 | 0x80, 2}
			i2c.Write(bb[:], 2)
		case 93:
			bb := [2]byte{61 | 0x80, 3}
			i2c.Write(bb[:], 2)
		case 94:
			bb := [2]byte{62 | 0x80, 1}
			i2c.Write(bb[:], 2)
		case 95:
			bb := [2]byte{63 | 0x80, 2}
			i2c.Write(bb[:], 2)
		}
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

	// Call every 10 ms
	ticker := time.NewTicker(10 * time.Millisecond)
	defer ticker.Stop()

	for range ticker.C {
		work()
	}
}
