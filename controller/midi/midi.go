package midi

import (
	"fmt"
	"time"

	"gitlab.com/gomidi/midi/v2"

	//_ "gitlab.com/gomidi/midi/v2/drivers/portmididrv" // autoregisters driver
	"gitlab.com/gomidi/midi/v2/drivers"
	"gitlab.com/gomidi/midi/v2/drivers/rtmididrv"
)

type Midi struct {
	out drivers.Out
	in  drivers.In
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

		m.in, err = rtd.OpenVirtualIn(port)
		if err != nil {
			return port, err
		}

		m.out, err = rtd.OpenVirtualOut(port)
		if err != nil {
			return port, err
		}

		return port, nil
	}

	m.in, err = midi.FindInPort(port)
	if err != nil {
		return port, err
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

func (m *Midi) SendKey(ch byte, key byte, state bool, vel byte) {
	var msg midi.Message

	if state {
		msg = midi.NoteOn(ch, key, vel)
	} else {
		msg = midi.NoteOff(ch, key)
	}

	m.out.Send(msg)
}

func (m *Midi) SendPitchBend(ch byte, value int16) {
	msg := midi.Pitchbend(ch, value)

	m.out.Send(msg)
}

func (m *Midi) SendCtrlChange(ch byte, ctrl byte, value byte) {
	msg := midi.ControlChange(ch, ctrl, value)

	m.out.Send(msg)
}

func PrintPorts() {
	fmt.Println("MIDI ports:")

	ports := midi.GetOutPorts()

	for _, p := range ports {
		fmt.Printf("%s\n", p)
	}
}

func (m *Midi) Listen() error {
	stop, err := midi.ListenTo(m.in, func(msg midi.Message, timestampms int32) {
		var ch uint8
		var key, vel uint8
		var prog uint8
		var ctrl, val uint8

		switch {
		case msg.GetNoteStart(&ch, &key, &vel):
			fmt.Printf("starting note %s on channel %v with velocity %v\n", midi.Note(key), ch, vel)
		case msg.GetNoteEnd(&ch, &key):
			fmt.Printf("ending note %s on channel %v\n", midi.Note(key), ch)
		case msg.GetProgramChange(&ch, &prog):
			fmt.Printf("program change %d on channel %v\n", prog, ch)
		case msg.GetControlChange(&ch, &ctrl, &val):
			fmt.Printf("control change %d=%d on channel %v\n", ctrl, val, ch)
		default:
			// ignore
		}
	}, nil)

	if err != nil {
		return err
	}

	time.Sleep(time.Second * 5)

	stop()

	return nil
}

func (m *Midi) Stop() {
}
