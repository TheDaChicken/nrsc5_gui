//
// Created by TheDaChicken on 7/15/2024.
//
#include <gtest/gtest.h>
#include <nrsc5/Decoder.h>

TEST(Nrsc5Test, OpenPipe) {
  NRSC5::Decoder decoder;
  decoder.OpenPipe();
  decoder.Close();
}

TEST(Nrsc5Test, TwoOpenPipe) {
  NRSC5::Decoder decoder1;
  NRSC5::Decoder decoder2;
  decoder1.OpenPipe();
  decoder2.OpenPipe();
  decoder1.Close();
  decoder2.Close();
}
