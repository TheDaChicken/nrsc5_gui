/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IirFilter_H
#define INCLUDED_IirFilter_H

#include <stdexcept>
#include <vector>
#include <complex>

/*!
 * \brief Base class template for Infinite Impulse Response filter (IIR)
 *
 * \details
 *
 * This class provides a templated kernel for IIR filters. These
 * IirFilters can be instantiated with a set of feed-forward
 * and feed-back taps in the constructor. We then call the
 * IirFilter::filter function to add a new sample to the
 * filter, or IirFilter::filter_n to add a vector of samples to
 * be filtered.
 *
 * Instantiating a filter means defining the templates for the
 * data types being processed by the filter. There are four templates:
 *
 * \li i_type the data type of the input data (i.e., float).
 * \li o_type the data type of the output data (i.e., float).
 * \li tap_type the data type of the filter taps (i.e., double).
 * \li acc_type the data type of the internal accumulator (i.e., double).
 *
 * The acc_type is specified to control how data is handled
 * internally in the filter. This should always be the highest
 * precision data type of any of the first three. Often, IIR
 * filters require double-precision values in the taps for
 * stability, and so the internal accumulator should also be
 * double precision.
 *
 * Example:
 *
 * \code
 * gr::filter::kernel::IirFilter<float,float,double,double> iir_filt(fftaps, fbtaps);
 * ...
 * float y = iir_filt.filter(x);
 *
 * <or>
 *
 * iir_filt.filter(y, x, N); // y and x are float arrays
 * \endcode
 *
 * Another example for handling complex samples with
 * double-precision taps (see filter::IirFilter_ccz):
 *
 * \code
 * gr:;filter::kernel::IirFilter<std::complex<float>, std::complex<float>, std::complex<float>d, std::complex<float>d>
 * iir_filt(fftaps, fbtaps); \endcode
 */
template<class i_type, class o_type, class tap_type, class acc_type>
class IirFilter {
public:
    /*!
     * \brief Construct an IIR with the given taps.
     *
     * This filter uses the Direct Form I implementation, where
     * \p fftaps contains the feed-forward taps, and \p fbtaps the feedback ones.
     *
     * \p oldstyle: The old style of the IIR filter uses feedback
     * taps that are negative of what most definitions use (scipy
     * and Matlab among them). This parameter keeps using the old
     * GNU Radio style and is set to TRUE by default. When taps
     * generated from scipy, Matlab, or gr_filter_design, use the
     * new style by setting this to FALSE.
     *
     * When \p oldstyle is set FALSE, the input and output satisfy a
     * difference equation of the form

     \f[
     y[n] + \sum_{k=1}^{M} a_k y[n-k] = \sum_{k=0}^{N} b_k x[n-k]
     \f]

     * with the corresponding rational system function

     \f[
     H(z) = \frac{\sum_{k=0}^{N} b_k z^{-k}}{1 + \sum_{k=1}^{M} a_k z^{-k}}
     \f]
     * where:
     * \f$x\f$ - input signal,
     * \f$y\f$ - output signal,
     * \f$a_k\f$ - k-th feedback tap,
     * \f$b_k\f$ - k-th feed-forward tap,
     * \f$M\f$ - \p len(fbtaps)-1,
     * \f$N\f$ - \p len(fftaps)-1.

     *  \f$a_0\f$, that is \p fbtaps[0], is ignored.
     */
    IirFilter(const std::vector<tap_type> &fftaps,
              const std::vector<tap_type> &fbtaps,
              bool oldstyle = true) noexcept(false) {
        d_oldstyle = oldstyle;
        set_taps(fftaps, fbtaps);
    }

    IirFilter() : d_latest_n(0), d_latest_m(0) {}

    /*!
     * \brief compute a single output value.
     * \returns the filtered input value.
     */
    o_type filter(const i_type input);

    /*!
     * \brief compute an array of N output values.
     * \p input must have N valid entries.
     */
    void filter_n(o_type output[], const i_type input[], long n);

    /*!
     * \return number of taps in filter.
     */
    unsigned ntaps_ff() const { return d_fftaps.size(); }

    unsigned ntaps_fb() const { return d_fbtaps.size(); }

    /*!
     * \brief install new taps.
     */
    void set_taps(const std::vector<tap_type> &fftaps,
                  const std::vector<tap_type> &fbtaps) {
        d_latest_n = 0;
        d_latest_m = 0;
        d_fftaps = fftaps;

        if (d_oldstyle) {
            d_fbtaps = fbtaps;
        } else {
            // New style negates taps a[1:N-1] to fit with most IIR
            // tap generating programs.
            d_fbtaps.resize(fbtaps.size());
            d_fbtaps[0] = fbtaps[0];
            for (size_t i = 1; i < fbtaps.size(); i++) {
                d_fbtaps[i] = -fbtaps[i];
            }
        }

        int n = fftaps.size();
        int m = fbtaps.size();
        d_prev_input.clear();
        d_prev_output.clear();
        d_prev_input.resize(2 * n, 0);
        d_prev_output.resize(2 * m, 0);
    }

protected:
    bool d_oldstyle;
    std::vector<tap_type> d_fftaps;
    std::vector<tap_type> d_fbtaps;
    int d_latest_n;
    int d_latest_m;
    std::vector<acc_type> d_prev_output;
    std::vector<i_type> d_prev_input;
};

//
// general case.  We may want to specialize this
//
template<class i_type, class o_type, class tap_type, class acc_type>
o_type IirFilter<i_type, o_type, tap_type, acc_type>::filter(const i_type input) {
    acc_type acc;
    unsigned i = 0;
    unsigned n = ntaps_ff();
    unsigned m = ntaps_fb();

    if (n == 0)
        return (o_type) 0;

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
    return (o_type) acc;
}

template<class i_type, class o_type, class tap_type, class acc_type>
void IirFilter<i_type, o_type, tap_type, acc_type>::filter_n(o_type output[],
                                                             const i_type input[],
                                                             long n) {
    for (int i = 0; i < n; i++)
        output[i] = filter(input[i]);
}

template <>
std::complex<float>
IirFilter<std::complex<float>, std::complex<float>, float, std::complex<float>>::filter(std::complex<float> input);

template <>
std::complex<float>
IirFilter<std::complex<float>, std::complex<float>, double, std::complex<double>>::filter(std::complex<float> input);

template <>
std::complex<float> IirFilter<std::complex<float>, std::complex<float>, std::complex<double>, std::complex<double>>::filter(
        std::complex<float> input);


#endif  /* INCLUDED_IirFilter_H */
