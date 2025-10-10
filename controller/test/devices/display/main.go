package main

import (
	"log"
	"time"

	"github.com/marcozaccari/lunatic-midi/devices"
)

func main() {
	// Configurazione display (usa valori di default)
	cfg := devices.OledConfig{}

	// Oppure personalizza i pin:
	// cfg := devices.OledConfig{
	// 	SPIBus:   "SPI0.0",
	// 	DCPin:    "GPIO27",
	// 	RSTPin:   "GPIO22",
	// 	SPISpeed: 8 * physic.MegaHertz,
	// }

	// Crea istanza display
	display, err := devices.NewOled(cfg)
	if err != nil {
		log.Fatalf("Errore creazione display: %v", err)
	}
	defer display.Close()

	// Inizializza display
	if err := display.Init(); err != nil {
		log.Fatalf("Errore inizializzazione: %v", err)
	}

	log.Println("Display inizializzato!")

	// Test testo
	log.Println("Test testo...")

	display.DrawString(0, 20, "BUONGIORNO", devices.FontMedium)
	display.DrawString(0, 34, "Amore Mio!", devices.FontMedium)
	display.Refresh()
	time.Sleep(10 * time.Second)

	// Font piccolo
	display.DrawString(0, 0, "Hello OLED!", devices.FontSmall)
	display.DrawString(0, 10, "Font piccolo", devices.FontSmall)

	// Font medio
	display.DrawString(0, 20, "AB1234DEFGabcd", devices.FontMedium)

	display.Refresh()
	time.Sleep(10 * time.Second)

	// Test Print/Println
	display.Clear()
	display.SetCursor(0, 0)
	display.Println("Line 1p", devices.FontMedium)
	display.Println("Line 2pg,123", devices.FontSmall)
	display.Print("Same line ", devices.FontSmall)
	display.Println("continued", devices.FontSmall)
	display.Println("continued2", devices.FontSmall)
	display.Println("continued3...", devices.FontSmall)

	// Numeri e caratteri speciali
	display.Println("123 + 456 = 579", devices.FontSmall)
	//display.Println("Test: OK!", devices.FontSmall)

	display.Refresh()
	time.Sleep(3 * time.Second)

	// Test contrasto
	log.Println("Test contrasto...")
	display.SetContrast(0xFF)
	time.Sleep(1 * time.Second)
	display.SetContrast(0x7F)

	// Disegna alcuni pattern
	log.Println("Disegno pattern...")

	// Quadrato pieno
	display.DrawRect(10, 10, 30, 30, true)

	// Quadrato con bordo
	display.DrawRectOutline(50, 10, 30, 30, true)

	// Linea orizzontale
	display.DrawHLine(0, 50, 128, true)

	// Linea verticale
	display.DrawVLine(64, 0, 64, true)

	// Pattern di pixel alternati
	for i := 0; i < 128; i += 4 {
		for j := 54; j < 60; j++ {
			display.SetPixel(i, j, true)
		}
	}

	// Refresh display
	if err := display.Refresh(); err != nil {
		log.Fatalf("Errore refresh: %v", err)
	}

	log.Println("Pattern visualizzato!")
	time.Sleep(5 * time.Second)

	// Test on/off
	log.Println("Test on/off...")
	display.Off()
	time.Sleep(1 * time.Second)
	display.On()
	time.Sleep(1 * time.Second)

	// Test inversione colori
	log.Println("Test inversione colori...")
	display.Invert(true)
	time.Sleep(1 * time.Second)
	display.Invert(false)

	// Test clear e fill
	log.Println("Test clear/fill...")
	display.Fill()
	display.Refresh()
	time.Sleep(1 * time.Second)

	display.Clear()
	display.Refresh()
	time.Sleep(1 * time.Second)

	log.Println("Test completato!")
}
