//
// Created by TheDaChicken on 8/2/2024.
//
#include <gtest/gtest.h>
#include <QPropertyAnimation>

#include "HybridRadio.h"
#include "utils/Log.h"
#include "audio/PortAudioCpp.h"
#include "Sdr.h"

class Delegate : public HybridRadio::Delegate
{
  void SetAudioStream(const PortAudio::StreamOutputPush &stream) override
  {

  }

  void HDSyncUpdate(bool sync) override
  {

  }
  void RadioStationUpdate(const RadioChannel &channel) override
  {

  }
  void HDID3Update(const NRSC5::ID3 &id3) override
  {

  }
  void HDReceivedLot(const std::shared_ptr<NRSC5::DataService> &component, const NRSC5::Lot &lot) override
  {

  }
};

TEST(HybridTest, Open)
{
  Delegate delegate;
  HybridRadio test(&delegate);
  test.Open();
}

TEST(HybridTest, Devices)
{
  Delegate delegate;
  HybridRadio radio(&delegate);

  ASSERT_EQ(radio.Open(), 0);

  SDR::System::GetInstance().Initialize();
  PortAudio::System::GetInstance().Initialize();

  std::shared_ptr<SDR::Device> sdrDevice;
  std::shared_ptr<PortAudio::Device> defaultDevice;

  /* select sdr if not chosen */
  ASSERT_EQ(SDR::System::GetInstance().GetFirstAvailableSDR(sdrDevice), 0);
  ASSERT_EQ(radio.SetSDRDevice(sdrDevice), 0);
  ASSERT_EQ(PortAudio::System::GetInstance().DefaultOutputDevice(defaultDevice), 0);
  ASSERT_EQ(radio.SetAudioDevice(defaultDevice), 0);

  ASSERT_EQ(radio.Start(), 0);
  ASSERT_EQ(radio.Stop(), 0);
}
