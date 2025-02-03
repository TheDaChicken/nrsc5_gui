//
// Created by TheDaChicken on 7/17/2023.
//

#include "FFTProcessor.h"
#include "utils/Log.h"
#include "dsp/filter/Window.h"

FFTProcessorComplex::FFTProcessorComplex(int fftSize, bool normalize_energy)
	: m_fftSize(fftSize), m_normalize_energy(normalize_energy),
	  m_fftIn(nullptr), m_fftOut(nullptr), m_fftPlan(nullptr)
{
  CreateFFTW();
  CreateWindows();

  assert(sizeof(fftwf_complex) == sizeof(std::complex<float>));
}

void FFTProcessorComplex::CreateFFTW()
{
  m_fftIn = fftwf_alloc_complex(m_fftSize);
  m_fftOut = fftwf_alloc_complex(m_fftSize);
  m_fftPlan = fftwf_plan_dft_1d(m_fftSize, m_fftIn, m_fftOut,
								FFTW_FORWARD, FFTW_MEASURE);
  if (!m_fftPlan)
	throw std::runtime_error("Failed to create fftw plan");
}

void FFTProcessorComplex::CreateWindows()
{
  d_window.clear();
  d_window = Window::build(Window::win_type::WIN_HAMMING, m_fftSize, 6.76);
  d_window.resize(m_fftSize);

  // Normalize using average of window for amplitude, or RMS for energy
  float sum = 0.0;
  for (auto v : d_window)
	sum += m_normalize_energy ? v * v : v;

  const float factor = (m_normalize_energy) ? std::sqrt(sum / m_fftSize) : sum / m_fftSize;
  const float inv = 1.0f / factor;

  for (int i = 0; i < m_fftSize; i++)
  {
	d_window[i] *= inv;
  }
}

void FFTProcessorComplex::ApplyWindow(int samples)
{
  if (!d_window.empty())
  {
	auto *in = reinterpret_cast<std::complex<float> *>(m_fftIn);
	for (int i = 0; i < samples; i++)
	{
	  std::complex<float> temp = in[i];
	  in[i] = temp * d_window[i];
	}
  }
}

FFTProcessorComplex::~FFTProcessorComplex()
{
  fftwf_free(m_fftIn);
  fftwf_free(m_fftOut);
  fftwf_destroy_plan(m_fftPlan);
}

fftwf_complex *FFTProcessorComplex::GetIn()
{
  return m_fftIn;
}

std::complex<float> *FFTProcessorComplex::Process()
{
  ApplyWindow(m_fftSize);
  fftwf_execute(m_fftPlan);
  return reinterpret_cast<std::complex<float> *>(m_fftOut);
}

std::complex<float> *FFTProcessorComplex::Process(const vector_complex_t &input)
{
  assert(input.size() <= m_fftSize);
  memcpy(m_fftIn, input.data(), input.size() * sizeof(fftwf_complex));
  return Process();
}


