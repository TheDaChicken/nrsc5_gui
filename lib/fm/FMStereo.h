//
// Created by TheDaChicken on 7/28/2023.
//

#ifndef NRSC5_GUI_FMSTEREO_H
#define NRSC5_GUI_FMSTEREO_H

#include "FMDeemph.h"
#include "FMPilotPhaseLock.h"

#include "dsp/LowpassFilter.h"
#include "dsp/Resampler.h"
#include "utils/DataBuffer.h"

#include "utils/Types.h"

class FMStereo
{
 public:
  enum DeEmphasis
  {
	US75U,
	EU50U
  };

  FMStereo(float input_rate, float audio_rate,
		   enum DeEmphasis option, int inputBufferSize, bool stereo = true);

  void SetStereo(bool stereo);
  void SetDeEmphasis(enum DeEmphasis option);

  void Process(const vector_float_t &input, vector_float_t &output);

  static double GetTau(enum DeEmphasis option);
  static void StereoToLeftRight(const vector_float_t &monoBuf, const vector_float_t &stereoBuf,
								vector_float_t &out);
  static void MonoToLeftRight(const vector_float_t &lrBuf,
							  vector_float_t &out);
 private:
  std::unique_ptr<LowpassFilterFF> m_lrFilter;
  std::unique_ptr<ResamplerFF> m_lrResample;
  std::unique_ptr<FMDeemph> m_lrDeemph;

  std::unique_ptr<FMPilotPhaseLock> m_pilotLock;

  std::unique_ptr<LowpassFilterFF> m_lrMinusFilter;
  std::unique_ptr<ResamplerFF> m_lrMinusResample;
  std::unique_ptr<FMDeemph> m_lrMinusDeemph;

  vector_float_t m_lrFilterBuf;
  vector_float_t m_lrEmph;
  vector_float_t m_lrAudioBuf;

  vector_float_t m_stereoBaseband;

  vector_float_t m_lrMinusFilterBuf;
  vector_float_t m_lrMinusEmph;
  vector_float_t m_lrMinusAudioBuf;

  bool m_stereo = false;
};

#endif //NRSC5_GUI_FMSTEREO_H
