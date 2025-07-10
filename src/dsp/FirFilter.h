/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef FIRFILTER_H
#define FIRFILTER_H

#include <algorithm>
#include <complex>
#include <vector>
#include <volk/volk.h>
#include <volk/volk_alloc.hh>

#include "utils/Types.h"

template<class IN_T, class OUT_T, class TAP_T>
class fir_filter
{
	public:
		explicit fir_filter(const std::vector<TAP_T> &taps);

		// Disallow copy.
		//
		// This prevents accidentally doing needless copies, not just of fir_filter,
		// but every block that contains one.
		fir_filter(const fir_filter &) = delete;
		fir_filter &operator=(const fir_filter &) = delete;
		fir_filter(fir_filter &&) = default;
		fir_filter &operator=(fir_filter &&) = default;

		void set_taps(const std::vector<TAP_T> &taps);
		void update_tap(TAP_T t, unsigned int index);
		std::vector<TAP_T> taps() const;
		unsigned int ntaps() const;

		OUT_T filter(const IN_T input[]) const;
		void filterN(OUT_T output[], const IN_T input[], unsigned long n);
		void filterNdec(OUT_T output[],
		                const IN_T input[],
		                unsigned long n,
		                unsigned int decimate);

	protected:
		std::vector<TAP_T> d_taps;
		unsigned int d_ntaps;
		std::vector<volk::vector<TAP_T> > d_aligned_taps;
		volk::vector<OUT_T> d_output;
		int d_align;
		int d_naligned;
};

typedef fir_filter<float, float, float> fir_filter_fff;
typedef fir_filter<cfloat_t, cfloat_t, float> fir_filter_ccf;
typedef fir_filter<float, cfloat_t, cfloat_t> fir_filter_fcc;
typedef fir_filter<cfloat_t, cfloat_t, cfloat_t> fir_filter_ccc;
typedef fir_filter<std::int16_t, cfloat_t, cfloat_t> fir_filter_scc;
typedef fir_filter<float, std::int16_t, float> fir_filter_fsf;
typedef fir_filter<cint16_t, cint16_t, cint16_t> fir_filter_cscscs;

template<class IN_T, class OUT_T, class TAP_T>
fir_filter<IN_T, OUT_T, TAP_T>::fir_filter(const std::vector<TAP_T> &taps) : d_output(1)
{
	d_align = volk_get_alignment();
	d_naligned = std::max(static_cast<size_t>(1), d_align / sizeof(IN_T));
	set_taps(taps);
}

template<class IN_T, class OUT_T, class TAP_T>
void fir_filter<IN_T, OUT_T, TAP_T>::set_taps(const std::vector<TAP_T> &taps)
{
	d_ntaps = static_cast<int>(taps.size());
	d_taps = taps;
	std::reverse(d_taps.begin(), d_taps.end());

	d_aligned_taps.clear();
	d_aligned_taps = std::vector<volk::vector<TAP_T> >(
		d_naligned,
		volk::vector<TAP_T>((d_ntaps + d_naligned - 1), 0));
	for (int i = 0; i < d_naligned; i++)
	{
		for (unsigned int j = 0; j < d_ntaps; j++)
			d_aligned_taps[i][i + j] = d_taps[j];
	}
}

template<class IN_T, class OUT_T, class TAP_T>
void fir_filter<IN_T, OUT_T, TAP_T>::update_tap(TAP_T t, unsigned int index)
{
	d_taps[index] = t;
	for (int i = 0; i < d_naligned; i++)
	{
		d_aligned_taps[i][i + index] = t;
	}
}

template<class IN_T, class OUT_T, class TAP_T>
std::vector<TAP_T> fir_filter<IN_T, OUT_T, TAP_T>::taps() const
{
	std::vector<TAP_T> t = d_taps;
	std::reverse(t.begin(), t.end());
	return t;
}

template<class IN_T, class OUT_T, class TAP_T>
unsigned int fir_filter<IN_T, OUT_T, TAP_T>::ntaps() const
{
	return d_ntaps;
}

template<class IN_T, class OUT_T, class TAP_T>
void fir_filter<IN_T, OUT_T, TAP_T>::filterN(OUT_T output[],
                                             const IN_T input[],
                                             unsigned long n)
{
	for (unsigned long i = 0; i < n; i++)
	{
		output[i] = filter(&input[i]);
	}
}

