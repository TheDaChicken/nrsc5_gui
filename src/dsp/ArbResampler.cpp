//
// Created by TheDaChicken on 6/19/2024.
//

#include "ArbResampler.h"

#include "utils/Log.h"

constexpr float Q15_SCALE = 32767.0f;

ArbResamplerCCC::ArbResamplerCCC(float rate, const std::vector<cfloat_t> &taps, unsigned int filter_size)
	: m_flt_rate(0),
	  m_taps_per_filter(0)
{
	m_acc = 0; // start accumulator at 0

	/* The number of filters is specified by the user as the
	   filter size; this is also the interpolation rate of the
	   filter. We use it and the rate provided to determine the
	   decimation rate. This acts as a rational resampler. The
	   flt_rate is calculated as the residual between the integer
	   decimation rate and the real decimation rate and will be
	   used to determine to interpolation point of the resampling
	   process.
	*/
	m_inter_rate = filter_size;
	SetRate(rate);

	m_last_filter = (taps.size() / 2) % filter_size;

	// Create an FIR filter for each channel and zero out the taps
	const std::vector<cfloat_t> vtaps(m_inter_rate);
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
	float delay = rate * (taps_per_filter() - 1.0) / 2.0;
	m_delay = static_cast<int>(std::lround(delay));

	// This calculation finds the phase offset induced by the
	// arbitrary resampling. It's based on which filter arm we are
	// at the filter's group delay plus the fractional offset
	// between the samples. Calculated here based on the rotation
	// around nfilts starting at start_filter.
	float accum = m_delay * m_flt_rate;
	int accum_int = static_cast<int>(accum);
	float accum_frac = accum - accum_int;
	int end_filter = static_cast<int>(
		std::lround(fmodf(m_last_filter + m_delay * m_dec_rate + accum_int,
		                  static_cast<float>(m_inter_rate))));

	m_est_phase_change = m_last_filter - (end_filter + accum_frac);
}

void ArbResamplerCCC::CreateTaps(const std::vector<cfloat_t> &newtaps,
                                  std::vector<std::vector<cfloat_t> > &ourtaps,
                                  std::vector<fir_filter_ccc> &ourfilter)
{
	unsigned int ntaps = newtaps.size();
	m_taps_per_filter = static_cast<unsigned int>(ceil(static_cast<double>(ntaps) / static_cast<double>(m_inter_rate)));

	// Create d_numchan vectors to store each channel's taps
	ourtaps.resize(m_inter_rate);

	// Make a vector of the taps plus fill it out with 0's to fill
	// each polyphase filter with exactly m_taps_per_filter
	std::vector<cfloat_t> tmp_taps = newtaps;
	while (static_cast<float>(tmp_taps.size()) < m_inter_rate * m_taps_per_filter)
	{
		tmp_taps.push_back({0.0, 0.0});
	}

	for (unsigned int i = 0; i < m_inter_rate; i++)
	{
		// Each channel uses all m_taps_per_filter with 0's if not enough taps to fill out
		ourtaps[i] = std::vector<cfloat_t>(m_taps_per_filter);
		for (unsigned int j = 0; j < m_taps_per_filter; j++)
		{
			ourtaps[i][j] = tmp_taps[i + j * m_inter_rate];
		}

		// Build a filter for each channel and add its taps to it
		ourfilter[i].set_taps(ourtaps[i]);
	}
}

void ArbResamplerCCC::CreateDiffTaps(const std::vector<cfloat_t> &newtaps,
                                       std::vector<cfloat_t> &difftaps)
{
	// Calculate the differential taps using a derivative filter
	std::vector<cfloat_t> diff_filter(2);
	diff_filter[0] = -1.0;
	diff_filter[1] = 1.0;

	for (unsigned int i = 0; i < newtaps.size() - 1; i++)
	{
		cfloat_t tap = {0, 0};
		for (unsigned int j = 0; j < diff_filter.size(); j++)
		{
			tap += diff_filter[j] * newtaps[i + j];
		}
		difftaps.push_back(tap);
	}
	difftaps.push_back({0, 0});
}

