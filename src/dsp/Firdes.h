/* -*- c++ -*- */
/*
 * Copyright 2002,2007,2008,2012,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef FIRDES_H
#define FIRDES_H

#include <vector>

#include "dsp/Window.h"
#include "utils/Types.h"

class firdes
{
public:
    static std::vector<float> Window(Window::win_type type, int ntaps, double param);

    // ... class methods ...

    /*!
     * \brief Use "Window method" to design a low-pass FIR filter.  The
     * normalized width of the transition band is what sets the number of
     * taps required.  Narrow --> more taps.  Window type determines maximum
     * attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param cutoff_freq         center of transition band (Hz)
     * \param transition_width    width of transition band (Hz)
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<float>
    low_pass(double gain,
             double sampling_freq,
             double cutoff_freq,      // Hz center of transition band
             double transition_width, // Hz width of transition band
             Window::win_type Window = Window::win_type::WIN_HAMMING,
             double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!
     * \brief Use "Window method" to design a low-pass FIR filter.  The
     * normalized width of the transition band and the required stop band
     * attenuation is what sets the number of taps required.  Narrow --> more
     * taps More attenuation --> more taps. The Window type determines
     * maximum attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param cutoff_freq         beginning of transition band (Hz)
     * \param transition_width    width of transition band (Hz)
     * \param attenuation_dB      required stopband attenuation
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<float>
    low_pass_2(double gain,
               double sampling_freq,
               double cutoff_freq,      // Hz beginning transition band
               double transition_width, // Hz width of transition band
               double attenuation_dB,   // out of band attenuation dB
               Window::win_type Window = Window::win_type::WIN_HAMMING,
               double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!
     * \brief Use "Window method" to design a high-pass FIR filter.  The
     * normalized width of the transition band is what sets the number of
     * taps required.  Narrow --> more taps. The Window determines maximum
     * attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param cutoff_freq         center of transition band (Hz)
     * \param transition_width    width of transition band (Hz)
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<float>
    high_pass(double gain,
              double sampling_freq,
              double cutoff_freq,      // Hz center of transition band
              double transition_width, // Hz width of transition band
              Window::win_type Window = Window::win_type::WIN_HAMMING,
              double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!
     * \brief Use "Window method" to design a high-pass FIR filter. The
     * normalized width of the transition band and the required stop band
     * attenuation is what sets the number of taps required.  Narrow --> more
     * taps More attenuation --> more taps. The Window determines maximum
     * attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param cutoff_freq         center of transition band (Hz)
     * \param transition_width    width of transition band (Hz).
     * \param attenuation_dB      out of band attenuation
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<float>
    high_pass_2(double gain,
                double sampling_freq,
                double cutoff_freq,      // Hz center of transition band
                double transition_width, // Hz width of transition band
                double attenuation_dB,   // out of band attenuation dB
                Window::win_type Window = Window::win_type::WIN_HAMMING,
                double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!
     * \brief Use "Window method" to design a band-pass FIR filter. The
     * normalized width of the transition band is what sets the number of
     * taps required.  Narrow --> more taps. The Window determines maximum
     * attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param low_cutoff_freq     center of transition band (Hz)
     * \param high_cutoff_freq    center of transition band (Hz)
     * \param transition_width    width of transition band (Hz).
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<float>
    band_pass(double gain,
              double sampling_freq,
              double low_cutoff_freq,  // Hz center of transition band
              double high_cutoff_freq, // Hz center of transition band
              double transition_width, // Hz width of transition band
              Window::win_type Window = Window::win_type::WIN_HAMMING,
              double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!
     * \brief Use "Window method" to design a band-pass FIR filter.  The
     * normalized width of the transition band and the required stop band
     * attenuation is what sets the number of taps required.  Narrow --> more
     * taps.  More attenuation --> more taps.  Window type determines maximum
     * attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param low_cutoff_freq     center of transition band (Hz)
     * \param high_cutoff_freq    center of transition band (Hz)
     * \param transition_width    width of transition band (Hz).
     * \param attenuation_dB      out of band attenuation
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<float>
    band_pass_2(double gain,
                double sampling_freq,
                double low_cutoff_freq,  // Hz beginning transition band
                double high_cutoff_freq, // Hz beginning transition band
                double transition_width, // Hz width of transition band
                double attenuation_dB,   // out of band attenuation dB
                Window::win_type Window = Window::win_type::WIN_HAMMING,
                double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows
    /*!
     * \brief Use the "Window method" to design a complex band-reject FIR
     * filter.  The normalized width of the transition band is what sets the
     * number of taps required.  Narrow --> more taps. The Window type
     * determines maximum attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param low_cutoff_freq     center of transition band (Hz)
     * \param high_cutoff_freq    center of transition band (Hz)
     * \param transition_width    width of transition band (Hz)
     * \param Window              one of Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<cfloat_t> complex_band_reject(
        double gain,
        double sampling_freq,
        double low_cutoff_freq,
        double high_cutoff_freq,
        double transition_width, // Hz width of transition band
        Window::win_type Window = Window::win_type::WIN_HAMMING,
        double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!
     * \brief Use "Window method" to design a complex band-reject FIR filter.
     * The normalized width of the transition band and the required stop band
     * attenuation is what sets the number of taps required.  Narrow --> more
     * taps More attenuation --> more taps. Window type determines maximum
     * attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param low_cutoff_freq     center of transition band (Hz)
     * \param high_cutoff_freq    center of transition band (Hz)
     * \param transition_width    width of transition band (Hz)
     * \param attenuation_dB      out of band attenuation
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<cfloat_t> complex_band_reject_2(
        double gain,
        double sampling_freq,
        double low_cutoff_freq,  // Hz beginning transition band
        double high_cutoff_freq, // Hz beginning transition band
        double transition_width, // Hz width of transition band
        double attenuation_dB,   // out of band attenuation dB
        Window::win_type Window = Window::win_type::WIN_HAMMING,
        double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!
     * \brief Use the "Window method" to design a complex band-pass FIR
     * filter.  The normalized width of the transition band is what sets the
     * number of taps required.  Narrow --> more taps. The Window type
     * determines maximum attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param low_cutoff_freq     center of transition band (Hz)
     * \param high_cutoff_freq    center of transition band (Hz)
     * \param transition_width    width of transition band (Hz)
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<cfloat_t> complex_band_pass(
        double gain,
        double sampling_freq,
        double low_cutoff_freq,  // Hz center of transition band
        double high_cutoff_freq, // Hz center of transition band
        double transition_width, // Hz width of transition band
        Window::win_type Window = Window::win_type::WIN_HAMMING,
        double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!
     * \brief Use "Window method" to design a complex band-pass FIR filter.
     * The normalized width of the transition band and the required stop band
     * attenuation is what sets the number of taps required.  Narrow --> more
     * taps More attenuation --> more taps. Window type determines maximum
     * attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param low_cutoff_freq     center of transition band (Hz)
     * \param high_cutoff_freq    center of transition band (Hz)
     * \param transition_width    width of transition band (Hz)
     * \param attenuation_dB      out of band attenuation
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<cfloat_t> complex_band_pass_2(
        double gain,
        double sampling_freq,
        double low_cutoff_freq,  // Hz beginning transition band
        double high_cutoff_freq, // Hz beginning transition band
        double transition_width, // Hz width of transition band
        double attenuation_dB,   // out of band attenuation dB
        Window::win_type Window = Window::win_type::WIN_HAMMING,
        double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!
     * \brief Use "Window method" to design a band-reject FIR filter.  The
     * normalized width of the transition band is what sets the number of
     * taps required.  Narrow --> more taps. Window type determines maximum
     * attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param low_cutoff_freq     center of transition band (Hz)
     * \param high_cutoff_freq    center of transition band (Hz)
     * \param transition_width    width of transition band (Hz)
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<float>
    band_reject(double gain,
                double sampling_freq,
                double low_cutoff_freq,  // Hz center of transition band
                double high_cutoff_freq, // Hz center of transition band
                double transition_width, // Hz width of transition band
                Window::win_type Window = Window::win_type::WIN_HAMMING,
                double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!
     * \brief Use "Window method" to design a band-reject FIR filter.  The
     * normalized width of the transition band and the required stop band
     * attenuation is what sets the number of taps required.  Narrow --> more
     * taps More attenuation --> more taps.  Window type determines maximum
     * attenuation and passband ripple.
     *
     * \param gain                overall gain of filter (typically 1.0)
     * \param sampling_freq       sampling freq (Hz)
     * \param low_cutoff_freq     center of transition band (Hz)
     * \param high_cutoff_freq    center of transition band (Hz)
     * \param transition_width    width of transition band (Hz).
     * \param attenuation_dB      out of band attenuation
     * \param Window              one of fft::Window::win_type
     * \param param               parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<float>
    band_reject_2(double gain,
                  double sampling_freq,
                  double low_cutoff_freq,  // Hz beginning transition band
                  double high_cutoff_freq, // Hz beginning transition band
                  double transition_width, // Hz width of transition band
                  double attenuation_dB,   // out of band attenuation dB
                  Window::win_type Window = Window::win_type::WIN_HAMMING,
                  double param = 6.76); // used for Kaiser, Exp., Gaussian, Tukey Windows

    /*!\brief design a Hilbert Transform Filter
     *
     * \param ntaps           number of taps, must be odd
     * \param Windowtype      one kind of fft::Window::win_type
     * \param param           parameter for Kaiser, Exp., Gaussian, Tukey Windows
     */
    static std::vector<float>
    hilbert(unsigned int ntaps = 19,
            Window::win_type Windowtype = Window::win_type::WIN_RECTANGULAR,
            double param = 6.76);

    /*!
     * \brief design a Root Cosine FIR Filter (do we need a Window?)
     *
     * \param gain            overall gain of filter (typically 1.0)
     * \param sampling_freq   sampling freq (Hz)
     * \param symbol_rate     symbol rate, must be a factor of sample rate
     * \param alpha           excess bandwidth factor
     * \param ntaps           number of taps
     */
    static std::vector<float>
    root_raised_cosine(double gain,
                       double sampling_freq,
                       double symbol_rate, // Symbol rate, NOT bitrate (unless BPSK)
                       double alpha,       // Excess Bandwidth Factor
                       int ntaps);

    /*!
     * \brief design a Gaussian filter
     *
     * \param gain    overall gain of filter (typically 1.0)
     * \param spb     symbol rate, must be a factor of sample rate
     * \param bt      bandwidth to bitrate ratio
     * \param ntaps   number of taps
     */
    static std::vector<float> gaussian(double gain,
                                       double spb,
                                       double bt, // Bandwidth to bitrate ratio
                                       int ntaps);

private:
    static double bessi0(double x);
    static void sanity_check_1f(double sampling_freq, double f1, double transition_width);
    static void
    sanity_check_2f(double sampling_freq, double f1, double f2, double transition_width);
    static void sanity_check_2f_c(double sampling_freq,
                                  double f1,
                                  double f2,
                                  double transition_width);

    static int compute_ntaps(double sampling_freq,
                             double transition_width,
                             Window::win_type Window_type,
                             double param);

    static int compute_ntaps_windes(double sampling_freq,
                                    double transition_width,
                                    double attenuation_dB);
};

#endif //FIRDES_H
