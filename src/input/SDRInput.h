//
// Created by TheDaChicken on 8/27/2025.
//

#ifndef SDRINPUT_H
#define SDRINPUT_H

#include "RadioInput.h"

class SDRInput final : public RadioInput
{
	public:
		bool Open(const std::shared_ptr<PortSDR::Device> &device);

		bool Start();
		bool Stop();

		uint32_t GetChannel() override;
		bool SetChannel(uint32_t freq) override;

		void SetListener(const PortSDR::Stream::SDR_CALLBACK &listener) override;

		tl::expected<void, int> SetSampleFormat(PortSDR::SampleFormat sample_format);
		tl::expected<void, int> SetSampleRate(uint32_t sample_rate);

		NRSC5::StreamCapabilities GetCapabilities() override;

		std::vector<std::string> GetGainModes() override;
		std::string GetGainMode() override;

		std::vector<PortSDR::Gain> GetGainStages() override;
		PortSDR::Gain GetGainStage() override;

		double GetGain(std::string_view name) override;

		tl::expected<void, int> SetGainMode(const std::string &mode) override;
		tl::expected<void, int> SetGain(std::string_view name, float gain) override;

	private:
		std::shared_ptr<PortSDR::Device> device_;
		std::unique_ptr<PortSDR::Stream> sdr_stream_;
};

#endif //SDRINPUT_H
