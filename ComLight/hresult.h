#pragma once
#include <stdint.h>
#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winerror.h>
#include <OleCtl.h>
#else
#include "pal/hresult.h"
#endif

// Unfortunately, Diligent also has this macro. Too bad macros ignore C++ namespaces, have to rename.
#define CL_CHECK( hr ) { const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) return __hr; }

#ifndef _MSC_VER
inline constexpr HRESULT HRESULT_FROM_WIN32( int c )
{
	return c < 0 ? c : ( ( 0xFFFF & c ) | 0x80070000 );
}

constexpr HRESULT OLE_E_BLANK = _HRESULT_TYPEDEF_( 0x80040007 );
constexpr HRESULT E_BOUNDS = _HRESULT_TYPEDEF_( 0x8000000BL ); 

constexpr int ERROR_HANDLE_EOF = 38;
constexpr int ERROR_ALREADY_INITIALIZED = 1247;
constexpr int ERROR_TIMEOUT = 1460;
#endif

constexpr HRESULT E_EOF = HRESULT_FROM_WIN32( ERROR_HANDLE_EOF );
constexpr HRESULT E_ALREADY_INITIALIZED = HRESULT_FROM_WIN32( ERROR_ALREADY_INITIALIZED );
constexpr HRESULT E_TIMEOUT = HRESULT_FROM_WIN32( ERROR_TIMEOUT );