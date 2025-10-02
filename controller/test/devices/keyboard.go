package main

import (
	"fmt"
	"time"

	"github.com/marcozaccari/lunatic-midi/devices/hardware"
)

const I2C_ADDR = 0x30

var i2c hardware.I2C

func work() {
	var buffer [256]byte
	var size int

	err := i2c.Read(buffer[:], 1)
	if err != nil {
		fmt.Printf("\nError reading len: %s\n", err)
		println(err)
		return
	}

	size = int(buffer[0])
	if size == 0 {
		//fmt.Printf(".")
		return
	}

	//printf("received %d bytes", size)

	//err = i2c.Read(buffer[:], size+1)
	err = i2c.Read(buffer[:], size+1)
	if err != nil {
		fmt.Printf("\nError reading data: %s\n", err)
		return
	}
	//if buffer[0] != 1 {
	//	fmt.Printf("\nError reading data (header): %X\n", buffer[0])
	//return
	//}

	var b byte
	var keyAbs byte
	var key int
	var keyOn bool
	var velocity byte

	for k := 1; k < size+1; k++ {
		b = buffer[k]

		if (b & 0x80) == 0x80 {
			// key
			keyOn = (b & 0x40) == 0x40

			keyAbs = (b & 0x3F)
			key = int(keyAbs) + 1
		} else {
			velocity = b

			if keyOn {
				fmt.Printf("-ON %d [%d]- ", key, velocity)
			} else {

				fmt.Printf("-OFF %d [%d]- ", key, velocity)
			}
		}
	}
}

func main() {
	err := i2c.Open(I2C_ADDR)
	if err != nil {
		panic(err)
	}

	// Call every 10 ms
	ticker := time.NewTicker(10 * time.Millisecond)
	defer ticker.Stop()

	for range ticker.C {
		work()
	}
}
