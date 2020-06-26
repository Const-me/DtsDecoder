#pragma once
#include "ComLight/comLightCommon.h"

struct DECLSPEC_NOVTABLE iAudioDecoder : public ComLight::IUnknown
{
	DEFINE_INTERFACE_ID( "6ef46e96-22c3-4100-9cee-c093723c0b33" );

	virtual HRESULT COMLIGHTCALL syncAndDecode( uint8_t* buffer, uint8_t volume ) = 0;

	virtual int COMLIGHTCALL blocksLeft() = 0;

	virtual HRESULT COMLIGHTCALL decodeBlock( int16_t* pcm ) = 0;

	virtual HRESULT COMLIGHTCALL clearHistory() = 0;

	virtual int COMLIGHTCALL sampleRate() = 0;
	virtual int COMLIGHTCALL channelsCount() = 0;
	virtual int COMLIGHTCALL blockSize() = 0;
	virtual HRESULT copiesCompressedData() = 0;
};

extern "C" HRESULT COMLIGHTCALL createDcaDecoder( iAudioDecoder** result, uint8_t channels, uint32_t sampleRate );

extern "C" const char* COMLIGHTCALL formatMessage( int code );