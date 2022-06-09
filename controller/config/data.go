// Code generated by json2struct 1.5. (@generated) DO NOT EDIT.
// cmdline: `json2struct -out=data.go defaults.jsonc`

package config

type Data struct {
	MIDI	Data_MIDI	`json:"MIDI"`
	Devices	Data_Devices	`json:"Devices"`
	Logging	Data_Logging	`json:"Logging"`
}

type Data_MIDI struct {
	PortName	string	`json:"PortName"`
}

type Data_Devices struct {
	Keyboards	[]Data_Devices_KeyboardsItem	`json:"Keyboards"`
	Buttons	Data_Devices_Buttons	`json:"Buttons"`
	Analog	Data_Devices_Analog	`json:"Analog"`
	LedStrip	Data_Devices_LedStrip	`json:"LedStrip"`
}

type Data_Devices_KeyboardsItem struct {
	Offset	int	`json:"Offset"`
	Velocity	string	`json:"Velocity"`
	I2C	string	`json:"I2C"`
}

type Data_Devices_Buttons struct {
	I2C	string	`json:"I2C"`
}

type Data_Devices_Analog struct {
	I2C	string	`json:"I2C"`
	Channel1	string	`json:"Channel1"`
	Channel2	string	`json:"Channel2"`
	Channel3	string	`json:"Channel3"`
	Channel4	string	`json:"Channel4"`
}

type Data_Devices_LedStrip struct {
	I2C	string	`json:"I2C"`
}

type Data_Logging struct {
	Stdout	bool	`json:"Stdout"`
	Syslog	bool	`json:"Syslog"`
	File	string	`json:"File"`
	TCPPort	int	`json:"TCP_Port"`
}

func defaultData() Data {
	return Data{

		MIDI: Data_MIDI{
			PortName: "hw:0,0,0",
		},

		Devices: Data_Devices{
			Keyboards: []Data_Devices_KeyboardsItem{
				{
					I2C: "0x00",
					Offset: 0,
					Velocity: "",
				},
			},

			Buttons: Data_Devices_Buttons{
				I2C: "0x00",
			},

			Analog: Data_Devices_Analog{
				I2C: "0x00",
				Channel1: "",
				Channel2: "",
				Channel3: "",
				Channel4: "",
			},

			LedStrip: Data_Devices_LedStrip{
				I2C: "0x00",
			},
		},

		Logging: Data_Logging{
			Stdout: true,
			Syslog: false,
			File: "",
			TCPPort: 0,
		},
	}
}

