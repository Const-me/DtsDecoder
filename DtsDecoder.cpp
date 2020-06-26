#include "stdafx.h"
#include "DtsDecoder.h"
#include "libdcadec/dca_frame.h"

constexpr int defaultDecoderFlags = 0;

DtsDecoderBase::DtsDecoderBase( uint32_t rate ) :
	m_sampleRate( (int)rate )
{
	// Not using dcadec_context_create, no need for another level of pointers indirection.
	m_dts.flags = defaultDecoderFlags;
	m_dts.core = &m_core;

	/* alloc_core_decoder:
		if (!(dca->core = ta_znew(dca, struct core_decoder)))
			return -DCADEC_ENOMEM;
		dca->core->ctx = dca;
		dca->core->x96_rand = 1;
	*/
	m_core.ctx = &m_dts;
	m_core.x96_rand = 1;
}

HRESULT DtsDecoderBase::decodeFrame( uint8_t* buffer, uint8_t volume, uint8_t outputChannels, int*** samples ) noexcept
{
	try
	{
		size_t size = 0;
		int res = dcadec_frame_parse_header( buffer, &size );
		if( res < 0 )
			return makeErrorCode( -res );

		size_t dstSize = size;
		res = dcadec_frame_convert_bitstream( buffer, &dstSize, buffer, size );
		if( res < 0 )
			return makeErrorCode( -res );

		m_cbConverted = dstSize;

		res = dcadec_context_parse( &m_dts, buffer, m_cbConverted );
		if( res < 0 )
			return makeErrorCode( -res );

		int nsamples, channel_mask, sample_rate, profile;
		res = dcadec_context_filter( &m_dts, samples, &nsamples, &channel_mask, &sample_rate, &m_bitsPerSample, &profile );
		if( res < 0 )
			return makeErrorCode( -res );

		/*
		bits_per_sample	24
		nsamples	512
		profile		1
		sample_rate	48000
		channel_mask	0x0000060f	= 6 set bits, for 5.1
		*/

		// TODO: define error codes for this
		if( outputChannels != dca_popcount( channel_mask ) )
			return E_INVALIDARG;
		if( 0 != ( nsamples % samplesPerBlock ) )
			return E_INVALIDARG;	// DTS has a concept of "termination frames" which can be of any length. Let's hope people ain't gonna use them in practice.
		if( nsamples <= 0 )
			return E_INVALIDARG;
		m_blocksLeft = nsamples / samplesPerBlock;
		m_volume = volume;
		return S_OK;
	}
	catch( HRESULT hr )
	{
		return hr;
	}
}

HRESULT COMLIGHTCALL DtsDecoderBase::clearHistory() noexcept
{
	dcadec_context_clear( &m_dts );
	return S_OK;
}

#include "Utils/interleaveChannels.hpp"

template<uint8_t outputChannels>
HRESULT DtsDecoder<outputChannels>::decodeBlock( int16_t* pcm ) noexcept
{
	if( m_blocksLeft <= 0 )
		return OLE_E_BLANK; // TODO: define a better error code for this

	try
	{
		switch ( m_bitsPerSample )
		{
#define BITS_PER_SAMPLE( n ) case n: decodeBlockImpl<n, (size_t)outputChannels>( pcm, m_samples, m_volume ); break
			BITS_PER_SAMPLE( 24 );
			BITS_PER_SAMPLE( 32 );
			BITS_PER_SAMPLE( 16 );
#undef BITS_PER_SAMPLE
		default:
			return E_INVALIDARG;	// TODO: define an error code for this
		}

		m_blocksLeft--;
		return S_OK;
	}
	catch( HRESULT hr )
	{
		return hr;
	}
}

DLLEXPORT HRESULT COMLIGHTCALL createDcaDecoder( iAudioDecoder** result, uint8_t channels, uint32_t sampleRate )
{
	if( nullptr == result )
		return E_POINTER;

	switch( channels )
	{
#define CREATE_DECODER( n ) case n: return ComLight::Object<DtsDecoder<n>>::create( result, sampleRate )
		CREATE_DECODER( 1 );
		CREATE_DECODER( 2 );
		CREATE_DECODER( 3 );
		CREATE_DECODER( 4 );
		CREATE_DECODER( 5 );
		CREATE_DECODER( 6 );
		CREATE_DECODER( 7 );
		CREATE_DECODER( 8 );
	}
	return E_INVALIDARG;
}

extern "C" const char* COMLIGHTCALL formatMessage( int code )
{
	return dcadec_strerror( -code );
}