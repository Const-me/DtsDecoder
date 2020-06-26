#pragma once
#include <stdint.h>
#include "ComLight/comLightCommon.h"

// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-erref/0642cb2f-2075-4469-918c-4441e69c548a
constexpr HRESULT makeErrorCode( int code )
{
	constexpr HRESULT FACILITY_DECODER_DTS = 4;
	constexpr HRESULT facility = FACILITY_DECODER_DTS;

	// The magic number below has severity bit = failure, customer bit = customer-defined value, and the facility from the template argument
	constexpr HRESULT mask = 0xA0000000 | ( facility << 16 );
	return mask | (HRESULT)code;
}