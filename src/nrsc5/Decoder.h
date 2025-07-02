//
// Created by TheDaChicken on 7/10/2023.
//

#ifndef NRSC5_GUI_DECODER_H
#define NRSC5_GUI_DECODER_H

#include <functional>
#include <complex>
#include <cstdint>

extern "C"
{
#include <nrsc5.h>
}

#define NRSC5_CHANNEL_TIMEOUT 15
#define NRSC5_MPS_PROGRAM 0x0

namespace NRSC5
{
class Decoder
{
 public:
  ~Decoder();

  static std::string_view VersionText();
  static std::string_view ServiceTypeName(unsigned int type);
  static std::string_view ProgramTypeName(unsigned int type);

  /**
   * @brief Opens decoder for an RTLSDR device using built-in driver
   */
  int Open();

  /**
   * @brief Opens decoder for IQ samples
   */
  int OpenPipe();

  void Close();

  void Start();
  void Stop();

  /**
   * @brief Sets the frequency on the built-in driver.
   * @param freq_hz The frequency in hertz.
   */
  int SetFrequency(float freq_hz);

  /**
   * @brief Sets the callback for the decoder.
   * @param callback The callback function.
   * @param opaque The opaque pointer.
   */
  void SetCallback(nrsc5_callback_t callback, void *opaque);

  /**
   * @brief Sets the mode for the decoder.
   * @param mode FM or AM mode.
   */
  void SetMode(int mode);

  /**
   * @brief Sends IQ data to the decoder.
   * @param buf The buffer containing the IQ data.
   * @param length The length of the buffer.
   */
  int SendIQ(const int16_t *buf, unsigned int length);
  int SendIQ(const uint8_t *buf, unsigned int length);
 private:
  nrsc5_t *nrsc_5_{nullptr};
};
}

#endif //NRSC5_GUI_DECODER_H
