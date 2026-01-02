//
// Created by TheDaChicken on 7/3/2025.
//
#include <fstream>
#include <gtest/gtest.h>

#include "dsp/ArbResampler.h"
#include "dsp/Firdes.h"
#include "utils/Log.h"

std::vector<float> test_fir_filter(std::vector<float> &src_data,
                                   const std::vector<float> &taps,
                                   int decim)
{
	std::vector<float> y;
	for (size_t i = 0; i < src_data.size(); i += decim)
	{
		float sum = 0.0f;
		for (size_t j = 0; j < taps.size(); ++j)
		{
			if (i + j < src_data.size())
			{
				sum += src_data[i + j] * taps[j];
			}
		}
		y.push_back(sum);
	}
	return y;
}

TEST(ResamplerTest, FIRFilter)
{
	int decim = 1;
	std::vector<float> taps;

	// 20 * 2 = 40 taps
	for (int i = 0; i < 20; ++i)
	{
		taps.push_back(0.5f);
		taps.push_back(0.5f);
	}

	std::vector<float> src_data;

	// 4 * 40 = 160 samples
	for (int i = 0; i < 40; i++)
	{
		src_data.push_back(1);
		src_data.push_back(2);
		src_data.push_back(3);
		src_data.push_back(4);
	}
	std::vector<float> expected_output = test_fir_filter(src_data, taps, decim);

	fir_filter_fff fir_filter(taps);

	// Fir Filter Test
	for (int i = 0; i < src_data.size(); i += decim)
	{
		float output = fir_filter.filter(&src_data[i]);
		ASSERT_NEAR(output, expected_output[i / decim], 0.01f)
			<< "Output mismatch at index " << i / decim;
	}
}

TEST(ResamplerTest, TestHistoryBuffer)
{
	const size_t history = 4;
	const size_t block_size = 3;
	HistoryBuffer<int> buf;
	buf.SetHistorySize(history);
	buf.SetBlockSize(4);
	buf.Reset();

	// Fill buffer with two blocks of data
	const int input1[] = {1, 2, 3};
	const int input2[] = {4, 5, 6};
	buf.Write(input1, block_size);
	buf.Write(input2, block_size);

	// Consume first block
	buf.Consume(block_size);

	// After consume, buffer should retain last (history-1) = 3 samples
	const int* data = buf.Get();
	EXPECT_EQ(data[0], 1);
	EXPECT_EQ(data[1], 2);
	EXPECT_EQ(data[2], 3);
	EXPECT_EQ(data[3], 4);
	EXPECT_EQ(data[4], 5); // Retained from second block
	EXPECT_EQ(data[5], 6);

	// Write third block
	int input3[] = {7, 8, 9};
	buf.Write(input3, block_size);

	// Consume second block
	buf.Consume(block_size + 1);

	data = buf.Get();
	EXPECT_EQ(data[0], 5);
	EXPECT_EQ(data[1], 6);
	EXPECT_EQ(data[2], 7);
	EXPECT_EQ(data[3], 8);
}

TEST(ResamplerTest, TestOutput)
{
	int N = 50000; // Number of samples
	float fs = 5000.0; // Sample rate
	float rrate = 0.715;

	int nfilts = 32;
	std::vector<cfloat_t> taps = firdes::complex_band_pass_2(
		nfilts,
		nfilts * fs,
		50,
		400,
		fs / 10,
		// Stop attenuation
		80,
		Window::WIN_BLACKMAN_hARRIS,
		0.0f
	);

	float freq = 211.123;

	std::vector<cfloat_t> input(N);

	for (int i = 0; i < N; ++i)
	{
		float real = std::cosf(2 * M_PI * freq * i / fs);;
		float imag = std::sinf(2 * M_PI * freq * i / fs);

		input[i] = cfloat_t(real, imag);
	}

	ArbResampler<cfloat_t, cfloat_t> resampler(rrate, taps, nfilts);

	int delay = resampler.GroupDelay();
	float phase = resampler.PhaseOffset(freq, fs);

	std::vector<cfloat_t> output(N * rrate + 152);

	int p;
	int resampled_size = resampler.ProcessBlock(output.data(), input.data(), N, p);
	ASSERT_GT(resampled_size, 0) << "Resampling failed";

	std::vector<cfloat_t> expected_floats(resampled_size);

	int n = -delay;

	for (int i = 0; i < resampled_size; ++i)
	{
		float t = static_cast<float>(n) / (fs * rrate);

		expected_floats[i] = cfloat_t(
			std::cosf(2 * M_PI * freq * t + phase),
			std::sinf(2 * M_PI * freq * t + phase)
		);

		n++;
	}

	std::ofstream output_file("resampled_output.raw", std::ios::binary);
	std::ofstream expected_file("expected_output.raw", std::ios::binary);

	for (int i = 0; i < resampled_size; ++i)
	{
		float real = output[i].real();
		float imag = output[i].imag();

		EXPECT_NEAR(real, expected_floats[i].real(), 0.01f) << "Real part mismatch at index " << i;
		EXPECT_NEAR(imag, expected_floats[i].imag(), 0.01f) << "Imaginary part mismatch at index " << i;

		output_file.write(reinterpret_cast<const char *>(&output[i]), sizeof(cfloat_t));
		expected_file.write(reinterpret_cast<const char *>(&expected_floats[i]), sizeof(cfloat_t));
	}
}