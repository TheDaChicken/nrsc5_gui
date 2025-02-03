//
// Created by TheDaChicken on 7/20/2023.
//

#include "BandpassFilter.h"

#include "dsp/filter/Firdes.h"

#include "utils/Log.h"

BandpassFilter::BandpassFilter(double sample_rate, double low, double high, double trans_width)
	: d_sample_rate(sample_rate), d_low(0), d_high(0), d_trans_width(0), m_filter(nullptr)
{
  SetParam(low, high, trans_width);
}

BandpassFilter::~BandpassFilter()
{
  Close();
}

void BandpassFilter::Close()
{
  if (m_filter)
	firfilt_cccf_destroy(m_filter);
  m_filter = nullptr;
}

void BandpassFilter::Reset()
{
  firfilt_cccf_reset(m_filter);
}

void BandpassFilter::SetParam(double low, double high, double trans_width)
{
  d_low = low;
  d_high = high;
  d_trans_width = trans_width;

  if (d_low < -0.95 * d_sample_rate / 2.0)
	d_low = -0.95 * d_sample_rate / 2.0;
  if (d_high > 0.95 * d_sample_rate / 2.0)
	d_high = 0.95 * d_sample_rate / 2.0;

  d_taps = Firdes::complex_band_pass(1.0, d_sample_rate, d_low, d_high, d_trans_width);

  Close();
  m_filter = firfilt_cccf_create(d_taps.data(), d_taps.size());
}

void BandpassFilter::Process(const vector_complex_t &input, vector_complex_t &output)
{
  output.Resize(input.Size());
  firfilt_cccf_execute_block(m_filter, const_cast<cfloat_t *>(input.Data()),
							 input.Size(), output.Data());
}
