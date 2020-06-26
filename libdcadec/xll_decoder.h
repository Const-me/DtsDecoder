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

#include "bitstream.h"

#define XLL_MAX_CHANNELS    8
#define XLL_MAX_BANDS       2
#define XLL_MAX_ADAPT_PRED_ORDER    16

#define XLL_DECI_HISTORY    8

#define XLL_BAND_0  0
#define XLL_BAND_1  1

#define XLL_DMIX_SIGNATURE(chs) \
    ((chs)->nchannels | ((chs)->hier_m << 4) | ((chs)->primary_chset << 12) \
     | ((chs)->dmix_embedded << 13) | ((chs)->hier_chset << 14) | ((chs)->dmix_type << 15))

#define xll_err(...)        dca_log(xll, ERROR, __VA_ARGS__)
#define xll_warn(...)       dca_log(xll, WARNING, __VA_ARGS__)
#define xll_verbose(...)    dca_log(xll, VERBOSE, __VA_ARGS__)

#define xll_err_once(...)     dca_log_once(xll, ERROR, __VA_ARGS__)
#define xll_warn_once(...)    dca_log_once(xll, WARNING, __VA_ARGS__)

struct xll_decoder;
struct exss_asset;

struct xll_band
{
	bool    decor_enabled;                      ///< Pairwise channel decorrelation flag
	int     orig_order[ XLL_MAX_CHANNELS ];       ///< Original channel order
	int     decor_coeff[ XLL_MAX_CHANNELS / 2 ];  ///< Pairwise channel coefficients

	int     adapt_pred_order[ XLL_MAX_CHANNELS ]; ///< Adaptive predictor order
	int     highest_pred_order;                 ///< Highest adaptive predictor order
	int     fixed_pred_order[ XLL_MAX_CHANNELS ]; ///< Fixed predictor order
	int     adapt_refl_coeff[ XLL_MAX_CHANNELS ][ XLL_MAX_ADAPT_PRED_ORDER ];   ///< Adaptive predictor reflection coefficients

	bool    dmix_embedded;  ///< Downmix performed by encoder in frequency band

	int     lsb_section_size;                   ///< Size of LSB section in any segment
	int     nscalablelsbs[ XLL_MAX_CHANNELS ];    ///< Number of bits to represent the samples in LSB part
	int     bit_width_adjust[ XLL_MAX_CHANNELS ]; ///< Number of bits discarded by authoring

	int     *msb_sample_buffer[ XLL_MAX_CHANNELS ];   ///< MSB sample buffer pointers
	int     *lsb_sample_buffer[ XLL_MAX_CHANNELS ];   ///< LSB sample buffer pointers or NULL
};

struct xll_chset
{
	struct xll_decoder  *decoder;   ///< Parent context

	// Channel set header
	int     nchannels;          ///< Number of channels in the channel set (N)
	int     residual_encode;    ///< Residual encoding mask (0 - residual, 1 - full channel)
	int     pcm_bit_res;        ///< PCM bit resolution (variable)
	int     storage_bit_res;    ///< Storage bit resolution (16 or 24)
	int     freq;               ///< Original sampling frequency (max. 96000 Hz)
	int     interpolate;        ///< Sampling frequency modifier
	int     replace_set_index;  ///< Which replacement set this channel set is member of

	bool    primary_chset;          ///< Primary channel set flag
	bool    dmix_coeffs_present;    ///< Downmix coefficients present in stream
	bool    dmix_embedded;          ///< Downmix already performed by encoder
	int     dmix_type;              ///< Primary channel set downmix type
	bool    hier_chset;             ///< Whether the channel set is part of a hierarchy
	int     hier_m;                 ///< Number of preceding channels in a hierarchy (M)
	DynamicArray<int> dmix_coeff;            ///< Downmixing coefficients buffer base
	int     *dmix_coeff_cur;        ///< M*N downmixing coefficients for current frame
	int     *dmix_coeff_pre;        ///< M*N downmixing coefficients for previous frame
	int     *dmix_scale_cur;        ///< M downmixing scales for current frame
	int     *dmix_scale_pre;        ///< M downmixing scales for previous frame
	int     *dmix_scale_inv_cur;    ///< M inverse downmixing scales for current frame
	int     *dmix_scale_inv_pre;    ///< M inverse downmixing scales for previous frame
	int     dmix_coeffs_signature;  ///< Signature of downmixing parameters from previous frame
	bool    dmix_coeffs_parity;     ///< Index of current downmixing coefficients buffer, flipped each frame
	bool    ch_mask_enabled;        ///< Channel mask enabled
	int     ch_mask;                ///< Channel mask for set

