//
// Created by TheDaChicken on 7/18/2023.
//

#include "Resampler.h"

#include "dsp/filter/Firdes.h"
#include "utils/Log.h"

ResamplerCC::ResamplerCC(float rate, float suppression)
	: m_rate(0), m_resampler(nullptr)
{
  SetRate(rate, suppression);
}

ResamplerCC::~ResamplerCC()
{
  if (m_resampler)
	msresamp_crcf_destroy(m_resampler);
}

void ResamplerCC::Reset()
{
  msresamp_crcf_reset(m_resampler);
}

void ResamplerCC::SetRate(float rate, float suppression)
{
  m_rate = rate;
  m_resampler = msresamp_crcf_create(rate, suppression);
}

int ResamplerCC::GetOutputSamples(std::size_t inputSize) const
{
  return (int)ceilf((float)inputSize * m_rate) + 512;
}

void ResamplerCC::Process(const vector_complex_t &input, vector_complex_t &output)
{
  unsigned int num_written;
  output.Reserve(GetOutputSamples(input.Size()));

  msresamp_crcf_execute(m_resampler, const_cast<cfloat_t*>(input.Data()), input.Size(),
						output.Data(), &num_written);
  output.Resize(num_written);
}

ResamplerFF::ResamplerFF(float rate)
	: m_rate(0), m_resampler(nullptr)
{
  SetRate(rate);
}

void ResamplerFF::Reset()
{
  msresamp_rrrf_reset(m_resampler);
}

void ResamplerFF::SetRate(float rate)
{
  m_rate = rate;
  m_resampler = msresamp_rrrf_create(rate, 100.0f);
}

int ResamplerFF::GetOutputSamples(std::size_t inputSize) const
{
  return (int)ceilf((float)inputSize * m_rate) + 512;
}

void ResamplerFF::Process(const vector_float_t &input, vector_float_t &output)
{
  unsigned int num_written;
  output.Resize(GetOutputSamples(input.Size()));

  msresamp_rrrf_execute(m_resampler, const_cast<float*>(input.Data()), input.Size(),
						output.Data(), &num_written);
  output.Resize(num_written);
}

