//
// Created by TheDaChicken on 11/22/2025.
//

#include <gmock/gmock-function-mocker.h>
#include <gtest/gtest.h>

#include "gui/controllers/SDRController.h"
#include "input/RadioInput.h"
#include "utils/Log.h"

class MockRadioInput final : public IRadioInput
{
		MockRadioInput()
		{
		}

		MOCK_METHOD(InputStatus, Start, (), (override));
		MOCK_METHOD(InputStatus, Stop, (), (override));
		MOCK_METHOD(bool, IsStreaming, (), (override));
		MOCK_METHOD(bool, SetCapabilities, (const NRSC5::StreamSupported &supported), (override));
		MOCK_METHOD(NRSC5::StreamCapabilities, GetCapabilities, (), (const override));
};

TEST(RadioInput, Init)
{
	//std::unique_ptr<IRadioInput> input = std::make_unique<MockRadioInput>();

	//RadioController radio;
	//radio.Open(std::move(input));
}

TEST(RadioTest, Init)
{
	SDRHost host;

	host.UpdateDeviceList();
	auto devices = host.GetDeviceList();

	auto test = host.CreateSession(devices[0]);
	auto test2 = host.CreateSession(devices[0]);

	test->Start();
	test->Stop();

	ASSERT_EQ(test2, nullptr);
}