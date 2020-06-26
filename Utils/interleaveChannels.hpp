#pragma once
#ifdef _MSC_VER
#include "simdTypes-sse.hpp"
#endif
#include "loadSamples.hpp"
#include "ChannelsData.hpp"
#include "ApplyVolume.hpp"
#include <algorithm>
#include "../globals.h"

// Load 8 samples from each channel.
// Convert them to 16-bit PCM.
// If needed, apply volume multiplier.
// Store the result in ChannelsData template class, which is written in the way that allows compiler to place in registers as opposed to memory.
// Increment source pointers by 8.
template<int bitsPerSample, size_t outputChannels, class TVolume, size_t i = 0>
__forceinline void makePcmData( ChannelsData<outputChannels>& dest, std::array<const int*, outputChannels>& src, const TVolume& vol )
{
	const int16x8_t v = loadSamples<bitsPerSample>( src[ i ] );
	src[ i ] += 8;
	// Unlike VC++, GCC ain't smart enough to find out that method is a template, need `template` keyword otherwise it doesn't compile.
	// https://stackoverflow.com/q/1840253/
	dest.template get<i>() = vol.apply( v );

	constexpr size_t iNext = i + 1;
	if constexpr( iNext < outputChannels )
		makePcmData<bitsPerSample, outputChannels, TVolume, iNext>( dest, src, vol );
}

template<int bitsPerSample, size_t outputChannels, class TVolume>
__forceinline void decodeBlockLoop( int16_t* pcm, std::array<const int*, outputChannels>& src, const TVolume& vol )
{
	const int* const srcEnd = src[ 0 ] + samplesPerBlock;
	constexpr size_t samplesPerLoop = 8 * outputChannels;

	ChannelsData<outputChannels> data;
	while( src[ 0 ] < srcEnd )
	{
		makePcmData<bitsPerSample>( data, src, vol );
		// Store interleaved samples
		data.store( pcm );

		pcm += samplesPerLoop;
	}
}

template<int bitsPerSample, size_t outputChannels>
__forceinline void decodeBlockImpl( int16_t* pcm, std::array<const int*, outputChannels>& src, uint8_t volumeByte )
{
	if( 0xFF == volumeByte )
	{
		MaxVolume vol;
		decodeBlockLoop<bitsPerSample>( pcm, src, vol );
	}
	else
	{
		ApplyVolume vol{ volumeByte };
		decodeBlockLoop<bitsPerSample>( pcm, src, vol );
	}
}