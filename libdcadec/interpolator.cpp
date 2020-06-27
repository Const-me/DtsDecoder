// I changed the files on June 26 2020 and June 27 2020
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

void interpolator::create( struct idct_context *parent, int flags )
{
	idct = parent;

	const size_t cbElement = ( flags & DCADEC_FLAG_CORE_BIT_EXACT ) ? sizeof( int ) : sizeof( double );
	const size_t count = ( flags & DCADEC_FLAG_CORE_SYNTH_X96 ) ? 1024 : 512;
	const size_t totalBytes = cbElement * count;

	if( nullptr == history )
		history = details::alignedMalloc( totalBytes );
	else
		history = details::alignedRealloc( history, totalBytes );
	if( !history )
		throw E_OUTOFMEMORY;
	m_totalBytes = totalBytes;
	memset( history, 0, totalBytes );

	if( flags & DCADEC_FLAG_CORE_BIT_EXACT )
	{
		if( flags & DCADEC_FLAG_CORE_SYNTH_X96 )
			interpolate = &interpolate_sub64_fixed;
		else
			interpolate = &interpolate_sub32_fixed;
	}
	else
	{
		if( flags & DCADEC_FLAG_CORE_SYNTH_X96 )
			interpolate = &interpolate_sub64_float;
		else
			interpolate = &interpolate_sub32_float;
	}
}

void interpolator::destroy()
{
	if( history )
	{
		details::alignedFree( history );
		history = nullptr;
	}
	interpolate = nullptr;
	idct = nullptr;
	m_totalBytes = 0;
}

void interpolator::clear()
{
	if( history )
		memset( history, 0, m_totalBytes );
}