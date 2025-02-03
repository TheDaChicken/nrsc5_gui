//
// Created by TheDaChicken on 7/15/2023.
//

#ifndef NRSC5_GUI_FMDEMOD_H
#define NRSC5_GUI_FMDEMOD_H

#include <memory>

#include "fm/FMStereo.h"

#include "dsp/Resampler.h"
#include "dsp/QuadratureDemod.h"
#include "dsp/BandpassFilter.h"

class FMDemod
{
 public:
  FMDemod();

  void Setup(double iqSampleRate, int audioSampleRate, uint32_t inputBufferSize);
  void Reset();

  void Process(const vector_complex_t &input, vector_float_t &output);
 private:
  std::unique_ptr<ResamplerCC> m_converter;
  std::unique_ptr<BandpassFilter> m_filter;
  std::unique_ptr<QuadratureDemod> m_demoder;
  std::unique_ptr<FMStereo> m_fmStereo;

  vector_complex_t m_downBuf;
  vector_complex_t m_filterBuf;
  vector_float_t m_mpxBuf;
};

#endif //NRSC5_GUI_FMDEMOD_H
