module github.com/marcozaccari/lunatic-midi

go 1.20

require (
	github.com/d2r2/go-i2c v0.0.0-20191123181816-73a8a799d6bc
	github.com/d2r2/go-logger v0.0.0-20210606094344-60e9d1233e22
	github.com/modulo-srl/mu-config v1.0.6
	github.com/modulo-srl/sparalog v1.0.0
	github.com/sasha-s/go-deadlock v0.3.1
	gitlab.com/gomidi/midi/v2 v2.0.30
)

require (
	github.com/davecgh/go-spew v1.1.1 // indirect
	github.com/mitchellh/go-homedir v1.1.0 // indirect
	github.com/mitchellh/panicwrap v1.0.0 // indirect
	github.com/pelletier/go-toml/v2 v2.1.1 // indirect
	github.com/petermattis/goid v0.0.0-20231207134359-e60b3f734c67 // indirect
	gitlab.com/c0b/go-ordered-json v0.0.0-20201030195603-febf46534d5a // indirect
	gopkg.in/yaml.v3 v3.0.1 // indirect
)

//replace github.com/modulo-srl/mu-config => /home/mark/projects/mu-config
