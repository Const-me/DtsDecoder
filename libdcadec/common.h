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
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <vector>
#include <array>

#include "compiler.h"
#ifdef _MSC_VER
#include "emmintrin.h"
#include "smmintrin.h"
#include "tmmintrin.h"
#include <intrin.h>
#else
#include <arm_neon.h>
#define __forceinline inline __attribute__((always_inline))
#endif

#include "memory.hpp"

__forceinline int dca_clz( uint32_t x )
{
#ifdef _MSC_VER
	unsigned long idx;
	uint8_t res = _BitScanReverse( &idx, x );
	return res ? 31 - idx : 32;
#else
	return __builtin_clz( x );
#endif
}

__forceinline int dca_popcount( uint32_t x )
{
#ifdef _MSC_VER
	return __popcnt( x );
#else
	return __builtin_popcount( x );
#endif
}

#define dca_countof(x)  (sizeof(x) / sizeof((x)[0]))

__forceinline uint16_t dca_bswap16( uint16_t x )
{
#ifdef _MSC_VER
	return _byteswap_ushort( x );
#else
	return __builtin_bswap16( x );
#endif
}

__forceinline uint32_t dca_bswap32( uint32_t x )
{
#ifdef _MSC_VER
	return _byteswap_ulong( x );
#else
	return __builtin_bswap32( x );
#endif
}

__forceinline uint64_t dca_bswap64( uint64_t x )
{
#ifdef _MSC_VER
	return _byteswap_uint64( x );
#else
	return __builtin_bswap64( x );
#endif
}

#define DCA_BSWAP16_C(x)    ((((x) & 0x00ff)   <<  8) | (((x) & 0xff00)  >>  8))
#define DCA_BSWAP32_C(x)    ((DCA_BSWAP16_C(x) << 16) | (DCA_BSWAP16_C(x >> 16)))
#define DCA_BSWAP64_C(x)    ((DCA_BSWAP32_C(x) << 32) | (DCA_BSWAP32_C(x >> 32)))

#if HAVE_BIGENDIAN
#define DCA_16LE(x) dca_bswap16(x)
#define DCA_32LE(x) dca_bswap32(x)
#define DCA_64LE(x) dca_bswap64(x)
#define DCA_16BE(x) ((uint16_t)(x))
#define DCA_32BE(x) ((uint32_t)(x))
#define DCA_64BE(x) ((uint64_t)(x))
#define DCA_16LE_C(x)   DCA_BSWAP16_C(x)
#define DCA_32LE_C(x)   DCA_BSWAP32_C(x)
#define DCA_64LE_C(x)   DCA_BSWAP64_C(x)
#define DCA_16BE_C(x)   (x)
#define DCA_32BE_C(x)   (x)
#define DCA_64BE_C(x)   (x)
#else
#define DCA_16LE(x) ((uint16_t)(x))
#define DCA_32LE(x) ((uint32_t)(x))
#define DCA_64LE(x) ((uint64_t)(x))
#define DCA_16BE(x) dca_bswap16(x)
#define DCA_32BE(x) dca_bswap32(x)
#define DCA_64BE(x) dca_bswap64(x)
#define DCA_16LE_C(x)   (x)
#define DCA_32LE_C(x)   (x)
#define DCA_64LE_C(x)   (x)
#define DCA_16BE_C(x)   DCA_BSWAP16_C(x)
#define DCA_32BE_C(x)   DCA_BSWAP32_C(x)
#define DCA_64BE_C(x)   DCA_BSWAP64_C(x)
#endif

#define DCA_MIN(a, b)   ((a) < (b) ? (a) : (b))
#define DCA_MAX(a, b)   ((a) > (b) ? (a) : (b))

#define DCA_ALIGN(value, align) \
    (((value) + (align) - 1) & ~((align) - 1))

#define DCA_SET_BIT(map, bit) \
    ((map)[(bit) >> 3] |= 1 << ((bit) & 7))

#define DCA_TEST_BIT(map, bit) \
    ((map)[(bit) >> 3] >> ((bit) & 7) & 1)

#define DCA_MEM16BE(data) \
    (((uint32_t)(data)[0] <<  8) | (data)[1])

#define DCA_MEM24BE(data) \
    (((uint32_t)(data)[0] << 16) | DCA_MEM16BE(&(data)[1]))

#define DCA_MEM32BE(data) \
    (((uint32_t)(data)[0] << 24) | DCA_MEM24BE(&(data)[1]))

#define DCA_MEM40BE(data) \
    (((uint64_t)(data)[0] << 32) | DCA_MEM32BE(&(data)[1]))

#define DCA_MEM16LE(data) \
    (((uint32_t)(data)[1] <<  8) | (data)[0])

#define DCA_MEM24LE(data) \
    (((uint32_t)(data)[2] << 16) | DCA_MEM16LE(data))

#define DCA_MEM32LE(data) \
    (((uint32_t)(data)[3] << 24) | DCA_MEM24LE(data))

#define DCA_MEM40LE(data) \
    (((uint64_t)(data)[4] << 32) | DCA_MEM32LE(data))

