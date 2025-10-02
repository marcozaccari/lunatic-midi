package main

import (
	"fmt"
	"time"

	"github.com/marcozaccari/lunatic-midi/devices/hardware"
)

const I2C_ADDR = 0x30

var i2c hardware.I2C

var last_byte byte
var testing bool

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
	for k := 1; k < size+1; k++ {
		if buffer[k] != 0xFF {
			b = buffer[k]
			fmt.Printf("0x%02X ", buffer[k])

			if !testing {
				testing = true
				last_byte = b
			} else {
				if !((b == 1) && (last_byte == 90)) && (b != last_byte+1) {
					//if b != 0x23 {
					fmt.Printf("\nORDER MISMATCH!\n")
				}

				last_byte = b
			}
		}
	}
}

func main() {
	err := i2c.Open(I2C_ADDR)
	if err != nil {
		panic(err)
	}

	testing = false

	// Call every 30 ms
	ticker := time.NewTicker(30 * time.Millisecond)
	defer ticker.Stop()

	for range ticker.C {
		work()
	}
}
