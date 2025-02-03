//
// Created by TheDaChicken on 6/19/2024.
//

#include "FirdecimQ15.h"

#define WINDOW_SIZE 2048

Halfband_16::Halfband_16(const std::vector<int16_t> &taps)
{
  ntaps = taps.size() == 32 ? 32 : 15;
  m_taps.resize(taps.size() * 2);
  reset();

  // reverse order so we can push into the window
  // duplicate for neon
  for (unsigned int i = 0; i < taps.size(); ++i)
  {
	m_taps[i * 2] = taps[taps.size() - 1 - i];
	m_taps[i * 2 + 1] = taps[taps.size() - 1 - i];
  }

  m_window.resize(WINDOW_SIZE);
}

void Halfband_16::reset()
{
  idx = ntaps - 1;
}

void Halfband_16::push(cint16_t x)
{
  if (idx == WINDOW_SIZE)
  {
	for (unsigned int i = 0; i < ntaps - 1; i++)
	  m_window[i] = m_window[idx - ntaps + 1 + i];
	idx = ntaps - 1;
  }
  m_window[idx++] = x;
}

static cint16_t dotprod_halfband_4(cint16_t *a, const int16_t *b)
{
  cint16_t sum = {0};
  int i;

  for (i = 0; i < 7; i += 2)
  {
	int16_t r = (a[i].real() + a[14 - i].real()) * b[i] >> 15;
	int16_t t = (a[i].imag() + a[14 - i].imag()) * b[i] >> 15;
	sum += std::complex(r, t);
  }
  sum += a[7];
  return sum;
}

void Halfband_16::execute(const cint16_t *x, cint16_t *y)
{
  push(x[0]);
  *y = dotprod_halfband_4(&m_window[idx - ntaps], m_taps.data());
  push(x[1]);
}

