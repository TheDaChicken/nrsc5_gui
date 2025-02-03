//
// Created by TheDaChicken on 7/28/2023.
//

#ifndef NRSC5_GUI_FMPILOTPHASELOCK_H
#define NRSC5_GUI_FMPILOTPHASELOCK_H

#include <vector>
#include <complex>

#include "utils/Types.h"

#include <liquid/liquid.h>

class FMPilotPhaseLock {
public:
    FMPilotPhaseLock(uint32_t d_input_rate, float min_freq, float max_freq);

    [[nodiscard]] bool IsFound() const;

    void Process(const vector_float_t& input, vector_float_t& output);
private:
    std::vector<std::complex<float>> d_tone_taps; /*! Tone BPF taps. */

    firhilbf firStereoR2C;
    firhilbf firStereoC2R;
    firfilt_cccf firFilter;
    nco_crcf stereoPilot;
    float m_pilot_level;
    float m_smoothingFactor;
};

#endif //NRSC5_GUI_FMPILOTPHASELOCK_H
