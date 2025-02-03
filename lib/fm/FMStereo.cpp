//
// Created by TheDaChicken on 7/28/2023.
//

#include "FMStereo.h"
#include "utils/Log.h"

FMStereo::FMStereo(float input_rate, float audio_rate,
				   enum DeEmphasis option, int inputBufferSize, bool stereo)
	: m_stereo(stereo)
{
  /* l + r signal filter */
  m_lrFilter = std::make_unique<LowpassFilterFF>(input_rate, 17e3, 2e3);
  m_lrResample = std::make_unique<ResamplerFF>(audio_rate / input_rate);
  m_lrDeemph = std::make_unique<FMDeemph>(audio_rate, GetTau(option));

  m_lrFilterBuf.Reserve(inputBufferSize);
  m_lrMinusFilterBuf.Reserve(inputBufferSize);
  m_stereoBaseband.Reserve(inputBufferSize);

  const int resampleOut = m_lrResample->GetOutputSamples(inputBufferSize);
  /* chain after resample */
  m_lrEmph.Reserve(resampleOut);
  m_lrAudioBuf.Reserve(resampleOut);

  /* l - r signal filter */
  m_lrMinusFilter = std::make_unique<LowpassFilterFF>(input_rate, 17e3, 2e3);
  m_lrMinusResample = std::make_unique<ResamplerFF>(audio_rate / input_rate);
  m_lrMinusDeemph = std::make_unique<FMDeemph>(audio_rate, GetTau(option));

  m_pilotLock = std::make_unique<FMPilotPhaseLock>(input_rate, 18980, 19020);

  /* l - r */
  m_lrMinusEmph.Reserve(resampleOut);
  m_lrMinusAudioBuf.Reserve(resampleOut);
}

double FMStereo::GetTau(enum DeEmphasis option)
{
  switch (option)
  {
	default:
	case EU50U:return 50e-6;
	case US75U:return 75e-6;
  }
}

void FMStereo::SetDeEmphasis(enum DeEmphasis option)
{
  m_lrDeemph->SetTau(GetTau(option));
}

void FMStereo::SetStereo(bool stereo)
{
  m_stereo = stereo;
}

void FMStereo::Process(const vector_float_t &mpxInput, vector_float_t &output)
{
  /* L + R */
  m_lrFilter->Process(mpxInput, m_lrFilterBuf);
  m_lrResample->Process(m_lrFilterBuf, m_lrEmph);
  m_lrDeemph->Process(m_lrEmph, m_lrAudioBuf);

  if (m_stereo)
  {
	/* L - R */
	m_pilotLock->Process(mpxInput, m_stereoBaseband);
	if (!m_pilotLock->IsFound())
	{
	  /* TODO use ringbuffer maybe ?
	   * keep resample in same block-sizes */
	  m_lrMinusResample->Process(m_stereoBaseband, m_lrMinusEmph);

	  MonoToLeftRight(m_lrAudioBuf, output);
	  return;
	}

	m_lrMinusFilter->Process(m_stereoBaseband, m_lrMinusFilterBuf);
	m_lrMinusResample->Process(m_lrMinusFilterBuf, m_lrMinusEmph);
	m_lrMinusDeemph->Process(m_lrMinusEmph, m_lrMinusAudioBuf);

	StereoToLeftRight(m_lrAudioBuf, m_lrMinusAudioBuf, output);
  }
  else
  {
	MonoToLeftRight(m_lrAudioBuf, output);
  }
}

void FMStereo::StereoToLeftRight(const vector_float_t &monoBuf,
								 const vector_float_t &stereoBuf,
								 vector_float_t &out)
{
  assert(monoBuf.Size() == stereoBuf.Size());

  out.Resize(monoBuf.Size() * 2);
  for (unsigned int i = 0; i < monoBuf.Size(); i++)
  {
	float m = monoBuf[i];
	float s = stereoBuf[i];
	out[i * 2] = (m + s);
	out[i * 2 + 1] = (m - s);
  }
}

// Duplicate mono signal in left/right channels.
void FMStereo::MonoToLeftRight(const vector_float_t &monoBuf,
							   vector_float_t &out)
{
  out.Resize(monoBuf.Size() * 2);
  // fit mono in stereo
  for (size_t i = 0; i < monoBuf.Size(); ++i)
  {
	out[i * 2] = monoBuf[i];
	out[i * 2 + 1] = monoBuf[i];
  }
}


