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

	"github.com/marcozaccari/lunatic-midi/config"
	"github.com/marcozaccari/lunatic-midi/devices"
	"github.com/marcozaccari/lunatic-midi/events"
	"github.com/marcozaccari/lunatic-midi/modes"
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
	logs.Infof("Config file: %s", config.GetFilename())

	err = start()

	config.Done()
	logs.Done()

	if err != nil {
		os.Exit(-2)
	}
}

func start() error {
	ch := events.NewChannels()

	devScheduler, err := devices.NewScheduler(config.Cfg.Devices, ch)
	if err != nil {
		return fmt.Errorf("Error initializing devices scheduler: %s", err)
	}

	go devScheduler.Work()

	defMode := modes.NewDefault(ch)
	go defMode.Work()

	sigc := make(chan os.Signal, 1)
	signal.Notify(sigc,
		syscall.SIGINT,
		syscall.SIGTERM,
		syscall.SIGQUIT,
	)

	sig := <-sigc
	logs.Warnf("terminating by signal \"%v\"", sig)

	devScheduler.Stop()
	defMode.Stop()

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
