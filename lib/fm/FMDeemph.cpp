//
// Created by TheDaChicken on 7/28/2023.
//

#include "FMDeemph.h"

#include <cmath>

FMDeemph::FMDeemph(float audio_rate, double tau)
    : m_audio_rate(audio_rate)
{
    /* de-emphasis */
    d_fftaps.resize(2);
    d_fbtaps.resize(2);

    d_deemph = std::make_unique<IirFilter<float, float,
            double, double>>(d_fftaps, d_fbtaps, false);

    SetTau(tau);
}

FMDeemph::~FMDeemph() {

}

/*! \brief Set FM de-emphasis time constant.
 *  \param tau The new time constant.
 */
void FMDeemph::SetTau(double tau)
{
    CalculateIIRTaps(tau);

    d_deemph->set_taps(d_fftaps, d_fbtaps);
}

/*! \brief Calculate taps for FM de-emph IIR filter. */
void FMDeemph::CalculateIIRTaps(double tau)
{
    if (tau > 1.0e-9)
    {
        // copied from fm_emph.py in gr-analog
        double  w_c;    // Digital corner frequency
        double  w_ca;   // Prewarped analog corner frequency
        double  k, z1, p1, b0;
        double  fs = (double)m_audio_rate;

        w_c = 1.0 / tau;
        w_ca = 2.0 * fs * tan(w_c / (2.0 * fs));

        // Resulting digital pole, zero, and gain term from the bilinear
        // transformation of H(s) = w_ca / (s + w_ca) to
        // H(z) = b0 (1 - z1 z^-1)/(1 - p1 z^-1)
        k = -w_ca / (2.0 * fs);
        z1 = -1.0;
        p1 = (1.0 + k) / (1.0 - k);
        b0 = -k / (1.0 - k);

        d_fftaps[0] = b0;
        d_fftaps[1] = -z1 * b0;
        d_fbtaps[0] = 1.0;
        d_fbtaps[1] = -p1;
    }
    else
    {
        d_fftaps[0] = 1.0;
        d_fftaps[1] = 0.0;
        d_fbtaps[0] = 0.0;
        d_fbtaps[1] = 0.0;
    }
}

void FMDeemph::Process(const vector_float_t &input, vector_float_t &output) {
    output.Resize(input.Size());
    d_deemph->filter_n(output.Data(), input.Data(), (int)output.Size());
}

