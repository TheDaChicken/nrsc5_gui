#include "FMDemod.h"
#include "utils/Log.h"

#define FM_QUAD_RATE   240e3f // Nominal channel spacing is 200 kHz

FMDemod::FMDemod()
{
  /* filter for safeguard */
  m_filter = std::make_unique<BandpassFilter>(FM_QUAD_RATE, -80000.0, 80000.0, 20000.0);
  /* demod to real numbers */
  m_demoder = std::make_unique<QuadratureDemod>(FM_QUAD_RATE, 75000.0);
}

void FMDemod::Setup(double iqSampleRate, int audioSampleRate, uint32_t inputBufferSize)
{
  /* resample to a bandwidth of 120-+ khz */
  m_converter = std::make_unique<ResamplerCC>(FM_QUAD_RATE / (float)iqSampleRate);

  const int resampleOut = m_converter->GetOutputSamples(inputBufferSize);

  m_fmStereo = std::make_unique<FMStereo>(FM_QUAD_RATE,
										  (float)audioSampleRate /* audio sample rate */,
										  FMStereo::DeEmphasis::US75U,
										  resampleOut /* buffer */,
										  true        /* stereo enabled */
  );

  m_downBuf.Reserve(resampleOut);
  m_filterBuf.Reserve(resampleOut);
  m_mpxBuf.Reserve(resampleOut);
}

void FMDemod::Reset()
{
  m_converter->Reset();
  m_filter->Reset();
}

void FMDemod::Process(const vector_complex_t &input, vector_float_t &output)
{
  m_converter->Process(input, m_downBuf);
  m_filter->Process(m_downBuf, m_filterBuf);
  m_demoder->Process(m_filterBuf, m_mpxBuf);
  m_fmStereo->Process(m_mpxBuf, output);
}
