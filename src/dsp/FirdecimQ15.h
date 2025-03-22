//
// Created by TheDaChicken on 6/19/2024.
//

#ifndef NRSC5_GUI_SRC_LIB_DSP_FIRDECIM_Q15_H_
#define NRSC5_GUI_SRC_LIB_DSP_FIRDECIM_Q15_H_

#include "utils/Types.h"
#include <cstdint>

class Halfband_16
{
 public:
  explicit Halfband_16(const std::vector<int16_t> &taps);

  void reset();

  void execute(const cint16_t *x, cint16_t *y);
 private:
  void push(cint16_t x);

  std::vector<cint16_t> m_window;
  std::vector<int16_t> m_taps;
  unsigned int ntaps;
  unsigned int idx;
};

#endif //NRSC5_GUI_SRC_LIB_DSP_FIRDECIM_Q15_H_
