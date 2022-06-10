package hardware

type I2C struct {
}

type I2CBuffer [256]byte

func (i *I2C) Open(addr byte) error {
	return nil
}

func (i *I2C) Close() {
}

func (i *I2C) Write(b *I2CBuffer, count int) error {
	return nil
}

func (i *I2C) Read(b *I2CBuffer, count int) error {
	return nil
}
