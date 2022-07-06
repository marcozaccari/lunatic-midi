package events

type Buttons struct {
	Button int
	State  bool
}

type Keyboard struct {
	KeyboardNum int

	Key      int
	State    bool
	Velocity byte
}

type Analog struct {
	Channel int
	Value   int
}

type MIDI struct {
	Ch int

	NoteOn  int
	NoteOff int

	Program int

	Control int
	Value   int
}

type Channels struct {
	Keyboard Channel[Keyboard]
	Buttons  Channel[Buttons]
	Analog   Channel[Analog]
	MIDI     Channel[MIDI]
}

type Channel[E Keyboard | Buttons | Analog | MIDI] chan E

func NewChannels() Channels {
	return Channels{
		Keyboard: make(Channel[Keyboard], 32),
		Buttons:  make(Channel[Buttons], 32),
		Analog:   make(Channel[Analog], 32),
		MIDI:     make(Channel[MIDI], 32),
	}
}
