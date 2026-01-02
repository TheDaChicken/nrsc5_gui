//
// Created by TheDaChicken on 6/19/2024.
//

#ifndef NRSC5_GUI_SRC_LIB_DSP_FIRDECIM_Q15_H_
#define NRSC5_GUI_SRC_LIB_DSP_FIRDECIM_Q15_H_

#include "IFilterBlock.h"
#include "utils/Types.h"

#include "dsp/FirFilter.h"
#include "utils/HistoryBuffer.h"

template<typename IN_T, typename TAPS_T>
class ArbResampler final : public IFilterBlock
{
	public:
		using FIR_FILTER = fir_filter<IN_T, IN_T, TAPS_T>;

		ArbResampler(float rate,
		             const std::vector<TAPS_T> &taps,
		             unsigned int filter_size);
		void CreateTaps(const std::vector<TAPS_T> &newtaps, std::vector<std::vector<TAPS_T> > &ourtaps,
		                std::vector<FIR_FILTER> &ourfilter);

		static void CreateDiffTaps(const std::vector<TAPS_T> &new_taps, std::vector<TAPS_T> &diff_taps);
		static IN_T Interpolate(const IN_T &o0,
		                        const IN_T &o1,
		                        float acc);

		void SetTaps(const std::vector<TAPS_T> &taps);
		void SetRate(float rate);

		[[nodiscard]] unsigned int TapsPerFilter() const { return m_taps_per_filter; }

		[[nodiscard]] int GroupDelay() const
		{
			return m_delay;
		}

		[[nodiscard]] float PhaseOffset(float freq, float fs) const
		{
			const double adj = (2.0 * M_PI) * (freq / fs) / static_cast<float>(m_inter_rate);
			return -adj * m_est_phase_change;
		}

		int ProcessBlock(IN_T *output, const IN_T *input, unsigned int n_to_read, int &consumed_samples);
		int ProcessBlock(void *output, const void *input, unsigned int n_to_read, int &consumed_samples) override
		{
			return ProcessBlock(static_cast<IN_T *>(output),
			                    static_cast<const IN_T *>(input),
			                    n_to_read,
			                    consumed_samples);
		}

		[[nodiscard]] int BlockSize() const override
		{
			return m_taps_per_filter;
		}

	private:
		std::vector<FIR_FILTER> m_filters;
		std::vector<FIR_FILTER> m_diff_filters;
		std::vector<std::vector<TAPS_T> > m_taps;
		std::vector<std::vector<TAPS_T> > m_difftaps;
		HistoryBuffer<IN_T> m_history;

		unsigned int m_inter_rate; // the number of filters (interpolation rate)
		unsigned int m_dec_rate; // the stride through the filters (decimation rate)
		float m_flt_rate; // residual rate for the linear interpolation
		unsigned int m_taps_per_filter; // num taps for each arm of the filterbank
		int m_delay; // filter's group delay
		float m_est_phase_change; // est. of phase change of a sine wave through filt.
		float m_acc; // accumulator; holds fractional part of sample
		unsigned int m_last_filter; // stores filter for re-entry
};

template<typename in_t, typename taps_t>
ArbResampler<in_t, taps_t>::ArbResampler(
	const float rate,
	const std::vector<taps_t> &taps,
	unsigned int filter_size)
	: m_inter_rate(filter_size),
	  m_dec_rate(0),
	  m_flt_rate(0),
	  m_taps_per_filter(0),
	  m_delay(0),
	  m_est_phase_change(0.0f)
{
	m_acc = 0; // start accumulator at 0

	SetRate(rate);

	m_last_filter = (taps.size() / 2) % filter_size;

	// Create an FIR filter for each channel and zero out the taps
	const std::vector<taps_t> vtaps(m_inter_rate, 0.0);

	m_filters.reserve(m_inter_rate);
	m_diff_filters.reserve(m_inter_rate);

	for (unsigned int i = 0; i < m_inter_rate; i++)
	{
		m_filters.emplace_back(vtaps);
		m_diff_filters.emplace_back(vtaps);
	}

	// Now, actually set the filters' taps
	SetTaps(taps);

	// Delay is based on number of taps per filter arm. Round to
	// the nearest integer.
	const float delay = rate * (TapsPerFilter() - 1.0) / 2.0;
	m_delay = static_cast<int>(std::lround(delay));

	// This calculation finds the phase offset induced by the
	// arbitrary resampling. It's based on which filter arm we are
	// at the filter's group delay plus the fractional offset
	// between the samples. Calculated here based on the rotation
	// around nfilts starting at start_filter.
	const float accum = m_delay * m_flt_rate;
	const int accum_int = static_cast<int>(accum);
	const float accum_frac = accum - accum_int;
	const int end_filter = static_cast<int>(
		std::lround(fmodf(m_last_filter + m_delay * m_dec_rate + accum_int,
		                  static_cast<float>(m_inter_rate))));

	m_est_phase_change = m_last_filter - (end_filter + accum_frac);
}

