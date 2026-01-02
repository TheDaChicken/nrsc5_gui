//
// Created by TheDaChicken on 8/27/2025.
//

#ifndef RADIOINPUT_H
#define RADIOINPUT_H

#include "../hybrid/HybridDecoder.h"
#include "utils/Frame.h"

enum InputStatus
{
	INPUT_OK,
	INPUT_UNKNOWN,
};

class IRadioInput
{
	public:
		class ISDRControl
		{
			public:
				virtual ~ISDRControl() = default;
				virtual std::vector<PortSDR::GainMode> GetGainModes() = 0;
				virtual PortSDR::GainMode GetGainMode() = 0;

				virtual std::vector<PortSDR::Gain> GetGainStages() = 0;
				virtual double GetGain(std::string_view name) = 0;

				virtual int SetGainMode(PortSDR::GainMode mode) = 0;
				virtual tl::expected<void, int> SetGain(std::string_view name, float gain) = 0;

				virtual bool SetFrequency(Band::Type, uint32_t freq) = 0;
				virtual uint32_t GetFrequency() = 0;
		};

		explicit IRadioInput()
		{
		}
		virtual ~IRadioInput() = default;

		IRadioInput(const IRadioInput &) = delete;
		IRadioInput &operator=(const IRadioInput &) = delete;

		//virtual SDRInfo GetDeviceInfo() = 0;
		virtual InputStatus Start() = 0;
		virtual InputStatus Stop() = 0;
		virtual bool IsStreaming() = 0;

		virtual bool SetCapabilities(const NRSC5::StreamSupported &supported) = 0;
		virtual NRSC5::StreamCapabilities GetCapabilities() const = 0;

		virtual ISDRControl *GetISDRControl()
		{
			return nullptr;
		}

		std::function<void(Band::Type band, uint32_t freq)> onFrequencyChanged;
		std::function<void(NRSC5::StreamSupported supported)> onCapabilityChanged;
		std::function<void(RadioFrame &)> onRadio;
};

class IRadioHost
{
	public:
		virtual ~IRadioHost() = default;
		virtual std::unique_ptr<IRadioInput> Open(int index) = 0;

		[[nodiscard]] virtual std::vector<PortSDR::Device> GetAvailableDevices() const = 0;
};

#endif //RADIOINPUT_H
