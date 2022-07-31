package devices

// Single-pole infinite impulse response filters
// or Exponential Moving Average filters.
//
// Optimized implementation of the difference equation with a slight
// optimization by using a factor of two as the pole location (this means
// that no division or floating point operations are required).
//
// Difference equation: @f$ y[n] = \alpha·x[n]+(1-\alpha)·y[n-1] @f$
// where @f$ \alpha = \left(\frac{1}{2}\right)^{K} @f$, @f$ x @f$ is the
// input sequence, and @f$ y @f$ is the output sequence.
//
// [An in-depth explanation of the EMA filter]
// (https://tttapa.github.io/Pages/Mathematics/Systems-and-Control-Theory/Digital-filters/Exponential%20Moving%20Average/)
type EMAfilter struct {
	// The amount of bits to shift by. This determines the location
	// of the pole in the EMA transfer function, and therefore the
	// cut-off frequency.
	// The higher this number, the more filtering takes place.
	// The pole location is 1 - 2^{-bits}.
	bits uint

	// Note: the type of state should be at least M+K bits wide,
	// where M is the maximum number of bits of the input.
	state uint
	half  uint
}

func NewEMAfilter(bits uint) *EMAfilter {
	return &EMAfilter{
		state: 0,
		half:  1 << (bits - 1),
		bits:  bits,
	}
}

func (f *EMAfilter) filter(input uint) uint {
	f.state += input

	output := (f.state + f.half) >> f.bits

	f.state -= output

	return output
}
