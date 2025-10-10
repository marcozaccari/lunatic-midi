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

var keys [64]byte

func printKeys(buffer [256]byte, size int) {
	s := ""

	for i := 0; i < 64; i++ {
		if keys[i] > 0 {
			s += fmt.Sprintf("[KEY %d (%d)] ", i, keys[i])
		}
	}

	if s == "" {
		s = "- silence -"
	}

	s += "  | "

	for i := 0; i < size; i++ {
		s += fmt.Sprintf("0x%X ", buffer[i+1])
	}

	fmt.Println(s)
}

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
		println(err)
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

	var b byte
	var keyAbs byte
	var key int
	var keyOn bool
	var velocity byte

	for k := 1; k < size+1; k++ {
		b = buffer[k]

		if b == 0xFF {
			fmt.Printf(colRed + "ERROR protocol: unexpected 0xFF\n" + colReset)
			continue
		}

		if (b & 0x80) == 0x80 {
			// key on/off
			keyOn = (b & 0x40) == 0x40

			keyAbs = (b & 0x3F)
			key = int(keyAbs) + 1

			if !keyOn {
				//fmt.Printf("-OFF %d- ", key)
				if keys[key] == 0 {
					fmt.Printf(colRed+"ERROR protocol: key %d is already off\n"+colReset, key)
				}
				keys[key] = 0
				printKeys(buffer, size)
			}

		} else {
			velocity = b

			if keyOn {
				//fmt.Printf("-ON %d [%d]- ", key, velocity)
				if keys[key] > 0 {
					fmt.Printf(colRed+"ERROR protocol: key %d is already on\n"+colReset, key)
				}
				keys[key] = velocity + 1
				printKeys(buffer, size)
			} else {
				fmt.Printf(colRed+"ERROR protocol: velocity %d at key %d off :(\n"+colReset, velocity, key)
			}
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

	for i := 0; i < 64; i++ {
		keys[i] = 0
	}

	// Call every 10 ms
	ticker := time.NewTicker(10 * time.Millisecond)
	defer ticker.Stop()

	for range ticker.C {
		work()
	}
}
