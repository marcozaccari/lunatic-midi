package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"os/signal"
	"path/filepath"
	"syscall"
	"time"

	"github.com/marcozaccari/lunatic-midi/cmd/linear-keyboard/config"
	"github.com/marcozaccari/lunatic-midi/cmd/linear-keyboard/worker"
	"github.com/marcozaccari/lunatic-midi/devices"

	"github.com/modulo-srl/sparalog"
	"github.com/modulo-srl/sparalog/logs"

	sync "github.com/sasha-s/go-deadlock"
)

func usage() {
	println(config.Version, "\n")

	println("Usage:")
	println("  linear-keyboard <flags>")
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
	fconfig := flag.String("config-file", "settings.json", "Set config file's absolute or relative path")
	fmidiports := flag.Bool("midiports", false, "Show system MIDI ports")
	flag.Parse()

	if *fversion {
		usage()
		return
	}

	if *fmidiports {
		devices.PrintMidiPorts()
		return
	}

	if *fdefaultConfig {
		fmt.Println(config.GetDefaultConfig())
		return
	}

	err = config.LoadConfig(*fconfig)
	if err != nil {
		log.Panicf("Error loading settings: %v", err)
	}

	setLogs(*fdebug)

	logs.Info(config.Version)

	devs, err := config.ParseConfig()
	if err != nil {
		log.Panicf("%s ", err)
	}

	err = start(devs)
	if err != nil {
		logs.Error(err)
	}

	logs.Done()

	if err != nil {
		os.Exit(-2)
	}
}

func start(devs worker.OutputDevices) error {
	worker := worker.New(devs)

	// Start worker.
	events := devices.Start()
	worker.Start(events)

	sigc := make(chan os.Signal, 1)
	signal.Notify(sigc,
		syscall.SIGINT,
		syscall.SIGTERM,
		syscall.SIGQUIT,
	)

	sig := <-sigc
	logs.Warnf("terminating by signal \"%v\"", sig)

	devices.Stop()
	worker.Stop()

	return nil
}

func setLogs(debug bool) {
	defer logs.Open()

	if debug {
		// All log levels to standard out.
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
		sys := logs.NewSyslogWriter("linear-keyboard")
		logs.AddWriter(sys)
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

		logs.AddLevelsWriter(sparalog.CriticalLevels, wfi)
		logs.AddLevelWriter(sparalog.InfoLevel, wfi)
	}

	// TCP
	if config.Cfg.Logging.TCPPort > 0 {
		port := config.Cfg.Logging.TCPPort
		logs.Info("tracing logs forwarded to TCP port ", port)

		wtcp, err := logs.NewTCPWriter("", port, debug, func(state bool) {
			if debug {
				if state {
					logs.Info("enable tracing log to TCP")
				} else {
					logs.Info("disable tracing log to TCP")
				}
			}
		})

		if err != nil {
			panic(err)
		}

		logs.AddWriter(wtcp)
	}

	// Watcher deadlock mutex.
	sync.Opts.DeadlockTimeout = time.Second * 10
	sync.Opts.OnPotentialDeadlock = func() {} // avoid ending the process
}
