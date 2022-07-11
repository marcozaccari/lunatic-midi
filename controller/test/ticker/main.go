package main

// Raspberry Zero W
// GOMAXPROCS: 1
// 892 × 1ms ticks in 1s
// 1464 × 100µs ticks in 1s
// 16487 × 10µs ticks in 1s
// 14585 × 1µs ticks in 1s
// 971 × 500µs ticks in 1s

import (
	"fmt"
	"runtime"
	"time"
)

func ticker(tick time.Duration) {
	ticker := time.NewTicker(tick)
	defer ticker.Stop()
	done := make(chan bool)
	sleep := 1 * time.Second
	go func() {
		time.Sleep(sleep)
		done <- true
	}()
	ticks := 0
	for {
		select {
		case <-done:
			fmt.Printf("%v × %v ticks in %v\n", ticks, tick, sleep)
			return
		case <-ticker.C:
			ticks++
		}
	}
}

func main() {
	fmt.Println("GOMAXPROCS:", runtime.GOMAXPROCS(0))
	start := time.Duration(1 * time.Millisecond)
	end := start / 1000
	for tick := start; tick >= end; tick /= 10 {
		ticker(tick)
	}
}