template<class IN_T, class OUT_T, class TAP_T>
void fir_filter<IN_T, OUT_T, TAP_T>::filterNdec(OUT_T output[],
                                                const IN_T input[],
                                                unsigned long n,
                                                unsigned int decimate)
{
	unsigned long j = 0;
	for (unsigned long i = 0; i < n; i++)
	{
		output[i] = filter(&input[j]);
		j += decimate;
	}
}

template<>
inline float fir_filter<float, float, float>::filter(const float input[]) const
{
	const float *ar = reinterpret_cast<float *>(reinterpret_cast<size_t>(input) & ~(d_align - 1));
	const unsigned al = input - ar;

	volk_32f_x2_dot_prod_32f_a(
		const_cast<float *>(d_output.data()),
		ar,
		d_aligned_taps[al].data(),
		d_ntaps + al);
	return d_output[0];
}

template<>
inline cfloat_t
fir_filter<cfloat_t, cfloat_t, float>::filter(const cfloat_t input[]) const
{
	const cfloat_t *ar = reinterpret_cast<cfloat_t *>(reinterpret_cast<size_t>(input) & ~(d_align - 1));
	const unsigned al = input - ar;

	volk_32fc_32f_dot_prod_32fc_a(const_cast<cfloat_t *>(d_output.data()),
	                              ar,
	                              d_aligned_taps[al].data(),
	                              (d_ntaps + al));
	return d_output[0];
}

template<>
inline cfloat_t fir_filter<float, cfloat_t, cfloat_t>::filter(
	const float input[]) const
{
	const float *ar = reinterpret_cast<float *>(reinterpret_cast<size_t>(input) & ~(d_align - 1));
	unsigned al = input - ar;

	volk_32fc_32f_dot_prod_32fc_a(const_cast<cfloat_t *>(d_output.data()),
	                              d_aligned_taps[al].data(),
	                              ar,
	                              (d_ntaps + al));
	return d_output[0];
}

template<>
inline cfloat_t
fir_filter<cfloat_t, cfloat_t, cfloat_t>::filter(
	const cfloat_t input[]) const
{
	const cfloat_t *ar = reinterpret_cast<cfloat_t *>(reinterpret_cast<size_t>(input) & ~(d_align - 1));
	const unsigned al = input - ar;

	volk_32fc_x2_dot_prod_32fc_a(const_cast<cfloat_t *>(d_output.data()),
	                             ar,
	                             d_aligned_taps[al].data(),
	                             (d_ntaps + al));
	return d_output[0];
}

template<>
inline cfloat_t
fir_filter<std::int16_t, cfloat_t, cfloat_t>::filter(const std::int16_t input[]) const
{
	const std::int16_t *ar = reinterpret_cast<std::int16_t *>((size_t)input & ~(d_align - 1));
	const unsigned al = input - ar;

	volk_16i_32fc_dot_prod_32fc_a(const_cast<cfloat_t *>(d_output.data()),
	                              ar,
	                              d_aligned_taps[al].data(),
	                              (d_ntaps + al));

	return d_output[0];
}

template<>
inline short fir_filter<float, std::int16_t, float>::filter(const float input[]) const
{
	const float *ar = reinterpret_cast<float *>(reinterpret_cast<size_t>(input) & ~(d_align - 1));
	const unsigned al = input - ar;

	volk_32f_x2_dot_prod_16i_a(const_cast<std::int16_t *>(d_output.data()),
	                           ar,
	                           d_aligned_taps[al].data(),
	                           (d_ntaps + al));

	return d_output[0];
}

template<>
inline cint16_t fir_filter<cint16_t, cint16_t, cint16_t>::filter(
	const cint16_t input[]) const
{
	int32_t acc_real = 0;
	int32_t acc_imag = 0;

	for (int i = 0; i < d_taps.size(); ++i)
	{
		int32_t ar = input[i].real();
		int32_t ai = input[i].imag();
		int32_t br = d_taps[i].real();
		int32_t bi = d_taps[i].imag();

		acc_real += ar * br - ai * bi;
		acc_imag += ar * bi + ai * br;
	}

	// Scale back to Q15
	acc_real >>= 15;
	acc_imag >>= 15;

	// const cint16_t *ar = reinterpret_cast<cint16_t *>(reinterpret_cast<size_t>(input) & ~(d_align - 1));
	// const unsigned al = input - ar;
	// volk_16ic_x2_dot_prod_16ic_a(const_cast<cint16_t *>(d_output.data()),
	//                              ar,
	//                              d_aligned_taps[al].data(),
	//                              (d_ntaps + al));
	//
	// return output[0];
	return {
		static_cast<int16_t>(acc_real),
		static_cast<int16_t>(acc_imag)
	};
}

#endif //FIRFILTER_H
