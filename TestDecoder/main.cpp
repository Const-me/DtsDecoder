#include "stdafx.h"
#include "../decoderLibrary.h"
#include "../ComLight/comLightClient.h"
using namespace ComLight;

HRESULT readFile( LPCTSTR path, std::vector<uint8_t>& vec )
{
	CAtlFile file;
	CHECK( file.Create( path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING ) );
	ULONGLONG cb;
	CHECK( file.GetSize( cb ) );
	vec.resize( (size_t)cb );
	CHECK( file.Read( vec.data(), (DWORD)vec.size() ) );
	return S_OK;
}

HRESULT writeFile( LPCTSTR path, const std::vector<int16_t>& vec )
{
	CAtlFile file;
	CHECK( file.Create( path, GENERIC_WRITE, 0, CREATE_ALWAYS ) );
	CHECK( file.Write( vec.data(), (DWORD)vec.size() * 2 ) );
	return S_OK;
}

CString fileName( LPCTSTR dir, LPCTSTR name, int i )
{
	CString n;
	n.Format( name, i );
	CPath p;
	p.m_strPath = dir;
	p.Append( n );
	return p.m_strPath;
}

// https://gist.github.com/csukuangfj/c1d1d769606260d436f8674c30662450
void writeWaveFile( LPCTSTR path, const std::vector<int16_t> &pcm )
{
	struct wav_hdr
	{
		/* RIFF Chunk Descriptor */
		uint8_t RIFF[ 4 ] = { 'R', 'I', 'F', 'F' }; // RIFF Header Magic header
		uint32_t ChunkSize;                     // RIFF Chunk Size
		uint8_t WAVE[ 4 ] = { 'W', 'A', 'V', 'E' }; // WAVE Header
		/* "fmt" sub-chunk */
		uint8_t fmt[ 4 ] = { 'f', 'm', 't', ' ' }; // FMT header
		uint32_t Subchunk1Size = 16;           // Size of the fmt chunk
		uint16_t AudioFormat = 1; // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM
								  // Mu-Law, 258=IBM A-Law, 259=ADPCM
		uint16_t NumOfChan = 2;   // Number of channels 1=Mono 2=Sterio
		uint32_t SamplesPerSec = 48000;   // Sampling Frequency in Hz
		uint32_t bytesPerSec = 48000 * 4; // bytes per second
		uint16_t blockAlign = 4;          // 2=16-bit mono, 4=16-bit stereo
		uint16_t bitsPerSample = 16;      // Number of bits per sample
		/* "data" sub-chunk */
		uint8_t Subchunk2ID[ 4 ] = { 'd', 'a', 't', 'a' }; // "data"  string
		uint32_t Subchunk2Size;                        // Sampled data length
	};

	CAtlFile file;

	CHECK( file.Create( path, GENERIC_WRITE, 0, CREATE_ALWAYS ) );

	wav_hdr wav;
	const uint32_t fsize = (uint32_t)sizeofVector( pcm );
	wav.ChunkSize = fsize + sizeof( wav_hdr ) - 8;
	wav.Subchunk2Size = fsize + sizeof( wav_hdr ) - 44;

	CHECK( file.Write( &wav, sizeof( wav ) ) );
	CHECK( file.Write( pcm.data(), fsize ) );
}

HRESULT mainImpl()
{
	ComLight::CComPtr<iAudioDecoder> dec;
	CHECK( createDcaDecoder( &dec, 2, 48000 ) );

	printf( "SPB: %i\n", dec->blockSize() );

	LPCTSTR dirSource = LR"(C:\Temp\2remove\AudioFrames\initial\)";
	LPCTSTR nameSource = L"frame-%04i.bin";
	LPCTSTR dest = LR"(C:\Temp\2remove\AudioFrames\decoded.wav)";
	constexpr uint32_t samples = 153;
	// WaveWriter ww{ dest, samples };

	std::vector<uint8_t> data;
	std::vector<int16_t> pcm;

	// const uint8_t vol = 0xFF;
	const uint8_t vol = 0xFE;

	for( uint32_t i = 1; i <= samples; i++ )
	{
		CString src = fileName( dirSource, nameSource, i );
		CHECK( readFile( src, data ) );
		CHECK( dec->syncAndDecode( data.data(), vol ) );

		// This is to test that copiesCompressedData=true is actually true
		// memset( data.data(), 0, sizeofVector( data ) );

		while( dec->blocksLeft() > 0 )
		{
			const size_t off = pcm.size();
			pcm.resize( off + 0x200 * 2 );	// 2 for stereo
			CHECK( dec->decodeBlock( pcm.data() + off ) );
		}
	}
	writeWaveFile( dest, pcm );
	return S_OK;
}

void testDoubles();

int main()
{
	// testDoubles();
	HRESULT hr = mainImpl();
	return 0;
}