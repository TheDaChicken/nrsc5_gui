//
// Created by TheDaChicken on 8/27/2025.
//

#ifndef RADIOINPUT_H
#define RADIOINPUT_H

#include "nrsc5/Processor.h"

class RadioInput
{
	public:
		virtual ~RadioInput() = default;

		virtual bool Start() = 0;
		virtual bool Stop() = 0;

		virtual NRSC5::StreamCapabilities GetCapabilities() = 0;
		virtual uint32_t GetChannel() = 0;

		virtual bool SetChannel(uint32_t freq) = 0;
		virtual void SetListener(const PortSDR::Stream::SDR_CALLBACK &listener) = 0;
		virtual tl::expected<void, int> SetSampleFormat(PortSDR::SampleFormat sample_format) = 0;
		virtual tl::expected<void, int> SetSampleRate(uint32_t sample_rate) = 0;

		virtual tl::expected<void, int> SetGainMode(const std::string &mode) = 0;
		virtual tl::expected<void, int> SetGain(std::string_view name, float gain) = 0;

		virtual std::vector<std::string> GetGainModes() = 0;
		virtual std::string GetGainMode() = 0;

		virtual double GetGain(std::string_view name) = 0;

		virtual std::vector<PortSDR::Gain> GetGainStages() = 0;
		virtual PortSDR::Gain GetGainStage() = 0;

	private:
};

#endif //RADIOINPUT_H
