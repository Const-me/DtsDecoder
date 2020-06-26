#pragma once

// Applies volume to 16-bit PCM samples.
class ApplyVolume
{
	const int16x8_t mul;

	static __forceinline int16x8_t makeVolumeMultiplier( uint8_t volumeByte )
	{
		assert( volumeByte < 0xFF );
		const int vol = volumeByte;
		const int16_t v = (int16_t)( ( vol * 0x8000 ) / 0xFF );
#ifdef _MSC_VER
		return _mm_set1_epi16( v );
#else
		return vdupq_n_s16( v );
#endif
	}

public:

	__forceinline ApplyVolume( uint8_t volume ) :
		mul( makeVolumeMultiplier( volume ) ) { }

	__forceinline ApplyVolume( const ApplyVolume& that ) :
		mul( that.mul ) { }

	__forceinline int16x8_t apply( int16x8_t src ) const
	{
#ifdef _MSC_VER
		return _mm_mulhrs_epi16( src, mul );
#else
		return vqrdmulhq_s16( src, mul );
#endif
	}
};

// Doesn't apply no volume to the samples.
struct MaxVolume
{
	__forceinline int16x8_t apply( int16x8_t src ) const
	{
		return src;
	}
};