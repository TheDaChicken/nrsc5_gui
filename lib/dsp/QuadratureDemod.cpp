//
// Created by TheDaChicken on 7/15/2023.
//

#include "QuadratureDemod.h"

QuadratureDemod::QuadratureDemod(int32_t sample_rate, float max_freq_dev)
	: m_gain(static_cast<float>(sample_rate) / (2.0f * max_freq_dev * static_cast<float>(M_PI)))
	  , m_ref(0, 0)
{
}

void QuadratureDemod::Process(const vector_complex_t &input, vector_float_t &output)
{
	output.resize(input.size());

	for (std::size_t i = 0; i < output.size(); ++i)
	{
		std::complex<float> d(conj(m_ref) * input[i]);
		output[i] = m_gain * atan2f(d.imag(), d.real());
		m_ref = input[i];
	}
}

