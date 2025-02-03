//
// Created by TheDaChicken on 7/29/2023.
//

#include "IirFilter.h"

template <>
std::complex<float>
IirFilter<std::complex<float>, std::complex<float>, float, std::complex<float>>::filter(const std::complex<float> input)
{
    std::complex<float> acc;
    unsigned i = 0;
    unsigned n = ntaps_ff();
    unsigned m = ntaps_fb();

    if (n == 0)
        return (std::complex<float>)0;

    int latest_n = d_latest_n;
    int latest_m = d_latest_m;

    acc = d_fftaps[0] * input;
    for (i = 1; i < n; i++)
        acc += (d_fftaps[i] * d_prev_input[latest_n + i]);
    for (i = 1; i < m; i++)
        acc += (d_fbtaps[i] * d_prev_output[latest_m + i]);

    // store the values twice to avoid having to handle wrap-around in the loop
    d_prev_output[latest_m] = acc;
    d_prev_output[latest_m + m] = acc;
    d_prev_input[latest_n] = input;
    d_prev_input[latest_n + n] = input;

    latest_n--;
    latest_m--;
    if (latest_n < 0)
        latest_n += n;
    if (latest_m < 0)
        latest_m += m;

    d_latest_m = latest_m;
    d_latest_n = latest_n;
    return static_cast<std::complex<float>>(acc);
}

template <>
std::complex<float>
IirFilter<std::complex<float>, std::complex<float>, double, std::complex<double>>::filter(const std::complex<float> input)
{
    std::complex<double> acc;
    unsigned i = 0;
    unsigned n = ntaps_ff();
    unsigned m = ntaps_fb();

    if (n == 0)
        return (std::complex<float>)0;

    int latest_n = d_latest_n;
    int latest_m = d_latest_m;

    acc = d_fftaps[0] * static_cast<std::complex<double>>(input);
    for (i = 1; i < n; i++)
        acc += (d_fftaps[i] * static_cast<std::complex<double>>(d_prev_input[latest_n + i]));
    for (i = 1; i < m; i++)
        acc += (d_fbtaps[i] * static_cast<std::complex<double>>(d_prev_output[latest_m + i]));

    // store the values twice to avoid having to handle wrap-around in the loop
    d_prev_output[latest_m] = acc;
    d_prev_output[latest_m + m] = acc;
    d_prev_input[latest_n] = input;
    d_prev_input[latest_n + n] = input;

    latest_n--;
    latest_m--;
    if (latest_n < 0)
        latest_n += n;
    if (latest_m < 0)
        latest_m += m;

    d_latest_m = latest_m;
    d_latest_n = latest_n;
    return static_cast<std::complex<float>>(acc);
}

template <>
std::complex<float> IirFilter<std::complex<float>, std::complex<float>, std::complex<double>, std::complex<double>>::filter(
        const std::complex<float> input)
{
    std::complex<double> acc;
    unsigned i = 0;
    unsigned n = ntaps_ff();
    unsigned m = ntaps_fb();

    if (n == 0)
        return (std::complex<float>)0;

    int latest_n = d_latest_n;
    int latest_m = d_latest_m;

    acc = d_fftaps[0] * static_cast<std::complex<double>>(input);
    for (i = 1; i < n; i++)
        acc += (d_fftaps[i] * static_cast<std::complex<double>>(d_prev_input[latest_n + i]));
    for (i = 1; i < m; i++)
        acc += (d_fbtaps[i] * static_cast<std::complex<double>>(d_prev_output[latest_m + i]));

    // store the values twice to avoid having to handle wrap-around in the loop
    d_prev_output[latest_m] = acc;
    d_prev_output[latest_m + m] = acc;
    d_prev_input[latest_n] = input;
    d_prev_input[latest_n + n] = input;

    latest_n--;
    latest_m--;
    if (latest_n < 0)
        latest_n += n;
    if (latest_m < 0)
        latest_m += m;

    d_latest_m = latest_m;
    d_latest_n = latest_n;
    return static_cast<std::complex<float>>(acc);
}