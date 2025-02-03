//
// Created by TheDaChicken on 7/18/2023.
//

#ifndef NRSC5_GUI_RESAMPLER_H
#define NRSC5_GUI_RESAMPLER_H

#include <vector>
#include <memory>
#include <complex>

#include <liquid/liquid.h>

#include "utils/Types.h"

class ResamplerCC {
public:
    explicit ResamplerCC(float rate, float suppression = 100.0f);
    ~ResamplerCC();

    void SetRate(float rate, float suppression = 100.0f);
    void Reset();

    void Process(const vector_complex_t& input, vector_complex_t& output);

    [[nodiscard]] int GetOutputSamples(std::size_t inputSize) const;
private:
    msresamp_crcf m_resampler;
    float m_rate;
};

class ResamplerFF {
public:
    explicit ResamplerFF(float rate);

    void SetRate(float rate);
    void Reset();

    void Process(const vector_float_t& input, vector_float_t& output);

    [[nodiscard]] int GetOutputSamples(std::size_t inputSize) const;
private:
    msresamp_rrrf m_resampler;
    float m_rate;
};

#endif //NRSC5_GUI_RESAMPLER_H