	int     nfreqbands; ///< Number of frequency bands (1 or 2)
	int     nabits;     ///< Number of bits to read bit allocation coding parameter

	struct xll_band     bands[ XLL_MAX_BANDS ];   ///< Frequency bands

	// Frequency band coding parameters
	bool    seg_common;                                 ///< Segment type
	bool    rice_code_flag[ XLL_MAX_CHANNELS ];           ///< Rice coding flag
	int     bitalloc_hybrid_linear[ XLL_MAX_CHANNELS ];   ///< Binary code length for isolated samples
	int     bitalloc_part_a[ XLL_MAX_CHANNELS ];          ///< Coding parameter for part A of segment
	int     bitalloc_part_b[ XLL_MAX_CHANNELS ];          ///< Coding parameter for part B of segment
	int     nsamples_part_a[ XLL_MAX_CHANNELS ];          ///< Number of samples in part A of segment

	// Decimator history
	int     deci_history[ XLL_MAX_CHANNELS ][ XLL_DECI_HISTORY ];   ///< Decimator history for frequency band 1

	// Sample buffers
	int     *out_sample_buffer[ XLL_MAX_CHANNELS ];   ///< Output sample buffer pointers

	DynamicArray<int> sample_buffer1;    ///< MSB sample buffer base
	DynamicArray<int> sample_buffer2;    ///< LSB sample buffer base
	DynamicArray<int> sample_buffer3;    ///< Frequency band assembly buffer base
};

struct xll_decoder
{
	struct dcadec_context   *ctx; ///< Parent context
	struct bitstream        bits; ///< Bitstream reader

	int     flags = 0;  ///< Context flags

	int     frame_size = 0;             ///< Number of bytes in a lossless frame
	int     nchsets = 0;                ///< Number of channels sets per frame
	int     nframesegs = 0;             ///< Number of segments per frame
	int     nsegsamples_log2 = 0;       ///< log2(nsegsamples)
	int     nsegsamples = 0;            ///< Samples in segment per one frequency band
	int     nframesamples_log2 = 0;     ///< log2(nframesamples)
	int     nframesamples = 0;          ///< Samples in frame per one frequency band
	int     seg_size_nbits = 0;         ///< Number of bits used to read segment size
	int     band_crc_present = 0;       ///< Presence of CRC16 within each frequency band
	bool    scalable_lsbs = false;          ///< MSB/LSB split flag
	int     ch_mask_nbits = 0;          ///< Number of bits used to read channel mask
	int     fixed_lsb_width = 0;        ///< Fixed LSB width

	DynamicArray<xll_chset> chset;     ///< Channel sets

	DynamicArray<int> navi;          ///< NAVI table

	int     nfreqbands = 0;     ///< Highest number of frequency bands
	int     nchannels = 0;      ///< Total number of channels in a hierarchy
	int     nactivechsets = 0;  ///< Number of active channel sets to decode

	int     nfailedsegs = 0;    ///< Number of frequency band segments that failed to decode

	int     hd_stream_id = 0;   ///< Previous DTS-HD stream ID for detecting changes

	DynamicArray<uint8_t> pbr_buffer;    ///< Peak bit rate (PBR) smoothing buffer
	int         pbr_length = 0;     ///< Length in bytes of data currently buffered
	int         pbr_delay = 0;      ///< Delay in frames before decoding buffered data
};

void xll_clear_band_data( struct xll_chset *chs, int band ) __attribute__( ( cold ) );
void xll_filter_band_data( struct xll_chset *chs, int band );
int xll_get_lsb_width( struct xll_chset *chs, int band, int ch );
void xll_assemble_msbs_lsbs( struct xll_chset *chs, int band );
int xll_assemble_freq_bands( struct xll_decoder *xll );
int xll_map_ch_to_spkr( struct xll_chset *chs, int ch );
int xll_parse( struct xll_decoder *xll, uint8_t *data, struct exss_asset *asset );
void xll_clear( struct xll_decoder *xll ) __attribute__( ( cold ) );