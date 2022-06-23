package midi

import (
	"fmt"

	"gitlab.com/gomidi/midi/v2"

	//_ "gitlab.com/gomidi/midi/v2/drivers/portmididrv" // autoregisters driver
	"gitlab.com/gomidi/midi/v2/drivers"
	"gitlab.com/gomidi/midi/v2/drivers/rtmididrv"
)

type Midi struct {
	out drivers.Out
}

func New() *Midi {
	return &Midi{}
}

func (m *Midi) Init(port string) (string, error) {
	var err error

	if port == "" {
		port = "lunatic"

		d := drivers.Get()
		rtd := d.(*rtmididrv.Driver)

		m.out, err = rtd.OpenVirtualOut(port)
		if err != nil {
			return port, err
		}

		return port, nil
	}

	m.out, err = midi.FindOutPort(port)
	if err != nil {
		return port, err
	}

	return port, nil
}

func (m *Midi) Done() {
	midi.CloseDriver()
}

func (m *Midi) SendKey(ch int, key int, state bool, vel byte) {
	var msg midi.Message

	if state {
		msg = midi.NoteOn(byte(ch), byte(key), vel)
	} else {
		msg = midi.NoteOff(byte(ch), byte(key))
	}

	m.out.Send(msg)
}

func PrintPorts() {
	fmt.Println("MIDI ports:")

	ports := midi.GetOutPorts()

	for _, p := range ports {
		fmt.Printf("%s\n", p)
	}
}
