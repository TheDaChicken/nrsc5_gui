//
// Created by TheDaChicken on 5/5/2024.
//

#ifndef NRSC5_GUI_SRC_LIB_UTILS_TYPES_H_
#define NRSC5_GUI_SRC_LIB_UTILS_TYPES_H_

#include "DataBuffer.h"

#include <vector>
#include <cstdint>
#include <complex>

using cfloat_t = std::complex<float>;
using cint16_t = std::complex<int16_t>;
using cint32_t = std::complex<int32_t>;
using cint64_t = std::complex<int64_t>;

using vector_int16_t = std::vector<int16_t>;
using vector_uint8_t = std::vector<uint8_t>;
using vector_complex_t = std::vector<cfloat_t>;
using vector_cint16_t = std::vector<cint16_t>;
using vector_float_t = std::vector<float>;

#endif //NRSC5_GUI_SRC_LIB_UTILS_TYPES_H_
