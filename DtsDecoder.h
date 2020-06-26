#pragma once
#include "ComLight/comLightServer.h"
#include "decoderLibrary.h"
#include "libdcadec/common.h"
#include <algorithm>
#include "globals.h"

class DtsDecoderBase : public ComLight::ObjectRoot<iAudioDecoder>
{
	const int m_sampleRate;

	int COMLIGHTCALL sampleRate() noexcept override final { return m_sampleRate; }
	int COMLIGHTCALL blockSize() noexcept override final { return samplesPerBlock; }

	HRESULT COMLIGHTCALL copiesCompressedData() noexcept override final { return S_OK; }

	int COMLIGHTCALL blocksLeft() noexcept override final { return m_blocksLeft; }

	HRESULT COMLIGHTCALL clearHistory() noexcept override final;

	size_t m_cbConverted;
	core_decoder m_core;

protected:

	dcadec_context m_dts;
	int m_blocksLeft = 0;
	int m_bitsPerSample = 0;
	uint8_t m_volume = 0xFF;

	HRESULT decodeFrame( uint8_t* buffer, uint8_t volume, uint8_t outputChannels, int*** samples ) noexcept;

public:

	DtsDecoderBase( uint32_t rate );
};

template<uint8_t outputChannels>
class DtsDecoder : public DtsDecoderBase
{
	static constexpr int decoderFlagsDownmixToStereo = DCADEC_FLAG_KEEP_DMIX_2CH | DCADEC_FLAG_KEEP_DMIX_6CH;

	std::array<const int*, outputChannels> m_samples;

	int COMLIGHTCALL channelsCount() noexcept override final { return outputChannels; }

	HRESULT COMLIGHTCALL syncAndDecode( uint8_t* buffer, uint8_t volume ) noexcept override final
	{
		int** samples;
		CL_CHECK( DtsDecoderBase::decodeFrame( buffer, volume, outputChannels, &samples ) );
		std::copy_n( samples, outputChannels, m_samples.data() );
		return S_OK;
	}

public:

	DtsDecoder( uint32_t sampleRate ) :
		DtsDecoderBase( sampleRate )
	{
		if constexpr( 2 == outputChannels )
		{
			// Combination of these 2 flags makes the decoder downnmix everything to stereo, which is what we want from it.
			m_dts.flags = decoderFlagsDownmixToStereo;
		}
	}

	HRESULT COMLIGHTCALL decodeBlock( int16_t* pcm ) noexcept override final;
};