template<typename in_t, typename taps_t>
void ArbResampler<in_t, taps_t>::CreateTaps(const std::vector<taps_t> &newtaps,
                                            std::vector<std::vector<taps_t> > &ourtaps,
                                            std::vector<FIR_FILTER> &ourfilter)
{
	unsigned int ntaps = newtaps.size();
	m_taps_per_filter = static_cast<unsigned int>(ceil(static_cast<double>(ntaps) / static_cast<double>(m_inter_rate)));

	// Create d_numchan vectors to store each channel's taps
	ourtaps.resize(m_inter_rate);

	// Make a vector of the taps plus fill it out with 0's to fill
	// each polyphase filter with exactly m_taps_per_filter
	std::vector<taps_t> tmp_taps = newtaps;
	while (static_cast<float>(tmp_taps.size()) < m_inter_rate * m_taps_per_filter)
	{
		tmp_taps.push_back(in_t(0));
	}

	for (unsigned int i = 0; i < m_inter_rate; i++)
	{
		// Each channel uses all m_taps_per_filter with 0's if not enough taps to fill out
		ourtaps[i] = std::vector<taps_t>(m_taps_per_filter, 0);
		for (unsigned int j = 0; j < m_taps_per_filter; j++)
		{
			ourtaps[i][j] = tmp_taps[i + j * m_inter_rate];
		}

		// Build a filter for each channel and add its taps to it
		ourfilter[i].set_taps(ourtaps[i]);
	}
}

template<typename in_t, typename taps_t>
void ArbResampler<in_t, taps_t>::SetTaps(const std::vector<taps_t> &taps)
{
	std::vector<in_t> dtaps;
	CreateDiffTaps(taps, dtaps);
	CreateTaps(taps, m_taps, m_filters);
	CreateTaps(dtaps, m_difftaps, m_diff_filters);
}

template<typename in_t, typename taps_t>
void ArbResampler<in_t, taps_t>::SetRate(float rate)
{
	m_dec_rate = static_cast<unsigned int>(floor(m_inter_rate / rate));
	m_flt_rate = m_inter_rate / rate - m_dec_rate;
}

template<typename in_t, typename taps_t>
int ArbResampler<in_t, taps_t>::ProcessBlock(
	in_t *output,
	const in_t *input,
	unsigned int n_to_read,
	int &consumed_samples)
{
	int i_in = 0, i_out = 0;
	unsigned int phase_index = m_last_filter;

	while (i_in < n_to_read)
	{
		// start j by wrapping around mod the number of channels
		while (phase_index < m_inter_rate)
		{
			// Take the current filter and derivative filter output
			in_t o0 = m_filters[phase_index].filter(&input[i_in]);
			in_t o1 = m_diff_filters[phase_index].filter(&input[i_in]);

			output[i_out] = o0 + Interpolate(o0, o1, m_acc);
			// linearly interpolate between samples
			i_out++;

			// Adjust accumulator and index into filterbank
			m_acc += m_flt_rate;
			phase_index += m_dec_rate + static_cast<int>(floor(m_acc));
			m_acc = fmodf(m_acc, 1.0);
		}

		i_in += static_cast<int>(phase_index / m_inter_rate);
		phase_index = phase_index % m_inter_rate;
	}

	consumed_samples = i_in;
	m_last_filter = phase_index; // save last filter state for re-entry
	return i_out;
}

template<>
inline cint16_t ArbResampler<cint16_t, cint16_t>::Interpolate(
	const cint16_t &o0,
	const cint16_t &o1,
	const float acc)
{
	const auto scale = static_cast<int32_t>(acc * 32768);
	const auto r = static_cast<int16_t>((static_cast<int32_t>(o1.real()) * scale) >> 15);
	const auto i = static_cast<int16_t>((static_cast<int32_t>(o1.imag()) * scale) >> 15);
	return o0 + cint16_t(r, i);
}

template<>
inline cfloat_t ArbResampler<cfloat_t, cfloat_t>::Interpolate(
	const cfloat_t &o0,
	const cfloat_t &o1,
	const float acc)
{
	return o0 + o1 * acc;
}

template<typename in_t, typename taps_t>
void ArbResampler<in_t, taps_t>::CreateDiffTaps(
	const std::vector<taps_t> &new_taps,
	std::vector<taps_t> &diff_taps)
{
	// Calculate the differential taps using a derivative filter
	std::vector<taps_t> diff_filter(2);
	diff_filter[0] = std::numeric_limits<taps_t>::min();
	diff_filter[1] = std::numeric_limits<taps_t>::max();

	FIR_FILTER diff_fir(diff_filter);

	diff_taps.resize(new_taps.size());

	for (unsigned int i = 0; i < new_taps.size() - 1; i++)
	{
		diff_taps[i] = diff_fir.filter(&new_taps[i]);
	}
	diff_taps[new_taps.size() - 1] = 0;
}

#endif //NRSC5_GUI_SRC_LIB_DSP_FIRDECIM_Q15_H_
