//
// Created by TheDaChicken on 7/28/2023.
//

#include "FMPilotPhaseLock.h"

#include "dsp/filter/Firdes.h"

#include "utils/Log.h"

FMPilotPhaseLock::FMPilotPhaseLock(uint32_t d_input_rate,
								   float min_freq, float max_freq)
	: m_smoothingFactor(0.90), m_pilot_level(0.0)
{

  d_tone_taps = Firdes::complex_band_pass(
	  1.0,                  // gain,
	  (double)d_input_rate, // sampling_freq
	  min_freq,             // low_cutoff_freq
	  max_freq,             // high_cutoff_freq
	  5000.);               // transition_width

  firStereoR2C = firhilbf_create(5, 60.0f);
  firStereoC2R = firhilbf_create(5, 60.0f);

  stereoPilot = nco_crcf_create(LIQUID_VCO);

  // PLL bandwidth
  nco_crcf_pll_set_bandwidth(stereoPilot, 0.25f);

  firFilter = firfilt_cccf_create(d_tone_taps.data(),
								  d_tone_taps.size());
}

bool FMPilotPhaseLock::IsFound() const
{
  return m_pilot_level > 0.0009;
}

void FMPilotPhaseLock::Process(const vector_float_t &input, vector_float_t &output)
{
  float phase_error;
  std::complex<float> converted, phase, phase_input, filtered, y;

  output.Resize(input.Size());

  m_pilot_level = 0.0;

  for (int i = 0; i < output.Size(); i++)
  {
	// real -> complex
	firhilbf_r2c_execute(firStereoR2C, input[i], &converted);

	// 19khz pilot band-pass
	firfilt_cccf_execute_one(firFilter, converted, &filtered);

	// phase
	nco_crcf_cexpf(stereoPilot, &phase);

	phase_input = filtered * std::conj(phase);
	phase_error = atan2f(phase_input.imag(), phase_input.real());

	// This is what chatgpt AI said. could this be better?
	float filtered_power = std::norm(filtered);
	m_pilot_level = m_smoothingFactor * m_pilot_level + (1 - m_smoothingFactor) * filtered_power;

	// step pll
	nco_crcf_pll_step(stereoPilot, phase_error);
	nco_crcf_step(stereoPilot);

	// 38khz down-mix
	nco_crcf_mix_down(stereoPilot, converted, &y);
	nco_crcf_mix_down(stereoPilot, y, &converted);

	// complex -> real
	float usb_discard;
	firhilbf_c2r_execute(firStereoC2R, converted, &output[i], &usb_discard);
  }
}




