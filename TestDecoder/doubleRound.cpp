#include "stdafx.h"
#include <fenv.h>
#include <inttypes.h>

union DoubleBits
{
	int64_t i64;
	uint64_t u64;
	double f64;
};

__forceinline int64_t toInt( double d )
{
	static_assert( sizeof( DoubleBits ) == 8 );
	DoubleBits bits;
	bits.f64 = d;

	std::array<char, 8> buffer;
	memcpy( buffer.data(), &d, 8 );
	int64_t r;
	memcpy( &r, buffer.data(), 8 );

	return bits.i64;
}

__forceinline double toDouble( uint64_t d )
{
	DoubleBits bits;
	bits.u64 = d;
	return bits.f64;
}

// Doubles have 53 bits of mantissa.
// We want 32-bit value in the least significant 32-bit of exponent.
// Integer zero therefore maps to the 0x80000000 in these bits.

inline double makeIntMagicNumber()
{
	constexpr int ExponentBias = 1023;
	constexpr int mantissaBits = 52;
	uint64_t res;
	// Set exponent
	res = ExponentBias + 52;
	res = res << mantissaBits;
	// Set mantissa
	res |= 0x80000000;
	return toDouble( res );
}

void testDoubles()
{
	const double dblMagic = makeIntMagicNumber();
	const int64_t intMagic = toInt( dblMagic );
	printf( "constexpr double intFromDouble_dbl = %A\n", dblMagic );
	printf( "constexpr int64_t intFromDouble_int = 0x%" PRIx64 "\n", intMagic );

	constexpr int count = 6;
	std::array<double, count> dbl = {
		0,
		INT_MIN - 0.49,
		INT_MIN + 0.49,
		INT_MIN + 0.51,
		INT_MAX,
		INT_MAX + 1254568.3254
	};
	std::array<int, count> result;

	fesetround( FE_DOWNWARD );

	for( int j = 0; j < count; j++ )
	{
		const double src = dbl[ j ];
		double d = src;
		d += dblMagic;
		int64_t i64 = toInt( d );
		i64 -= intMagic;
		i64 = std::max( i64, (int64_t)INT_MIN );
		i64 = std::min( i64, (int64_t)INT_MAX );

		const int i32 = (int)i64;
		result[ j ] = i32;

		const int stdlib = lrint( src );
		printf( "%f: stdlib %i, my %i\n", src, stdlib, i32 );
	}
	__debugbreak();
}