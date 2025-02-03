/*
 * iqbal.c
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

#include "IQBalance.h"
#include "utils/Log.h"

#include <cmath>
#include <complex>
#include <cassert>

IQBalance::IQBalance()
    : m_phase(0.0f), m_mag(0.0f), d_first(true), m_cache(std::make_unique<IQCache>()) {
  CreateCache(m_cache.get());
}

void IQBalance::CreateCache(IQCache *cache) {
  if (cache->m_fftPlan)
    return;
  cache->fft_size = 1204;
  cache->fft_count = 8;
  cache->m_fft = fftwf_alloc_complex(cache->fft_size);
  cache->m_fftPlan = fftwf_plan_dft_1d(cache->fft_size, cache->m_fft,
                                       cache->m_fft, FFTW_FORWARD, FFTW_MEASURE);
}

/*! \brief Apply IQ balance correction to a given complex buffer
 *  \param[out] out Complex output buffer
 *  \param[in] in Complex input buffer
 *  \param[in] mag Magnitude correction (approximated)
 *  \param[in] phase Phase correction (approximated)
 *
 *  The input and output buffers can be the same for in-place modification.
 *
 *  The applied transform is out[i] = (a * (1 + mag)) + (b + phase * a) * i
 *  (with in[i] = a+bi).
 */
void IQBalance::ApplyIQBalance(const std::vector<std::complex<float>> &in, std::vector<std::complex<float>> &out,
                               float mag, float phase) /* osmo_iqbal_fix */
{
  const float magp1 = 1.0f + mag;

  for (unsigned int i = 0; i < in.size(); i++) {
    std::complex<float> v = in[i];
    out[i] = std::complex<float>(
        (v.real() * magp1),
        v.imag() + phase * v.real()
    );
  }
}

/*! \brief Optimization objective function - Value
 *  \param[in] cache Current state object of optimization loop
 *  \returns The value of the objective function at point 'x'
 */
float IQBalance::ApplyEstimate(IQCache *cache, std::vector<std::complex<float>> &in,
                               float mag, float phase) /* _iqbal_objfn_value */
{
  ApplyIQBalance(in, in, mag, phase);
  return EstimateIQBalance(cache, in, cache->fft_size,
                           cache->fft_count);
}

/*! \brief Optimization objective function - Gradient estimation
 *  \param[in] state Current state object of optimization loop
 *  \param[in] mag
 *  \param[in] phase
 *  \param[in] v The value of the objective function at point 'x'
 *  \param[out] grad An array of 2 float for the estimated gradient at point 'x'
 */
void IQBalance::EstimateGradient(IQCache *cache, std::vector<std::complex<float>> &in,
                                 float mag, float phase, float v, float grad[2]) /* _iqbal_objfn_gradient */
{
  const float GRAD_STEP = 1e-6f;
  float new_phase;
  float new_mag;

  new_mag = ApplyEstimate(cache, in, mag + GRAD_STEP, phase);
  new_phase = ApplyEstimate(cache, in, mag, phase + GRAD_STEP);

  grad[0] = (new_mag - v) / GRAD_STEP;
  grad[1] = (new_phase - v) / GRAD_STEP;
}

/*! \brief Optimization objective function - Value & Gradient estimation
 *  \param[in] state Current state object of optimization loop
 *  \param[in] x An array of 2 float for (mag,phase) point to evaluate at
 *  \param[out] grad An array of 2 float for the estimated gradient at point 'x'
 *  \returns The value of the objective function at point 'x'
 */
float IQBalance::ApplyGradient(IQCache *cache, std::vector<std::complex<float>> &input,
                               float mag, float phase, float grad[2]) /* _iqbal_objfn_val_gradient */
{
  float v = ApplyEstimate(cache, input, mag, phase);
  EstimateGradient(cache, input, mag, phase, v, grad);
  return v;
}

/*! \brief Objectively estimate IQ balance in a given complex buffer
 *  \param[in] in Input complex buffer (at least fft_size * fft_count samples)
 *  \param[in] fft_size Size of the FFT to use internally
 *  \param[in] fft_count The number of consecutive FFT to use internally
 *  \param[out] cache Cache object for multiple calls
 *  \returns A number >= 0.0f estimating the IQ balance (the lower, the better)
 *
 *  The Cache object should only be used for multiple calls with the same parameters
 *  and the same size of input vector. Once you don't plan on using it anymore,
 *  you should call \ref _osmo_iqbal_estimate_release . The initial pointer value
 *  should also be initialized to NULL.
 */
float IQBalance::EstimateIQBalance(IQCache *cache, const std::vector<std::complex<float>> &in,
                                   int fft_size, int fft_count) { /* _osmo_iqbal_estimate */
  float est = 0.0f;
  auto *fftOut = reinterpret_cast<std::complex<float> *>(cache->m_fft);

  for (unsigned int i = 0; i < fft_count; i++) {
    std::complex<float> corr = 0.0f;

    memcpy(cache->m_fft, &in[i * fft_size], sizeof(std::complex<float>) * fft_size);
    fftwf_execute(cache->m_fftPlan);

    for (unsigned int j = 1; j < fft_size / 2; j++)
      corr += fftOut[fft_size - j] * std::conj(fftOut[j]);

    est += std::norm(corr); /* / (fft_size / 2); */
  }

  return est;
}

/*! \brief Finds the best IQ balance correction parameters for a given signal
 *  \param[in] in The input signal to optimize for
 *
 *  The mag and phase parameters are pointers to float. If in the options,
 *  the 'start_at_prev' is enabled, the initial values of those will be used
 *  and so they should be initialized appropriately.
 */
void IQBalance::FindBestIQBalance(IQCache *cache,
                                  std::vector<std::complex<float>> in,
                                  float &mag,
                                  float &phase) { /* osmo_iqbal_cxvec_optimize */
  float cv, nv, step;
  float cgrad[2];
  float p;

  assert(cache != nullptr);

  if (in.size() < (cache->fft_size * cache->fft_count))
    throw std::runtime_error("Invalid parma");

  cv = ApplyGradient(cache, in, mag, phase, cgrad);
  step = cv / (std::fabs(cgrad[0]) + std::fabs(cgrad[1]));

  for (unsigned i = 0; i < cache->max_iter; i++) {
    float new_mag = mag - step * (cgrad[0] / (std::fabs(cgrad[0]) + std::fabs(cgrad[1])));
    float new_phase = phase - step * (cgrad[1] / (std::fabs(cgrad[0]) + std::fabs(cgrad[1])));

    nv = ApplyEstimate(cache, in, new_mag, new_phase);

    if (nv <= cv) {
      p = (cv - nv) / cv;

      mag = new_mag;
      phase = new_phase;
      cv = nv;
      EstimateGradient(cache, in, mag, phase, cv, cgrad);

      if (p < 0.01f)
        break;
    } else {
      step /= 2.0f * (nv / cv);
    }
  }
}

void IQBalance::Process(const std::vector<std::complex<float>> &in, std::vector<std::complex<float>> &output) {
  float p_mag = 0;
  float p_phase = 0;

  FindBestIQBalance(m_cache.get(), in, p_mag, p_phase);

  if (this->d_first) {
    this->d_first = false;
    this->m_mag = p_mag;
    this->m_phase = p_phase;
  } else {
    this->m_mag = (0.95f * this->m_mag) + (p_mag * 0.05f);
    this->m_phase = (0.95f * this->m_phase) + (p_phase * 0.05f);
  }

  ApplyIQBalance(in, output, m_mag, m_phase);
}