const raw = "\x7b\x0a\x20\x20\x20\x20\x22\x4d\x49\x44\x49\x22\x3a\x20\x7b\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x47\x65\x74\x20\x70\x6f\x72\x74\x73\x20\x6e\x61\x6d\x65\x73\x20\x75\x73\x69\x6e\x67\x20\x22\x61\x6d\x69\x64\x69\x20\x2d\x6c\x22\x20\x6f\x72\x20\x22\x6c\x75\x6e\x61\x74\x69\x63\x2d\x6d\x69\x64\x69\x20\x6d\x69\x64\x69\x70\x6f\x72\x74\x73\x22\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x49\x6e\x20\x6f\x72\x64\x65\x72\x20\x74\x6f\x20\x6f\x62\x74\x61\x69\x6e\x20\x76\x69\x72\x74\x75\x61\x6c\x20\x70\x6f\x72\x74\x73\x2c\x20\x75\x73\x65\x3a\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x20\x20\x22\x73\x75\x64\x6f\x20\x6d\x6f\x64\x70\x72\x6f\x62\x65\x20\x73\x6e\x64\x2d\x76\x69\x72\x6d\x69\x64\x69\x22\x20\x6f\x72\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x20\x20\x22\x73\x75\x64\x6f\x20\x65\x63\x68\x6f\x20\x27\x73\x6e\x64\x2d\x76\x69\x72\x6d\x69\x64\x69\x27\x20\x3e\x3e\x20\x2f\x65\x74\x63\x2f\x6d\x6f\x64\x75\x6c\x65\x73\x22\x20\x6f\x72\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x20\x20\x22\x73\x75\x64\x6f\x20\x65\x63\x68\x6f\x20\x27\x73\x6e\x64\x2d\x76\x69\x72\x6d\x69\x64\x69\x20\x69\x6e\x64\x65\x78\x3d\x6e\x27\x20\x3e\x3e\x20\x2f\x65\x74\x63\x2f\x6d\x6f\x64\x70\x72\x6f\x62\x65\x2e\x64\x2f\x73\x6e\x64\x2d\x76\x69\x72\x6d\x69\x64\x69\x2e\x63\x6f\x6e\x66\x22\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x22\x50\x6f\x72\x74\x4e\x61\x6d\x65\x22\x3a\x20\x22\x68\x77\x3a\x30\x2c\x30\x2c\x30\x22\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x52\x61\x73\x70\x62\x65\x72\x72\x79\x20\x5a\x65\x72\x6f\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x22\x50\x6f\x72\x74\x4e\x61\x6d\x65\x22\x3a\x20\x22\x68\x77\x3a\x32\x2c\x30\x2c\x30\x22\x0a\x20\x20\x20\x20\x7d\x2c\x0a\x0a\x20\x20\x20\x20\x22\x44\x65\x76\x69\x63\x65\x73\x22\x3a\x20\x7b\x0a\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x22\x4b\x65\x79\x62\x6f\x61\x72\x64\x73\x22\x3a\x20\x5b\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x7b\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x49\x32\x43\x20\x61\x64\x64\x72\x65\x73\x73\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x22\x49\x32\x43\x22\x3a\x20\x22\x30\x78\x30\x30\x22\x2c\x0a\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x4e\x6f\x74\x65\x73\x20\x6f\x66\x66\x73\x65\x74\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x22\x4f\x66\x66\x73\x65\x74\x22\x3a\x20\x30\x2c\x0a\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x56\x65\x6c\x6f\x63\x69\x74\x79\x20\x63\x75\x72\x76\x65\x20\x28\x64\x61\x74\x61\x2f\x76\x65\x6c\x6f\x63\x69\x74\x79\x2f\x2a\x2e\x64\x61\x74\x29\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x22\x56\x65\x6c\x6f\x63\x69\x74\x79\x22\x3a\x20\x22\x22\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x7d\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x5d\x2c\x0a\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x22\x42\x75\x74\x74\x6f\x6e\x73\x22\x3a\x20\x7b\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x49\x32\x43\x20\x61\x64\x64\x72\x65\x73\x73\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x22\x49\x32\x43\x22\x3a\x20\x22\x30\x78\x30\x30\x22\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x7d\x2c\x0a\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x22\x41\x6e\x61\x6c\x6f\x67\x22\x3a\x20\x7b\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x49\x32\x43\x20\x61\x64\x64\x72\x65\x73\x73\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x22\x49\x32\x43\x22\x3a\x20\x22\x30\x78\x30\x30\x22\x2c\x0a\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x43\x68\x61\x6e\x6e\x65\x6c\x20\x74\x79\x70\x65\x3a\x20\x22\x73\x6c\x69\x64\x65\x72\x22\x20\x28\x64\x65\x66\x61\x75\x6c\x74\x29\x20\x2f\x20\x22\x72\x69\x62\x62\x6f\x6e\x22\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x22\x43\x68\x61\x6e\x6e\x65\x6c\x31\x22\x3a\x20\x22\x22\x2c\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x22\x43\x68\x61\x6e\x6e\x65\x6c\x32\x22\x3a\x20\x22\x22\x2c\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x22\x43\x68\x61\x6e\x6e\x65\x6c\x33\x22\x3a\x20\x22\x22\x2c\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x22\x43\x68\x61\x6e\x6e\x65\x6c\x34\x22\x3a\x20\x22\x22\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x7d\x2c\x0a\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x22\x4c\x65\x64\x53\x74\x72\x69\x70\x22\x3a\x20\x7b\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x2f\x2f\x20\x49\x32\x43\x20\x61\x64\x64\x72\x65\x73\x73\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x22\x49\x32\x43\x22\x3a\x20\x22\x30\x78\x30\x30\x22\x0a\x20\x20\x20\x20\x20\x20\x20\x20\x7d\x0a\x20\x20\x20\x20\x7d\x2c\x0a\x0a\x20\x20\x20\x20\x22\x4c\x6f\x67\x67\x69\x6e\x67\x22\x3a\x20\x7b\x0a\x09\x09\x22\x53\x74\x64\x6f\x75\x74\x22\x3a\x20\x74\x72\x75\x65\x2c\x0a\x09\x09\x22\x53\x79\x73\x6c\x6f\x67\x22\x3a\x20\x66\x61\x6c\x73\x65\x2c\x0a\x09\x09\x22\x46\x69\x6c\x65\x22\x3a\x20\x22\x22\x2c\x0a\x09\x09\x22\x54\x43\x50\x5f\x50\x6f\x72\x74\x22\x3a\x20\x30\x0a\x20\x20\x20\x20\x7d\x0a\x7d\x0a"

