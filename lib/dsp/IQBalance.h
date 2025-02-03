/*
 * iqbal.h
 *
 * IQ balance correction / estimation utilities
 *
 * Copyright (C) 2013  Sylvain Munaut <tnt@246tNt.com>
 *
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef NRSC5_GUI_IQBALANCE_H
#define NRSC5_GUI_IQBALANCE_H

#include <complex>
#include <vector>
#include <memory>

#include <fftw3.h>

struct IQCache {
  fftwf_plan m_fftPlan{nullptr};
  fftwf_complex *m_fft{nullptr};

  int fft_size{1204};
  int fft_count{8};
  int max_iter{25};
};

class IQBalance {
 public:
  IQBalance();

  static float EstimateIQBalance(IQCache *cache, const std::vector<std::complex<float>> &in,
                                 int fft_size, int fft_count);
  static void FindBestIQBalance(IQCache *cache, std::vector<std::complex<float>> in, float &mag, float &phase);

  void Process(const std::vector<std::complex<float>> &in, std::vector<std::complex<float>> &output);
 private:
  float m_mag;
  float m_phase;
  bool d_first;
  std::unique_ptr<IQCache> m_cache;

  static void CreateCache(IQCache *cache);
  static void EstimateGradient(IQCache *cache,
                               std::vector<std::complex<float>> &in,
                               float mag,
                               float phase,
                               float v,
                               float *grad);
  static float ApplyGradient(IQCache *cache,
                             std::vector<std::complex<float>> &input,
                             float mag,
                             float phase,
                             float *grad);
  static void ApplyIQBalance(const std::vector<std::complex<float>> &in,
                             std::vector<std::complex<float>> &out,
                             float mag,
                             float phase);
  static float ApplyEstimate(IQCache *cache, std::vector<std::complex<float>> &in,
                             float mag, float phase);
};

#endif //NRSC5_GUI_IQBALANCE_H
