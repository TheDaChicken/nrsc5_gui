//
// Created by TheDaChicken on 7/10/2023.
//

#include "Decoder.h"
#include "utils/Log.h"

extern "C" {
#include <nrsc5.h>
}

std::string_view NRSC5::Decoder::VersionText()
{
  const char *version;
  nrsc5_get_version(&version);
  return version;
}
std::string_view NRSC5::Decoder::ServiceTypeName(unsigned int type)
{
  const char *name;
  nrsc5_service_data_type_name(type, &name);
  return name;
}

std::string_view NRSC5::Decoder::ProgramTypeName(unsigned int type)
{
  const char *text;
  nrsc5_program_type_name(type, &text);
  return text;
}

NRSC5::Decoder::~Decoder()
{
  if (!nrsc_5_)
    return;

  Stop();
  Close();
}

int NRSC5::Decoder::Open()
{
  return nrsc5_open(&nrsc_5_, 0);
}

int NRSC5::Decoder::OpenPipe()
{
  return nrsc5_open_pipe(&nrsc_5_);
}

void NRSC5::Decoder::Close()
{
  nrsc5_close(nrsc_5_);
  nrsc_5_ = nullptr;
}

void NRSC5::Decoder::Start()
{
  if (!nrsc_5_)
    return;

  nrsc5_start(nrsc_5_);
}

void NRSC5::Decoder::Stop()
{
  if (!nrsc_5_)
    return;

  nrsc5_stop(nrsc_5_);
}

int NRSC5::Decoder::SetFrequency(float freq_hz)
{
  if (!nrsc_5_)
    return -1;

  return nrsc5_set_frequency(nrsc_5_, freq_hz);
}

void NRSC5::Decoder::SetCallback(nrsc5_callback_t callback, void *opaque)
{
  if (!nrsc_5_)
    return;

  nrsc5_set_callback(nrsc_5_, callback, opaque);
}

void NRSC5::Decoder::SetMode(int mode)
{
  nrsc5_set_mode(nrsc_5_, mode);
}

int NRSC5::Decoder::SendIQ(const uint8_t *buf, unsigned int length)
{
  if (!nrsc_5_)
    return -1;

  return nrsc5_pipe_samples_cu8(nrsc_5_, buf, length);
}

int NRSC5::Decoder::SendIQ(const int16_t *buf, unsigned int length)
{
  if (!nrsc_5_)
    return -1;

  return nrsc5_pipe_samples_cs16(nrsc_5_, buf, length);
}
