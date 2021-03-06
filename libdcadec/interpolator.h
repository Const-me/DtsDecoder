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

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#define MAX_LFE_HISTORY     12

struct interpolator;

typedef void( *interpolate_lfe_cb )( int *pcm_samples, int *lfe_samples,
	int npcmblocks, int dec_select );

typedef void( *interpolate_sub_cb )( struct interpolator *dsp, int *pcm_samples,
	int **subband_samples_lo,
	int **subband_samples_hi,
	int nsamples, bool perfect );

struct interpolator
{
	struct idct_context *idct = nullptr;
	void *history = nullptr;
	interpolate_sub_cb interpolate = nullptr;

	operator bool() const
	{
		return nullptr != interpolate;
	}
	void create( struct idct_context *parent, int flags );
	void destroy();
	void clear();
	~interpolator() { destroy(); }

private:
	size_t m_totalBytes = 0;
};

/* struct interpolator *interpolator_create(struct idct_context *parent, int flags)
	__attribute__((cold));
void interpolator_clear(struct interpolator *dsp) __attribute__((cold)); */

#define INTERPOLATE_LFE(x) \
    void interpolate_##x(int *pcm_samples, int *lfe_samples, \
                         int npcmblocks, int dec_select)

#define INTERPOLATE_SUB(x) \
    void interpolate_##x(struct interpolator *dsp, int *pcm_samples, \
                         int **subband_samples_lo, \
                         int **subband_samples_hi, \
                         int nsamples, bool perfect)

INTERPOLATE_LFE( lfe_float_fir );
INTERPOLATE_LFE( lfe_float_fir_2x );
INTERPOLATE_LFE( lfe_float_iir );
INTERPOLATE_SUB( sub32_float );
INTERPOLATE_SUB( sub64_float );

INTERPOLATE_LFE( lfe_fixed_fir );
INTERPOLATE_SUB( sub32_fixed );
INTERPOLATE_SUB( sub64_fixed );

#endif
