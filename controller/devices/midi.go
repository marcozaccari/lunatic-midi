package devices

import (
	"gitlab.com/gomidi/midi/v2"

	//_ "gitlab.com/gomidi/midi/v2/drivers/portmididrv" // autoregisters driver
	"gitlab.com/gomidi/midi/v2/drivers"
	"gitlab.com/gomidi/midi/v2/drivers/rtmididrv"
)

type MidiDevice struct {
	out drivers.Out
	in  drivers.In

	stopListenF func()

	events MidiEvents
}

func NewMidi(port *string) (*MidiDevice, error) {
	dev := &MidiDevice{}

	var err error

	if *port == "" {
		*port = "lunatic"

		d := drivers.Get()
		rtd := d.(*rtmididrv.Driver)

		dev.in, err = rtd.OpenVirtualIn(*port)
		if err != nil {
			return nil, err
		}

		dev.out, err = rtd.OpenVirtualOut(*port)
		if err != nil {
			return nil, err
		}

		return dev, nil
	}

	dev.in, err = midi.FindInPort(*port)
	if err != nil {
		return nil, err
	}

	dev.out, err = midi.FindOutPort(*port)
	if err != nil {
		return nil, err
	}

	err = dev.listen()
	if err != nil {
		dev.done()
		return nil, err
	}

	dev.events = registerMidi(dev)

	return dev, nil
}

func (dev *MidiDevice) SendKey(ch byte, key byte, state bool, vel byte) {
	var msg midi.Message

	if state {
		msg = midi.NoteOn(ch, key, vel)
	} else {
		msg = midi.NoteOff(ch, key)
	}

	dev.out.Send(msg)
}

func (dev *MidiDevice) SendPitchBend(ch byte, value int16) {
	msg := midi.Pitchbend(ch, value)

	dev.out.Send(msg)
}

func (dev *MidiDevice) SendCtrlChange(ch byte, ctrl byte, value byte) {
	msg := midi.ControlChange(ch, ctrl, value)

	dev.out.Send(msg)
}

func (dev *MidiDevice) done() {
	if dev.stopListenF != nil {
		dev.stopListenF()
	}

	midi.CloseDriver()
}

func (dev *MidiDevice) listen() error {
	var err error

	dev.stopListenF, err = midi.ListenTo(dev.in, func(msg midi.Message, timestampms int32) {
		var ch uint8
		var key, vel uint8
		var prog uint8
		var ctrl, val uint8

		switch {
		case msg.GetNoteStart(&ch, &key, &vel):
			dev.events <- MidiEvent{
				Ch:     int(ch),
				NoteOn: int(midi.Note(key).Value()),
			}

			//logs.Tracef("starting note %s on channel %v with velocity %v\n", midi.Note(key), ch, vel)

		case msg.GetNoteEnd(&ch, &key):
			dev.events <- MidiEvent{
				Ch:      int(ch),
				NoteOff: int(midi.Note(key).Value()),
			}

			//logs.Tracef("ending note %s on channel %v\n", midi.Note(key), ch)

		case msg.GetProgramChange(&ch, &prog):
			dev.events <- MidiEvent{
				Ch:      int(ch),
				Program: int(prog),
			}

			//logs.Tracef("program change %d on channel %v\n", prog, ch)

		case msg.GetControlChange(&ch, &ctrl, &val):
			dev.events <- MidiEvent{
				Ch:      int(ch),
				Control: int(ctrl),
				Value:   int(val),
			}

			//logs.Tracef("control change %d=%d on channel %v\n", ctrl, val, ch)

		default:
			// ignore
		}
	}, nil)

	return err
}
