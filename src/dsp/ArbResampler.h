//
// Created by TheDaChicken on 6/19/2024.
//

#ifndef NRSC5_GUI_SRC_LIB_DSP_FIRDECIM_Q15_H_
#define NRSC5_GUI_SRC_LIB_DSP_FIRDECIM_Q15_H_

#include "utils/Types.h"

#include "FirFilter.h"
#include "HistoryBuffer.h"

class ArbResamplerCCC
{
	public:
		ArbResamplerCCC(float rate,
		                const std::vector<cfloat_t> &taps,
		                unsigned int filter_size);
		void CreateTaps(const std::vector<cfloat_t> &newtaps, std::vector<std::vector<cfloat_t> > &ourtaps,
		                std::vector<fir_filter_ccc> &ourfilter);

		void CreateDiffTaps(const std::vector<cfloat_t> &taps, std::vector<cfloat_t> &dtaps);
		void SetTaps(const std::vector<cfloat_t> &taps);
		void SetRate(float rate);
		void Reset();

		[[nodiscard]] unsigned int taps_per_filter() const { return m_taps_per_filter; }

		int group_delay() const
		{
			return m_delay;
		}

		float phase_offset(float freq, float fs) const
		{
			double adj = (2.0 * M_PI) * (freq / fs) / static_cast<float>(m_inter_rate);
			return -adj * m_est_phase_change;
		}

		int Process(cfloat_t *output, const cfloat_t *input, unsigned int n_to_read);

		int ProcessBlock(cfloat_t *output, const cfloat_t *input, unsigned int n_to_read, int &consumed_samples);

	private:
		std::vector<fir_filter_ccc> m_filters;
		std::vector<fir_filter_ccc> m_diff_filters;
		std::vector<std::vector<cfloat_t> > m_taps;
		std::vector<std::vector<cfloat_t> > m_difftaps;

		HistoryBuffer<cfloat_t> m_buffer;

		unsigned int m_inter_rate; // the number of filters (interpolation rate)
		unsigned int m_dec_rate; // the stride through the filters (decimation rate)
		float m_flt_rate; // residual rate for the linear interpolation
		float m_acc; // accumulator; holds fractional part of sample
		unsigned int m_last_filter; // stores filter for re-entry
		unsigned int m_taps_per_filter; // num taps for each arm of the filterbank
		int m_delay; // filter's group delay
		float m_est_phase_change; // est. of phase change of a sine wave through filt.
};

class ArbResamplerQ15
{
	public:
		ArbResamplerQ15(float rate,
		                const std::vector<cint16_t> &taps,
		                unsigned int filter_size);
		void CreateTaps(const std::vector<cint16_t> &newtaps, std::vector<std::vector<cint16_t> > &ourtaps,
		                std::vector<fir_filter_cscscs> &ourfilter);

		void CreateDiffTaps(const std::vector<cint16_t> &taps, std::vector<cint16_t> &dtaps);
		void SetTaps(const std::vector<cint16_t> &taps);
		void SetRate(float rate);
		void Reset();

		[[nodiscard]] unsigned int taps_per_filter() const { return m_taps_per_filter; }

		int group_delay() const
		{
			return m_delay;
		}

		float phase_offset(float freq, float fs) const
		{
			double adj = (2.0 * M_PI) * (freq / fs) / static_cast<float>(m_inter_rate);
			return -adj * m_est_phase_change;
		}

		int Process(cint16_t *output, const cint16_t *input, unsigned int n_to_read);
		int ProcessBlock(cint16_t *output, const cint16_t *input, unsigned int n_to_read, int &consumed_samples);

	private:
		std::vector<fir_filter_cscscs> m_filters;
		std::vector<fir_filter_cscscs> m_diff_filters;
		std::vector<std::vector<cint16_t> > m_taps;
		std::vector<std::vector<cint16_t> > m_difftaps;

		HistoryBuffer<cint16_t> m_buffer;

		unsigned int m_inter_rate; // the number of filters (interpolation rate)
		unsigned int m_dec_rate; // the stride through the filters (decimation rate)
		float m_flt_rate; // residual rate for the linear interpolation
		float m_acc; // accumulator; holds fractional part of sample
		unsigned int m_last_filter; // stores filter for re-entry
		unsigned int m_taps_per_filter; // num taps for each arm of the filterbank
		int m_delay; // filter's group delay
		float m_est_phase_change; // est. of phase change of a sine wave through filt.

		unsigned int d_consumed_samples; // number of samples need to be skipped
		std::size_t naligned = volk_get_alignment();
};

#endif //NRSC5_GUI_SRC_LIB_DSP_FIRDECIM_Q15_H_
