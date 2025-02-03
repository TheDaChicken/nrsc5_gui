//
// Created by TheDaChicken on 7/21/2023.
//

#include "LowpassFilter.h"

#include "dsp/filter/Firdes.h"

LowpassFilterFF::LowpassFilterFF(double sample_rate, double cutoff_freq, double trans_width, double gain)
    : d_sample_rate(sample_rate)
    , d_gain(gain)
    , d_cutoff_freq(0)
    , d_trans_width(0)
    , filter(nullptr)
{
    SetParam(cutoff_freq, trans_width);
}

LowpassFilterFF::~LowpassFilterFF() {
    Close();
}

void LowpassFilterFF::Close() {
    if(filter)
        firfilt_rrrf_destroy(filter);
    filter = nullptr;
}

void LowpassFilterFF::SetParam(double cutoff_freq, double trans_width) {
    d_cutoff_freq = cutoff_freq;
    d_trans_width = trans_width;

    /* generate new taps */
    d_taps = Firdes::low_pass(d_gain, d_sample_rate,
                              d_cutoff_freq, d_trans_width);

    Close();
    filter = firfilt_rrrf_create(d_taps.data(), d_taps.size());
}

void LowpassFilterFF::Process(const vector_float_t& input, vector_float_t& output) {
    output.Resize(input.Size());

    firfilt_rrrf_execute_block(filter, const_cast<float*>(input.Data()), input.Size(), output.Data());
}