void ArbResamplerCCC::SetTaps(const std::vector<cfloat_t> &taps)
{
	std::vector<cfloat_t> dtaps;
	CreateDiffTaps(taps, dtaps);
	CreateTaps(taps, m_taps, m_filters);
	CreateTaps(dtaps, m_difftaps, m_diff_filters);

	m_buffer.SetHistorySize(m_taps_per_filter);
	m_buffer.SetBlockSize(4);
	m_buffer.Reset();
}

void ArbResamplerCCC::SetRate(float rate)
{
	m_dec_rate = static_cast<unsigned int>(floor(m_inter_rate / rate));
	m_flt_rate = (m_inter_rate / rate) - m_dec_rate;
}

void ArbResamplerCCC::Reset()
{
	m_buffer.Reset();
}

int ArbResamplerCCC::Process(cfloat_t *output, const cfloat_t *input, unsigned int n_to_read)
{
	unsigned int i_out = 0, i_in = 0;

	while (i_in < n_to_read)
	{
		unsigned int left = std::min(n_to_read - i_in, m_buffer.WriteAvailable());

		m_buffer.Write(&input[i_in], left);
		i_in += left;

		if (m_buffer.ReadAvailable() < m_taps_per_filter)
			break;

		int n_read;
		i_out += ProcessBlock(&output[i_out],
		                       m_buffer.Get(),
		                       m_taps_per_filter,
		                       n_read);

		m_buffer.Consume(n_read);
	}

	return i_out;
}

