#pragma once

template<size_t ch>
class ChannelsData;

template<>
class ChannelsData<1>
{
	int16x8_t val;
public:

	template<size_t i>
	__forceinline int16x8_t& get()
	{
		static_assert( 0 == i );
		return val;
	}

	__forceinline void store( int16_t* pcm ) const
	{
#ifdef _MSC_VER
		_mm_storeu_si128( ( __m128i* )pcm, val );
#else
		vst1q_s16( pcm, val );
#endif
	}
};

template<>
class ChannelsData<2>
{
	int16x8x2_t vals;
public:

	template<size_t i>
	__forceinline int16x8_t& get()
	{
		static_assert( i < 2 );
#ifdef _MSC_VER
		return vals[ i ];
#else
		return vals.val[ i ];
#endif
	}

	__forceinline void store( int16_t* pcm ) const
	{
#ifdef _MSC_VER
		__m128i* const p = ( __m128i* )pcm;
		_mm_storeu_si128( p, _mm_unpacklo_epi16( vals[ 0 ], vals[ 1 ] ) );
		_mm_storeu_si128( p + 1, _mm_unpackhi_epi16( vals[ 0 ], vals[ 1 ] ) );
#else
		vst2q_s16( pcm, vals );
#endif
	}
};

// Interleaving 3 channels is rather tricky on AMD64 due to the lack of vst3q_s16, have to use shuffles for the same effect.
#ifdef _MSC_VER
#include "store3-sse.hpp"
#endif
template<>
class ChannelsData<3>
{
	int16x8x3_t vals;
public:

	template<size_t i>
	__forceinline int16x8_t& get()
	{
		static_assert( i < 3 );
#ifdef _MSC_VER
		return vals[ i ];
#else
		return vals.val[ i ];
#endif
	}

	__forceinline void store( int16_t* pcm ) const
	{
#ifdef _MSC_VER
		store3sse( ( __m128i* )pcm, vals );
#else
		vst3q_s16( pcm, vals );
#endif
	}
};

template<>
class ChannelsData<4>
{
	int16x8x4_t vals;
public:

	template<size_t i>
	__forceinline int16x8_t& get()
	{
		static_assert( i < 4 );
#ifdef _MSC_VER
		return vals[ i ];
#else
		return vals.val[ i ];
#endif
	}

	__forceinline void store( int16_t* pcm ) const
	{
#ifdef _MSC_VER
		__m128i* const dest = ( __m128i* )pcm;

		__m128i c12 = _mm_unpacklo_epi16( vals[ 0 ], vals[ 1 ] );
		__m128i c34 = _mm_unpacklo_epi16( vals[ 2 ], vals[ 3 ] );
		_mm_storeu_si128( dest, _mm_unpacklo_epi32( c12, c34 ) );
		_mm_storeu_si128( dest + 1, _mm_unpackhi_epi32( c12, c34 ) );

		c12 = _mm_unpackhi_epi16( vals[ 0 ], vals[ 1 ] );
		c34 = _mm_unpackhi_epi16( vals[ 2 ], vals[ 3 ] );
		_mm_storeu_si128( dest + 2, _mm_unpacklo_epi32( c12, c34 ) );
		_mm_storeu_si128( dest + 3, _mm_unpackhi_epi32( c12, c34 ) );
#else
		vst4q_s16( pcm, vals );
#endif
	}
};

template<uint8_t lane>
__forceinline int16_t extractLane( int16x8_t reg )
{
#ifdef _MSC_VER
	return (int16_t)_mm_extract_epi16( reg, lane );
#else
	return vgetq_lane_s16( reg, lane );
#endif
}

// BTW, a `for` loop won't compile for that code.
// Both _mm_extract_epi16 and vgetq_lane_s16 encode lane index into the opcode of the instruction, index argument needs to be a compile-time constant.
template<size_t channelsCount, uint32_t i = 0>
__forceinline void storeSamples( int16_t* dest, const std::array<int16x8_t, channelsCount>& data )
{
	constexpr uint32_t vec = i % channelsCount;
	constexpr uint8_t lane = i / channelsCount;
	dest[ i ] = extractLane<lane>( data[ vec ] );

	constexpr uint32_t iNext = i + 1;
	if constexpr( iNext < channelsCount * 8 )
		storeSamples<channelsCount, iNext>( dest, data );
}

// Generic version for > 4 channels, uses scalar stores
template<size_t ch>
class ChannelsData
{
	std::array<int16x8_t, ch> vals;
public:

	template<size_t i>
	__forceinline int16x8_t& get()
	{
		static_assert( i < ch );
		return vals[ i ];
	}

	__forceinline void store( int16_t* pcm ) const
	{
		storeSamples( pcm, vals );
	}
};