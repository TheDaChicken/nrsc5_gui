//
// Created by TheDaChicken on 7/28/2023.
//

#ifndef NRSC5_GUI_FMDEEMPH_H
#define NRSC5_GUI_FMDEEMPH_H

#include <vector>
#include <memory>

#include <liquid/liquid.h>

#include "dsp/filter/IirFilter.h"
#include "utils/Types.h"

class FMDeemph {
public:
    FMDeemph(float audio_rate, double tau);
    ~FMDeemph();

    void SetTau(double tau);

    void Process(const vector_float_t &input, vector_float_t &output);
private:
    std::unique_ptr<IirFilter<float, float, double, double>> d_deemph;

    float m_audio_rate;

    /* De-emph IIR filter taps */
    std::vector<double> d_fftaps;  /*! Feed forward taps. */
    std::vector<double> d_fbtaps;  /*! Feed back taps. */

    void CalculateIIRTaps(double tau);
};

#endif //NRSC5_GUI_FMDEEMPH_H
