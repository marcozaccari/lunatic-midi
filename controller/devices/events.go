package devices

type MidiEvent struct {
	Ch int

	NoteOn  int
	NoteOff int

	Program int

	Control int
	Value   int
}
type MidiEvents chan MidiEvent

type KeyboardEvent struct {
	Key      int // 1..MaxKeys
	State    bool
	Velocity byte
}
type KeyboardEvents chan KeyboardEvent

type ButtonEvent struct {
	Button int // 1..MaxButtons
	State  bool
}
type ButtonsEvents chan ButtonEvent

type AnalogEvent struct {
	Channel int // 1..MaxChannels
	Type    int
	Value   int
	Raw     int
}
type AnalogEvents chan AnalogEvent

type Events struct {
	Midi     MidiEvents
	Keyboard KeyboardEvents
	Buttons  ButtonsEvents
	Analog   AnalogEvents
}