int ArbResamplerCCC::ProcessBlock(cfloat_t *output, const cfloat_t *input, unsigned int n_to_read,
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
			cfloat_t o0 = m_filters[phase_index].filter(&input[i_in]);
			cfloat_t o1 = m_diff_filters[phase_index].filter(&input[i_in]);

			output[i_out] = o0 + o1 * m_acc; // linearly interpolate between samples
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

ArbResamplerQ15::ArbResamplerQ15(float rate, const std::vector<cint16_t> &taps, unsigned int filter_size)
	: m_flt_rate(0),
	  m_taps_per_filter(0),
	  d_consumed_samples(0)
{
	m_acc = 0; // start accumulator at 0

	/* The number of filters is specified by the user as the
	   filter size; this is also the interpolation rate of the
	   filter. We use it and the rate provided to determine the
	   decimation rate. This acts as a rational resampler. The
	   flt_rate is calculated as the residual between the integer
	   decimation rate and the real decimation rate and will be
	   used to determine to interpolation point of the resampling
	   process.
	*/
	m_inter_rate = filter_size;
	SetRate(rate);

	m_last_filter = (taps.size() / 2) % filter_size;

	// Create an FIR filter for each channel and zero out the taps
	const std::vector<cint16_t> vtaps(m_inter_rate, 0.0);
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
	float delay = rate * (taps_per_filter() - 1.0) / 2.0;
	m_delay = static_cast<int>(std::lround(delay));

	// This calculation finds the phase offset induced by the
	// arbitrary resampling. It's based on which filter arm we are
	// at the filter's group delay plus the fractional offset
	// between the samples. Calculated here based on the rotation
	// around nfilts starting at start_filter.
	float accum = m_delay * m_flt_rate;
	int accum_int = static_cast<int>(accum);
	float accum_frac = accum - accum_int;
	int end_filter = static_cast<int>(
		std::lround(fmodf(m_last_filter + m_delay * m_dec_rate + accum_int,
		                  static_cast<float>(m_inter_rate))));

	m_est_phase_change = m_last_filter - (end_filter + accum_frac);
}

void ArbResamplerQ15::CreateTaps(const std::vector<cint16_t> &newtaps,
                                  std::vector<std::vector<cint16_t> > &ourtaps,
                                  std::vector<fir_filter_cscscs> &ourfilter)
{
	unsigned int ntaps = newtaps.size();
	m_taps_per_filter = static_cast<unsigned int>(ceil(static_cast<double>(ntaps) / static_cast<double>(m_inter_rate)));

	// Create d_numchan vectors to store each channel's taps
	ourtaps.resize(m_inter_rate);

	// Make a vector of the taps plus fill it out with 0's to fill
	// each polyphase filter with exactly m_taps_per_filter
	std::vector<cint16_t> tmp_taps = newtaps;
	while (static_cast<float>(tmp_taps.size()) < m_inter_rate * m_taps_per_filter)
	{
		tmp_taps.push_back(0.0);
	}

	for (unsigned int i = 0; i < m_inter_rate; i++)
	{
		// Each channel uses all m_taps_per_filter with 0's if not enough taps to fill out
		ourtaps[i] = std::vector<cint16_t>(m_taps_per_filter, 0);
		for (unsigned int j = 0; j < m_taps_per_filter; j++)
		{
			ourtaps[i][j] = tmp_taps[i + j * m_inter_rate];
		}

		// Build a filter for each channel and add its taps to it
		ourfilter[i].set_taps(ourtaps[i]);
	}
}

void ArbResamplerQ15::CreateDiffTaps(const std::vector<cint16_t> &newtaps,
                                       std::vector<cint16_t> &difftaps)
{
	// Calculate the differential taps using a derivative filter
	std::vector<cint16_t> diff_filter(2);
	diff_filter[0] = -32768; // Q15 representation of -1.0
	diff_filter[1] = 32767; // Q15 representation of 1.0

	for (unsigned int i = 0; i < newtaps.size() - 1; i++)
	{
		int32_t acc_real = 0;
		int32_t acc_imag = 0;
		for (unsigned int j = 0; j < diff_filter.size(); j++)
		{
			const int32_t real = static_cast<int32_t>(newtaps[i + j].real()) * diff_filter[j].real();
			const int32_t imag = static_cast<int32_t>(newtaps[i + j].imag()) * diff_filter[j].imag();

			acc_real += real;
			acc_imag += imag;
		}
		acc_real >>= 15;
		acc_imag >>= 15;

		difftaps.emplace_back(
			static_cast<int16_t>(acc_real),
			static_cast<int16_t>(acc_imag)
		);
	}
	difftaps.emplace_back(0);
}

void ArbResamplerQ15::SetTaps(const std::vector<cint16_t> &taps)
{
	std::vector<cint16_t> dtaps;
	CreateDiffTaps(taps, dtaps);
	CreateTaps(taps, m_taps, m_filters);
	CreateTaps(dtaps, m_difftaps, m_diff_filters);

	m_buffer.SetHistorySize(m_taps_per_filter);
	m_buffer.SetBlockSize(4);
	m_buffer.Reset();
}

void ArbResamplerQ15::SetRate(float rate)
{
	m_dec_rate = static_cast<unsigned int>(floor(m_inter_rate / rate));
	m_flt_rate = (m_inter_rate / rate) - m_dec_rate;
}

void ArbResamplerQ15::Reset()
{
	m_buffer.Reset();
}

int ArbResamplerQ15::Process(cint16_t *output, const cint16_t *input, unsigned int n_to_read)
{
	unsigned int i_out = 0, i_in = 0;

	while (i_in < n_to_read)
	{
		unsigned int left = std::min(n_to_read - i_in, m_buffer.WriteAvailable());

		m_buffer.Write(&input[i_in], left);
		i_in += left;

		// If we reach the end of the input, and we have not filled the buffer,
		// we can stop processing.
		if (m_buffer.ReadAvailable() < m_taps_per_filter)
			break;

		int n_read;
		i_out += ProcessBlock(&output[i_out],
		                       m_buffer.Get(),
		                       m_taps_per_filter,
		                       n_read);

		m_buffer.Consume(n_read);
	}

	return i_out;
}

int ArbResamplerQ15::ProcessBlock(cint16_t *output, const cint16_t *input, unsigned int n_to_read,
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
			cint16_t o0 = m_filters[phase_index].filter(&input[i_in]);
			cint16_t o1 = m_diff_filters[phase_index].filter(&input[i_in]);

			const auto scaled_acc = static_cast<int32_t>(m_acc * Q15_SCALE);

			auto o1_real = static_cast<int16_t>((static_cast<int32_t>(o1.real()) * scaled_acc) >> 15);
			auto o1_imag = static_cast<int16_t>((static_cast<int32_t>(o1.imag()) * scaled_acc) >> 15);

			output[i_out] = o0 + cint16_t(o1_real, o1_imag);
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
