//
// Created by TheDaChicken on 7/21/2023.
//

#ifndef NRSC5_GUI_LOWPASSFILTER_H
#define NRSC5_GUI_LOWPASSFILTER_H

#include <vector>
#include <liquid/liquid.h>
#include <memory>

#include "utils/Types.h"

class LowpassFilterFF {
public:
    LowpassFilterFF(double sample_rate, double cutoff_freq,
                    double trans_width, double gain = 1.0);
    ~LowpassFilterFF();
    void Close();

    void SetParam(double cutoff_freq, double trans_width);

    void Process(const vector_float_t& input, vector_float_t& output);
private:
    std::vector<float> d_taps;
    firfilt_rrrf filter;

    double d_sample_rate;
    double d_cutoff_freq;
    double d_trans_width;
    double d_gain;
};

#endif //NRSC5_GUI_LOWPASSFILTER_H
