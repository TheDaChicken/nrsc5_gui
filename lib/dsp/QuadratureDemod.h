//
// Created by TheDaChicken on 7/15/2023.
//

#ifndef NRSC5_GUI_QUADRATUREDEMOD_H
#define NRSC5_GUI_QUADRATUREDEMOD_H

#include <cstdint>
#include <vector>
#include <complex>

#include "utils/Types.h"

class QuadratureDemod {
public:
    explicit QuadratureDemod(int32_t sample_rate, float max_freq_dev);

    void Process(const vector_complex_t& input, vector_float_t& output);
private:
    float m_gain;
    std::complex<float> m_ref;
};


#endif //NRSC5_GUI_QUADRATUREDEMOD_H
