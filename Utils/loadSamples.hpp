#pragma once

// Load 4 integers, 32 bit / each
__forceinline int32x4_t load4( const int* src )
{
#ifdef _MSC_VER
	return _mm_loadu_si128( ( const __m128i* )src );
#else
	return vld1q_s32( src );
#endif
}

template<int bits>
__forceinline int16x8_t loadSamples( const int* src );

// Convert signed 24-bit samples into 16 bit. This uses saturation, i.e. clips them to the range.
template<>
__forceinline int16x8_t loadSamples<24>( const int* src )
{
	int32x4_t v1 = load4( src );
	int32x4_t v2 = load4( src + 4 );
#ifdef _MSC_VER
	v1 = _mm_srai_epi32( v1, 8 );
	v2 = _mm_srai_epi32( v2, 8 );
	return _mm_packs_epi32( v1, v2 );
#else
	// reads each vector element in the source SIMD&FP register, right shifts each result by an immediate value,
	// saturates each shifted result to a value that is half the original width, puts the final result into a vector, 
	// and writes the vector to the lower or upper half of the destination SIMD&FP register.
	const int16x4_t s1 = vqshrn_n_s32( v1, 8 );
	const int16x4_t s2 = vqshrn_n_s32( v2, 8 );
	return vcombine_s16( s1, s2 );
#endif
}

// Convert signed 32-bit samples into 16 bit. No saturation is needed, it just discards the lower 16 bits of the values
template<>
__forceinline int16x8_t loadSamples<32>( const int* src )
{
	int32x4_t v1 = load4( src );
	int32x4_t v2 = load4( src + 4 );
#ifdef _MSC_VER
	v1 = _mm_srai_epi32( v1, 16 );
	return _mm_blend_epi16( v1, v2, 0b10101010 );
#else
	const int16x4_t s1 = vqshrn_n_s32( v1, 16 );
	const int16x4_t s2 = vqshrn_n_s32( v2, 16 );
	return vcombine_s16( s1, s2 );
#endif
}

// Pack 16-bit samples using saturation
template<>
__forceinline int16x8_t loadSamples<16>( const int* src )
{
	int32x4_t v1 = load4( src );
	int32x4_t v2 = load4( src + 4 );
#ifdef _MSC_VER
	return _mm_packs_epi32( v1, v2 );
#else
	const int16x4_t s1 = vqmovn_s32( v1 );
	const int16x4_t s2 = vqmovn_s32( v2 );
	return vcombine_s16( s1, s2 );
#endif
}