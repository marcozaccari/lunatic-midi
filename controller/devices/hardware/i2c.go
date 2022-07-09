package hardware

import (
	"fmt"

	"github.com/d2r2/go-i2c"
	"github.com/d2r2/go-logger"
)

type I2C struct {
	Address byte

	i2c *i2c.I2C
}

func (i *I2C) Open(addr byte) error {
	var err error

	i.Address = addr

	i.i2c, err = i2c.NewI2C(i.Address, 1)
	if err != nil {
		return err
	}

	return nil
}

func (i *I2C) Close() {
	i.i2c.Close()
}

func (i *I2C) Write(bb []byte, count int) error {
	cc, err := i.i2c.WriteBytes(bb[:count])
	if err != nil {
		return err
	}

	if cc != count {
		return fmt.Errorf("i2c: expected %d bytes, got %d", count, cc)
	}

	return err
}

func (i *I2C) Read(bb []byte, count int) error {
	cc, err := i.i2c.ReadBytes(bb[:count])
	if err != nil {
		return err
	}

	if cc != count {
		return fmt.Errorf("i2c: expected %d bytes, got %d", count, cc)
	}

	return err
}

func init() {
	logger.ChangePackageLogLevel("i2c", logger.FatalLevel)
}
