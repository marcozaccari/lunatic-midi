package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strconv"
	"time"

	"github.com/marcozaccari/lunatic-midi/config"
	"github.com/marcozaccari/lunatic-midi/devices"
	"github.com/modulo-srl/sparalog"
	"github.com/modulo-srl/sparalog/logs"
	sync "github.com/sasha-s/go-deadlock"
)

func usage() {
	println(config.Version, "\n")

	println("Usage:")
	println("  lunatic-midi <flags>")
	println()

	flag.PrintDefaults()
	println()
}

func main() {
	var err error

	//rand.Seed(time.Now().UnixNano())

	flag.Usage = usage

	fversion := flag.Bool("version", false, "Show program version")
	fdebug := flag.Bool("debug", false, "Logs on stdout/stderr")
	fdefaultConfig := flag.Bool("default-config", false, "Show default config")
	fconfig := flag.String("config-file", "config.json", "Set config file's absolute or relative path")
	flag.Parse()

	if *fversion {
		usage()
		return
	}

	if *fdefaultConfig {
		fmt.Println(config.GetDefaultConfig())
		return
	}

	err = config.Init(*fconfig)
	if err != nil {
		log.Panicln("Error initializing settings:", err)
	}
	err = config.LoadConfig()
	if err != nil {
		log.Panicf("Error loading settings %v: %v", config.GetFilename(), err)
	}

	setLogs(*fdebug)

	logs.Info(config.Version)
	logs.Infof("config file: %s", config.GetFilename())

	// TODO
	readConfigDevices()

	config.Done()
	logs.Done()

	if err != nil {
		os.Exit(-2)
	}
}

type Device interface {
	Work() error
}

var devs []Device

func readConfigDevices() error {
	devs := make([]Device, 0, 8)

	for _, cfgKeyb := range config.Cfg.Devices.Keyboards {
		addr, err := strconv.ParseInt(cfgKeyb.I2C, 0, 0)
		if err != nil {
			return err
		}

		keyb, err := devices.NewKeyboard(byte(addr), cfgKeyb.Offset)
		if err != nil {
			return err
		}

		devs = append(devs, keyb)

		logs.Infof("Keyboard initialized (%s)", cfgKeyb.I2C)
	}

	if config.Cfg.Devices.Buttons.I2C != "" {
		addr, err := strconv.ParseInt(config.Cfg.Devices.Buttons.I2C, 0, 0)
		if err != nil {
			return err
		}

		ana, err := devices.NewButtons(byte(addr))
		if err != nil {
			return err
		}

		devs = append(devs, ana)

		logs.Infof("Buttons initialized (%s)", config.Cfg.Devices.Buttons.I2C)
	}

	if config.Cfg.Devices.Analog.I2C != "" {
		addr, err := strconv.ParseInt(config.Cfg.Devices.Analog.I2C, 0, 0)
		if err != nil {
			return err
		}

		ana, err := devices.NewAnalog(byte(addr))
		if err != nil {
			return err
		}

		devs = append(devs, ana)

		logs.Infof("Analog initialized (%s)", config.Cfg.Devices.Analog.I2C)
	}

	if config.Cfg.Devices.LedStrip.I2C != "" {
		addr, err := strconv.ParseInt(config.Cfg.Devices.Buttons.I2C, 0, 0)
		if err != nil {
			return err
		}

		ana, err := devices.NewLedStrip(byte(addr))
		if err != nil {
			return err
		}

		devs = append(devs, ana)

		logs.Infof("LedStrip initialized (%s)", config.Cfg.Devices.LedStrip.I2C)
	}

	return nil
}

func setLogs(debug bool) {
	defer logs.Open()

	if debug {
		logs.Mute(sparalog.DebugLevel, false)
		logs.Mute(sparalog.TraceLevel, false)
		return
	}

	if config.Cfg.Logging.Stdout {
		logs.Mute(sparalog.DebugLevel, false)
		logs.Mute(sparalog.TraceLevel, false)
		logs.ResetLevelsWriters(sparalog.DebugLevels, nil)
	} else {
		logs.ResetWriters(nil)
	}

	if config.Cfg.Logging.Syslog {
		sys := logs.NewSyslogWriter("lunatic-midi")
		logs.AddWriter(sys, "")
	}

	// File
	if config.Cfg.Logging.File != "" {
		filename := config.Cfg.Logging.File

		if filename[0] != '/' {
			ex, err := os.Executable()
			if err != nil {
				panic(err)
			}
			filename = filepath.Dir(ex) + "/" + filename
		}

		wfi, err := logs.NewFileWriter(filename)
		if err != nil {
			panic(err)
		}

		logs.AddLevelsWriter(sparalog.CriticalLevels, wfi, "")
		logs.AddLevelWriter(sparalog.InfoLevel, wfi, "")
	}

	// TCP
	if config.Cfg.Logging.TCPPort > 0 {
		port := config.Cfg.Logging.TCPPort
		logs.Info("tracing logs forwarded to TCP port ", port)

		wtcp, err := logs.NewTCPWriter("", port, debug, func(state bool) {
			// true = è entrato il primo client
			// false = è uscito l'ultimo client
			if debug {
				if state {
					logs.Info("enable tracing log to TCP")
				} else {
					logs.Info("disable tracing log to TCP")
				}
			}

			// i seguenti livelli sono controllati dalla presenza/assenza di writer.
			//logs.Mute(sparalog.DebugLevel, !state)
			//logs.Mute(sparalog.TraceLevel, !state)
		})

		if err != nil {
			panic(err)
		}

		logs.AddWriter(wtcp, "")
	}

	// Impostazioni watcher deadlock mutex.
	sync.Opts.DeadlockTimeout = time.Second * 10
	sync.Opts.OnPotentialDeadlock = func() {} // evita di terminare il processo
}
