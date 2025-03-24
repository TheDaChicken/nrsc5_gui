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