static inline uint32_t DCA_MEM32NE( const void *data )
{
	uint32_t res;
	memcpy( &res, data, sizeof( res ) );
	return res;
}

void dca_format_log( struct dcadec_context *dca, int level,
	const char *file, int line, const char *fmt, ... )
	__attribute__( ( format( printf, 5, 6 ) ) );

#define DCADEC_LOG_ONCE     0x80000000

#define dca_log(obj, lvl, ...) \
    dca_format_log((obj)->ctx, DCADEC_LOG_##lvl, __FILE__, __LINE__, __VA_ARGS__)

#define dca_log_once(obj, lvl, ...) \
    dca_format_log((obj)->ctx, DCADEC_LOG_##lvl | DCADEC_LOG_ONCE, __FILE__, __LINE__, __VA_ARGS__)

#define DCADEC_FLAG_KEEP_DMIX_MASK  \
    (DCADEC_FLAG_KEEP_DMIX_2CH | DCADEC_FLAG_KEEP_DMIX_6CH)

#define SPEAKER_LAYOUT_MONO         (SPEAKER_MASK_C)
#define SPEAKER_LAYOUT_STEREO       (SPEAKER_MASK_L | SPEAKER_MASK_R)
#define SPEAKER_LAYOUT_2POINT1      (SPEAKER_LAYOUT_STEREO | SPEAKER_MASK_LFE1)
#define SPEAKER_LAYOUT_3_0          (SPEAKER_LAYOUT_STEREO | SPEAKER_MASK_C)
#define SPEAKER_LAYOUT_2_1          (SPEAKER_LAYOUT_STEREO | SPEAKER_MASK_Cs)
#define SPEAKER_LAYOUT_3_1          (SPEAKER_LAYOUT_3_0 | SPEAKER_MASK_Cs)
#define SPEAKER_LAYOUT_2_2          (SPEAKER_LAYOUT_STEREO | SPEAKER_MASK_Ls | SPEAKER_MASK_Rs)
#define SPEAKER_LAYOUT_5POINT0      (SPEAKER_LAYOUT_3_0 | SPEAKER_MASK_Ls | SPEAKER_MASK_Rs)
#define SPEAKER_LAYOUT_7POINT0_WIDE (SPEAKER_LAYOUT_5POINT0 | SPEAKER_MASK_Lw | SPEAKER_MASK_Rw)
#define SPEAKER_LAYOUT_7POINT1_WIDE (SPEAKER_LAYOUT_7POINT0_WIDE | SPEAKER_MASK_LFE1)

enum WaveTag {
	TAG_RIFF = 0x46464952,
	TAG_WAVE = 0x45564157,
	TAG_data = 0x61746164,
	TAG_fmt = 0x20746d66
};

// WAVEFORMATEXTENSIBLE speakers
enum WaveSpeaker {
	WAVESPKR_FL, WAVESPKR_FR, WAVESPKR_FC, WAVESPKR_LFE,
	WAVESPKR_BL, WAVESPKR_BR, WAVESPKR_FLC, WAVESPKR_FRC,
	WAVESPKR_BC, WAVESPKR_SL, WAVESPKR_SR, WAVESPKR_TC,
	WAVESPKR_TFL, WAVESPKR_TFC, WAVESPKR_TFR, WAVESPKR_TBL,
	WAVESPKR_TBC, WAVESPKR_TBR,

	WAVESPKR_COUNT
};

// Table 6-22: Loudspeaker masks
enum SpeakerMask {
	SPEAKER_MASK_C = 0x00000001,
	SPEAKER_MASK_L = 0x00000002,
	SPEAKER_MASK_R = 0x00000004,
	SPEAKER_MASK_Ls = 0x00000008,
	SPEAKER_MASK_Rs = 0x00000010,
	SPEAKER_MASK_LFE1 = 0x00000020,
	SPEAKER_MASK_Cs = 0x00000040,
	SPEAKER_MASK_Lsr = 0x00000080,
	SPEAKER_MASK_Rsr = 0x00000100,
	SPEAKER_MASK_Lss = 0x00000200,
	SPEAKER_MASK_Rss = 0x00000400,
	SPEAKER_MASK_Lc = 0x00000800,
	SPEAKER_MASK_Rc = 0x00001000,
	SPEAKER_MASK_Lh = 0x00002000,
	SPEAKER_MASK_Ch = 0x00004000,
	SPEAKER_MASK_Rh = 0x00008000,
	SPEAKER_MASK_LFE2 = 0x00010000,
	SPEAKER_MASK_Lw = 0x00020000,
	SPEAKER_MASK_Rw = 0x00040000,
	SPEAKER_MASK_Oh = 0x00080000,
	SPEAKER_MASK_Lhs = 0x00100000,
	SPEAKER_MASK_Rhs = 0x00200000,
	SPEAKER_MASK_Chr = 0x00400000,
	SPEAKER_MASK_Lhr = 0x00800000,
	SPEAKER_MASK_Rhr = 0x01000000,
	SPEAKER_MASK_Cl = 0x02000000,
	SPEAKER_MASK_Ll = 0x04000000,
	SPEAKER_MASK_Rl = 0x08000000,
	SPEAKER_MASK_RSV1 = 0x10000000,
	SPEAKER_MASK_RSV2 = 0x20000000,
	SPEAKER_MASK_RSV3 = 0x40000000,
	SPEAKER_MASK_RSV4 = 0x80000000
};

// Table 6-22: Loudspeaker masks
enum Speaker {
	SPEAKER_C, SPEAKER_L, SPEAKER_R, SPEAKER_Ls,
	SPEAKER_Rs, SPEAKER_LFE1, SPEAKER_Cs, SPEAKER_Lsr,
	SPEAKER_Rsr, SPEAKER_Lss, SPEAKER_Rss, SPEAKER_Lc,
	SPEAKER_Rc, SPEAKER_Lh, SPEAKER_Ch, SPEAKER_Rh,
	SPEAKER_LFE2, SPEAKER_Lw, SPEAKER_Rw, SPEAKER_Oh,
	SPEAKER_Lhs, SPEAKER_Rhs, SPEAKER_Chr, SPEAKER_Lhr,
	SPEAKER_Rhr, SPEAKER_Cl, SPEAKER_Ll, SPEAKER_Rl,
	SPEAKER_RSV1, SPEAKER_RSV2, SPEAKER_RSV3, SPEAKER_RSV4,

	SPEAKER_COUNT
};

// Table 7-1: Sync words
enum SyncWord {
	SYNC_WORD_CORE = 0x7ffe8001,
	SYNC_WORD_CORE_LE = 0xfe7f0180,
	SYNC_WORD_CORE_LE14 = 0xff1f00e8,
	SYNC_WORD_CORE_BE14 = 0x1fffe800,
	SYNC_WORD_REV1AUX = 0x9a1105a0,
	SYNC_WORD_REV2AUX = 0x7004c070,
	SYNC_WORD_XCH = 0x5a5a5a5a,
	SYNC_WORD_XXCH = 0x47004a03,
	SYNC_WORD_X96 = 0x1d95f262,
	SYNC_WORD_XBR = 0x655e315e,
	SYNC_WORD_LBR = 0x0a801921,
	SYNC_WORD_XLL = 0x41a29547,
	SYNC_WORD_EXSS = 0x64582025,
	SYNC_WORD_EXSS_LE = 0x58642520,
	SYNC_WORD_CORE_EXSS = 0x02b09261,
};

// Table 7-10: Loudspeaker bit mask for speaker activity
enum SpeakerPair {
	SPEAKER_PAIR_C = 0x0001,
	SPEAKER_PAIR_LR = 0x0002,
	SPEAKER_PAIR_LsRs = 0x0004,
	SPEAKER_PAIR_LFE1 = 0x0008,
	SPEAKER_PAIR_Cs = 0x0010,
	SPEAKER_PAIR_LhRh = 0x0020,
	SPEAKER_PAIR_LsrRsr = 0x0040,
	SPEAKER_PAIR_Ch = 0x0080,
	SPEAKER_PAIR_Oh = 0x0100,
	SPEAKER_PAIR_LcRc = 0x0200,
	SPEAKER_PAIR_LwRw = 0x0400,
	SPEAKER_PAIR_LssRss = 0x0800,
	SPEAKER_PAIR_LFE2 = 0x1000,
	SPEAKER_PAIR_LhsRhs = 0x2000,
	SPEAKER_PAIR_Chr = 0x4000,
	SPEAKER_PAIR_LhrRhr = 0x8000,
	SPEAKER_PAIR_ALL_1 = 0x5199,
	SPEAKER_PAIR_ALL_2 = 0xae66
};

static inline int count_chs_for_mask( int mask )
{
	return dca_popcount( mask ) + dca_popcount( mask & SPEAKER_PAIR_ALL_2 );
}

// Table 7-11: Representation type
enum RepresentationType {
	REPR_TYPE_LtRt = 2,
	REPR_TYPE_LhRh = 3
};

// Table 7-15: Core/extension mask
enum ExtensionMask {
	CSS_CORE = 0x001,
	CSS_XXCH = 0x002,
	CSS_X96 = 0x004,
	CSS_XCH = 0x008,
	EXSS_CORE = 0x010,
	EXSS_XBR = 0x020,
	EXSS_XXCH = 0x040,
	EXSS_X96 = 0x080,
	EXSS_LBR = 0x100,
	EXSS_XLL = 0x200,
	EXSS_RSV1 = 0x400,
	EXSS_RSV2 = 0x800
};

// Table 8-8: Downmix type
enum DownMixType {
	DMIX_TYPE_1_0,
	DMIX_TYPE_LoRo,
	DMIX_TYPE_LtRt,
	DMIX_TYPE_3_0,
	DMIX_TYPE_2_1,
	DMIX_TYPE_2_2,
	DMIX_TYPE_3_1,

	DMIX_TYPE_COUNT
};

#include "core_decoder.h"
#include "exss_parser.h"
#include "xll_decoder.h"
#include "lbr_decoder.h"
#include "dca_context.h"