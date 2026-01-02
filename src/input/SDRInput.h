//
// Created by TheDaChicken on 8/27/2025.
//

#ifndef SDRINPUT_H
#define SDRINPUT_H

#include "RadioInput.h"
#include "utils/RingBuffer.h"

class SDRInput final
		: public IRadioInput,
		  public IRadioInput::ISDRControl
{
	public:
		explicit SDRInput(
			std::unique_ptr<PortSDR::Stream> stream);

		//SDRInfo GetDeviceInfo() override;

		bool IsStreaming() override;

		InputStatus Start() override;
		InputStatus Stop() override;

		bool SetCapabilities(const NRSC5::StreamSupported &supported) override;
		NRSC5::StreamCapabilities GetCapabilities() const override;

		void ResetBuffer();

		tl::expected<void, int> SetSampleFormat(PortSDR::SampleFormat sample_format);
		tl::expected<void, int> SetSampleRate(uint32_t sample_rate);

		std::vector<PortSDR::GainMode> GetGainModes() override;
		PortSDR::GainMode GetGainMode() override;

		std::vector<PortSDR::Gain> GetGainStages() override;

		double GetGain(std::string_view name) override;

		int SetGainMode(const PortSDR::GainMode mode) override;
		tl::expected<void, int> SetGain(std::string_view name, float gain) override;

		uint32_t GetFrequency() override;
		bool SetFrequency(Band::Type band, uint32_t freq) override;

		IRadioInput::ISDRControl *GetISDRControl() override
		{
			return this;
		}

	private:
		bool running = false;
		std::unique_ptr<PortSDR::Stream> sdr_stream_;
};

// class SDRHost
// {
// 	public:
// 		SDRHost();
//
// 		tl::expected<std::unique_ptr<IRadioInput>, int> Open(const PortSDR::Device &device);
//
// 		std::vector<SDRInfo> GetDevices();
//
// 	private:
//
// };

#endif //SDRINPUT_H
