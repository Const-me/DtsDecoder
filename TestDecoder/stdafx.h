#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include <array>
#include <vector>
#include <algorithm>

#include <atltypes.h>
#include <atlstr.h>
#include <atlfile.h>
#include <atlpath.h>

#define CHECK( hr ) { const HRESULT _hr = ( hr ); if( FAILED( _hr ) ) __debugbreak(); }

template<class E>
inline size_t sizeofVector( const std::vector<E>& vec )
{
	return vec.size() * sizeof( E );
}