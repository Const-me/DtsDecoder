#pragma once

namespace store3
{
	template<char i0, char i1, char i2, char i3, char i4, char i5, char i6, char i7>
	__forceinline __m128i shuffleMask()
	{
#define MAKE_TWO( ii ) ( ii >= 0 ) ? ii * 2: -1, ( ii >= 0 ) ? ( ii * 2 ) + 1 : -1
		return _mm_setr_epi8(
			MAKE_TWO( i0 ),
			MAKE_TWO( i1 ),
			MAKE_TWO( i2 ),
			MAKE_TWO( i3 ),
			MAKE_TWO( i4 ),
			MAKE_TWO( i5 ),
			MAKE_TWO( i6 ),
			MAKE_TWO( i7 )
		);
#undef MAKE_TWO
	}

	constexpr char z = -1;
	static const __m128i r1_c1 = shuffleMask<0, z, z, 1, z, z, 2, z>();
	static const __m128i r1_c2 = shuffleMask<z, 0, z, z, 1, z, z, 2>();
	static const __m128i r1_c3 = shuffleMask<z, z, 0, z, z, 1, z, z>();

	static const __m128i r2_c1 = shuffleMask<z, 3, z, z, 4, z, z, 5>();
	static const __m128i r2_c2 = shuffleMask<z, z, 3, z, z, 4, z, z>();
	static const __m128i r2_c3 = shuffleMask<2, z, z, 3, z, z, 4, z>();

	static const __m128i r3_c1 = shuffleMask<z, z, 6, z, z, 7, z, z>();
	static const __m128i r3_c2 = shuffleMask<5, z, z, 6, z, z, 7, z>();
	static const __m128i r3_c3 = shuffleMask<z, 5, z, z, 6, z, z, 7>();

	__forceinline __m128i shuffle3( __m128i c1, __m128i c2, __m128i c3, const __m128i s1, const __m128i s2, const __m128i s3 )
	{
		c1 = _mm_shuffle_epi8( c1, s1 );
		c2 = _mm_shuffle_epi8( c2, s2 );
		c3 = _mm_shuffle_epi8( c3, s3 );
		return _mm_or_si128( _mm_or_si128( c1, c2 ), c3 );
	}
}

__forceinline void store3sse( __m128i* dest, const std::array<__m128i, 3>& src )
{
	using namespace store3;

	__m128i r = shuffle3( src[ 0 ], src[ 1 ], src[ 2 ], r1_c1, r1_c2, r1_c3 );
	_mm_storeu_si128( dest, r );

	r = shuffle3( src[ 0 ], src[ 1 ], src[ 2 ], r2_c1, r2_c2, r2_c3 );
	_mm_storeu_si128( dest + 1, r );

	r = shuffle3( src[ 0 ], src[ 1 ], src[ 2 ], r3_c1, r3_c2, r3_c3 );
	_mm_storeu_si128( dest + 2, r );
}