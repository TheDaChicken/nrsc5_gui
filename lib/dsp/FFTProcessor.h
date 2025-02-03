//
// Created by TheDaChicken on 7/17/2023.
//

#ifndef NRSC5_GUI_PlotterFFTCC_H
#define NRSC5_GUI_PlotterFFTCC_H

#include <fftw3.h>
#include <complex>
#include <cstdint>
#include <mutex>
#include <vector>

#include "utils/Types.h"

class FFTProcessorComplex {
public:
    explicit FFTProcessorComplex(int fftSize, bool d_normalize_energy = false);
    ~FFTProcessorComplex();

    FFTProcessorComplex(const FFTProcessorComplex&) = delete;

    fftwf_complex* GetIn();
    std::complex<float>* Process(const vector_complex_t& input);
    std::complex<float>* Process();

    [[nodiscard]] int GetFFTSize() const { return m_fftSize; };
private:
    void CreateFFTW();
    void CreateWindows();
    void ApplyWindow(int samples);

    fftwf_complex *m_fftIn;
    fftwf_complex *m_fftOut;
    fftwf_plan m_fftPlan;

    std::vector<float> d_window; /*! FFT window taps. */

    int m_fftSize;
    bool m_normalize_energy;
};

#endif //NRSC5_GUI_PlotterFFTCC_H
