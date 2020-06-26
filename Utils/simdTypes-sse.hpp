#pragma once

// Just a few typedefs for MSVC
using int32x4_t = __m128i;
using int16x8_t = __m128i;
using int16x8x2_t = std::array<__m128i, 2>;
using int16x8x3_t = std::array<__m128i, 3>;
using int16x8x4_t = std::array<__m128i, 4>;