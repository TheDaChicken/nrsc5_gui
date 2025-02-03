//
// Created by TheDaChicken on 7/20/2023.
//

#ifndef NRSC5_GUI_BANDPASSFILTER_H
#define NRSC5_GUI_BANDPASSFILTER_H

#include <vector>
#include <complex>

#include <liquid/liquid.h>

#include "utils/Types.h"

class BandpassFilter {
public:
    BandpassFilter(double sample_rate, double low, double high, double trans_width);
    ~BandpassFilter();
    void Close();

    void Reset();

    void SetParam(double low, double high, double trans_width);

    void Process(const vector_complex_t& input, vector_complex_t& output);
private:
    std::vector<std::complex<float>> d_taps;
    firfilt_cccf m_filter;

    double d_sample_rate;
    double d_low;
    double d_high;
    double d_trans_width;
};

#endif //NRSC5_GUI_BANDPASSFILTER_H
