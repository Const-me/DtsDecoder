/*
 * This file is part of libdcadec.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "common.h"
#include "interpolator.h"
#include "idct.h"
#include "fixed_math.h"
#include "fir_float.h"

static const double lfe_iir_scale = 0.001985816114019982;

static const double lfe_iir[ 12 ] = {
	-0.9698530866436986,  1.9696040724997900,  0.9999999999999996,
	-1.9643358221499630, -0.9787938538720836,  1.9785545764679620,
	 1.0000000000000020, -1.9951847249255470, -0.9925096137076496,
	 1.9922787089263100,  1.0000000000000000, -1.9974180593495760
};

static inline int convert( double a )
{
	return clip23( lrint( a ) );
}

static void interpolate_lfe( int *pcm_samples, int *lfe_samples, int npcmblocks,
	const double *filter_coeff, int dec_select )
{
	// Select decimation factor
	int factor = 64 << dec_select;
	int ncoeffs = 8 >> dec_select;
	int nlfesamples = npcmblocks >> ( dec_select + 1 );

	// Interpolation
	for( int i = 0; i < nlfesamples; i++ ) {
		int *src = lfe_samples + MAX_LFE_HISTORY + i;

		// One decimated sample generates 64 or 128 interpolated ones
		for( int j = 0; j < factor / 2; j++ ) {
			// Clear accumulation
			double res1 = 0.0;
			double res2 = 0.0;

			// Accumulate
			for( int k = 0; k < ncoeffs; k++ ) {
				res1 += filter_coeff[ j * ncoeffs + k ] * src[ -k ];
				res2 += filter_coeff[ 255 - j * ncoeffs - k ] * src[ -k ];
			}

			// Save interpolated samples
			pcm_samples[ j ] = convert( res1 );
			pcm_samples[ factor / 2 + j ] = convert( res2 );
		}

		// Advance output pointer
		pcm_samples += factor;
	}

	// Update history
	for( int n = MAX_LFE_HISTORY - 1; n >= MAX_LFE_HISTORY - 8; n-- )
		lfe_samples[ n ] = lfe_samples[ nlfesamples + n ];
}

INTERPOLATE_LFE( lfe_float_fir )
{
	(void)dec_select;
	assert( !dec_select );

	interpolate_lfe( pcm_samples, lfe_samples, npcmblocks, lfe_fir_64, 0 );
}

INTERPOLATE_LFE( lfe_float_fir_2x )
{
	(void)dec_select;
	assert( dec_select );

	interpolate_lfe( pcm_samples, lfe_samples, npcmblocks, lfe_fir_128, 1 );
}

INTERPOLATE_LFE( lfe_float_iir )
{
	// Select decimation factor
	int factor = 64 << dec_select;
	int nlfesamples = npcmblocks >> ( dec_select + 1 );

	// Load history
	double lfe_history[ 6 ];
	for( int i = 0; i < 6; i++ )
		lfe_history[ i ] = ( (double *)lfe_samples )[ i ];

	// Interpolation
	for( int i = 0; i < nlfesamples; i++ ) {
		double res1 = lfe_samples[ MAX_LFE_HISTORY + i ] * lfe_iir_scale;
		double res2;

		// One decimated sample generates 64 or 128 interpolated ones
		for( int j = 0; j < factor; j++ ) {
			// Filter
			for( int k = 0; k < 3; k++ ) {
				double tmp1 = lfe_history[ k * 2 + 0 ];
				double tmp2 = lfe_history[ k * 2 + 1 ];

				res2 = tmp1 * lfe_iir[ k * 4 + 0 ] + tmp2 * lfe_iir[ k * 4 + 1 ] + res1;
				res1 = tmp1 * lfe_iir[ k * 4 + 2 ] + tmp2 * lfe_iir[ k * 4 + 3 ] + res2;

				lfe_history[ k * 2 + 0 ] = tmp2;
				lfe_history[ k * 2 + 1 ] = res2;
			}

			// Save interpolated samples
			*pcm_samples++ = convert( res1 );
			res1 = 0.0;
		}
	}

	// Store history
	for( int i = 0; i < 6; i++ )
		( (double *)lfe_samples )[ i ] = lfe_history[ i ];
}

namespace
{
	// These 2 magic numbers were printed by testDoubles() function in TestDecoder\doubleRound.cpp source file.
	constexpr double intFromDouble_dbl = 0X1.0000080000000P+52;
	constexpr int64_t intFromDouble_int = 0x4330000080000000;

	constexpr int clip24_min = -0x800000;
	constexpr int clip24_max = 0x7FFFFF;

#ifdef _MSC_VER
	static const __m128d g_dblZero = _mm_set1_pd( intFromDouble_dbl );
	static const __m128i g_intZero = _mm_set1_epi64x( intFromDouble_int );

	static const __m128i g_clip24Min = _mm_set1_epi32( clip24_min );
	static const __m128i g_clip24Max = _mm_set1_epi32( clip24_max );
#else
	static const int64x2_t g_intZero = vdupq_n_s64( intFromDouble_int );

	union DoubleBits
	{
		int64_t i64;
		uint64_t u64;
		double f64;
	};
	__forceinline uint64_t makeUint( double d )
	{
		DoubleBits bits;
		bits.f64 = d;
		return bits.u64;
	}
	__forceinline int64x2_t makeVector( double a, double b )
	{
		const int64x1_t low = vcreate_s64( makeUint( a ) );
		const int64x1_t high = vcreate_s64( makeUint( b ) );
		return vcombine_s64( low, high );
	}
	static const int32x2_t g_clip24Min = vdup_n_s32( clip24_min );
	static const int32x2_t g_clip24Max = vdup_n_s32( clip24_max );
#endif
}

__forceinline void storeSamples( int& s1, int& s2, double i1, double i2 )
{
#ifdef _DEBUG
	const int oldConvert1 = convert( i1 );
	const int oldConvert2 = convert( i2 );
#endif

#ifdef _MSC_VER
	__m128d doubles = _mm_setr_pd( i1, i2 );
	doubles = _mm_add_pd( doubles, g_dblZero );
	__m128i integers = _mm_castpd_si128( doubles );
	integers = _mm_sub_epi64( integers, g_intZero );
	// Unlike NEON, AMD64 doesn't have instructions to saturate signed 64-bit into 32-bit, nor compute min/max of 64-bit values.
	// It doesn't even have instructions to compare them for greater or less, only for equality (_mm_cmpgt_epi64 is in SSE 4.2 i.e. we can't use it).
	// The following code just ignores the high 32 bits out of 64.
	// The good news is, original implementation fails to handle that case as well, `lrint` CRT function returns 0 on 32-bit overflows instead of clipping.
	integers = _mm_max_epi32( integers, g_clip24Min );
	integers = _mm_min_epi32( integers, g_clip24Max );
	s1 = _mm_cvtsi128_si32( integers );
	s2 = _mm_extract_epi32( integers, 2 );
#else
	i1 += intFromDouble_dbl;
	i2 += intFromDouble_dbl;
	int64x2_t i64 = makeVector( i1, i2 );
	i64 = vsubq_s64( i64, g_intZero );
	// Unlike AMD64, NEON has an instruction to saturate signed 64-bit values into 32-bit. Nice.
	int32x2_t i32 = vqmovn_s64( i64 );
	i32 = vmax_s32( i32, g_clip24Min );
	i32 = vmin_s32( i32, g_clip24Max );
	s1 = vget_lane_s32( i32, 0 );
	s2 = vget_lane_s32( i32, 1 );
#endif

#ifdef _DEBUG
	assert( s1 == oldConvert1 );
	assert( s2 == oldConvert2 );
#endif
}

INTERPOLATE_SUB( sub32_float )
{
	(void)subband_samples_hi;
	assert( !subband_samples_hi );

	// Get history pointer
	double * const history = (double*)dsp->history;

	// Select filter
	const double * const filter_coeff = perfect ? band_fir_perfect : band_fir_nonperfect;

	// Interpolation begins
	for( int sample = 0; sample < nsamples; sample++ )
	{
		// Load in one sample from each subband
		double input[ 32 ];
		for( int i = 0; i < 32; i++ )
			input[ i ] = subband_samples_lo[ i ][ sample ];

		// Inverse DCT
		double output[ 32 ];
		idct_fast( dsp->idct, input, output );

		// Store history
		for( int i = 0, k = 31; i < 16; i++, k-- ) {
			history[ i ] = output[ i ] - output[ k ];
			history[ 16 + i ] = output[ i ] + output[ k ];
		}

		// One subband sample generates 32 interpolated ones
		for( int i = 0, k = 15; i < 16; i++, k-- )
		{
			// Clear accumulation
			double res1 = 0.0;
			double res2 = 0.0;

			// Accumulate
			for( int j = 0; j < 512; j += 64 ) {
				res1 += history[ i + j ] * filter_coeff[ i + j ];
				res2 += history[ k + j ] * filter_coeff[ 16 + i + j ];
			}

			for( int j = 32; j < 512; j += 64 ) {
				res1 += history[ 16 + i + j ] * filter_coeff[ i + j ];
				res2 += history[ 16 + k + j ] * filter_coeff[ 16 + i + j ];
			}

			// Save interpolated samples
			// pcm_samples[ i ] = convert( res1 );
			// pcm_samples[ 16 + i ] = convert( res2 );

			// MSVC profiler on Windows said the `convert` functions above consumed vast majority of CPU time.
			// Very likely, the same happens on ARM Linux as well, no reason for the CRT to be much slower on Windows.
			// Replacing that piece of code with faster version.
			storeSamples( pcm_samples[ i ], pcm_samples[ 16 + i ], res1, res2 );
		}

		// Advance output pointer
		pcm_samples += 32;

		// Shift history
		// for( i = 511; i >= 32; i-- )
		//	history[ i ] = history[ i - 32 ];
		memmove( history + 32, history, sizeof( double ) * ( 512 - 32 ) );
	}
}

INTERPOLATE_SUB( sub64_float )
{
	(void)perfect;

	// Get history pointer
	double *history = (double*)dsp->history;

	// Interpolation begins
	for( int sample = 0; sample < nsamples; sample++ ) {
		int i, j, k;

		// Load in one sample from each subband
		double input[ 64 ];
		if( subband_samples_hi ) {
			// Full 64 subbands, first 32 are residual coded
			for( i = 0; i < 32; i++ )
				input[ i ] = subband_samples_lo[ i ][ sample ] + subband_samples_hi[ i ][ sample ];
			for( i = 32; i < 64; i++ )
				input[ i ] = subband_samples_hi[ i ][ sample ];
		}
		else {
			// Only first 32 subbands
			for( i = 0; i < 32; i++ )
				input[ i ] = subband_samples_lo[ i ][ sample ];
			for( i = 32; i < 64; i++ )
				input[ i ] = 0;
		}

		// Inverse DCT
		double output[ 64 ];
		idct_fast( dsp->idct, input, output );

		// Store history
		for( i = 0, k = 63; i < 32; i++, k-- ) {
			history[ i ] = output[ i ] - output[ k ];
			history[ 32 + i ] = output[ i ] + output[ k ];
		}

		// One subband sample generates 64 interpolated ones
		for( i = 0, k = 31; i < 32; i++, k-- ) {
			// Clear accumulation
			double res1 = 0.0;
			double res2 = 0.0;

			// Accumulate
			for( j = 0; j < 1024; j += 128 ) {
				res1 += history[ i + j ] * band_fir_x96[ i + j ];
				res2 += history[ k + j ] * band_fir_x96[ 32 + i + j ];
			}

			for( j = 64; j < 1024; j += 128 ) {
				res1 += history[ 32 + i + j ] * band_fir_x96[ i + j ];
				res2 += history[ 32 + k + j ] * band_fir_x96[ 32 + i + j ];
			}

			// Save interpolated samples
			pcm_samples[ i ] = convert( res1 );
			pcm_samples[ 32 + i ] = convert( res2 );
		}

		// Advance output pointer
		pcm_samples += 64;

		// Shift history
		for( i = 1023; i >= 64; i-- )
			history[ i ] = history[ i - 64 ];
	}
}
