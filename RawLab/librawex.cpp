// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

LibRawEx::LibRawEx()
{
	m_defaultLibrawOutputParamsPtr = std::make_unique<libraw_output_params_t>(imgdata.params);

//#pragma premsg(проинициализировать exparams) 
	// init exparams
}

// from void CLASS scale_colors()
std::array<float, 4> LibRawEx::getAutoWB()
{
	std::array<float, 4> autowb = { imgdata.color.pre_mul[0], imgdata.color.pre_mul[1], imgdata.color.pre_mul[2], imgdata.color.pre_mul[3] };
//	if (imgdata.image) тут проверить состояние
	const unsigned int (&greybox)[4] = imgdata.params.greybox;
	const ushort &height = imgdata.sizes.height;
	const ushort &width = imgdata.sizes.width;
	const ushort &iwidth = imgdata.sizes.iwidth;
	const unsigned int &filters = imgdata.idata.filters;
	const unsigned int &maximum = imgdata.color.maximum;
	const ushort &shrink = libraw_internal_data.internal_output_params.shrink;
	const unsigned int (&cblack)[LIBRAW_CBLACK_SIZE] = imgdata.color.cblack;
	ushort (* const (&image))[4] = imgdata.image;
	unsigned int c;
	int val;
	unsigned int sum[8];
	double dsum[8], dmin, dmax;
	{
		memset(dsum, 0, sizeof dsum);
		unsigned int bottom = MIN(greybox[1] + greybox[3], height);
		unsigned int right = MIN(greybox[0] + greybox[2], width);
		for (unsigned int row = greybox[1]; row < bottom; row += 8)
			for (unsigned int col = greybox[0]; col < right; col += 8)
			{
				memset(sum, 0, sizeof sum);
				for (unsigned int y = row; y < row + 8 && y < bottom; y++)
					for (unsigned int x = col; x < col + 8 && x < right; x++)
						for (unsigned int c = 0; c < 4; c++)
						{
							if (filters)
							{
								c = fcol(y, x);
								val = imgdata.image[((y) >> shrink) * iwidth + ((x) >> shrink)][fcol(y, x)];
							}
							else
								val = imgdata.image[y * width + x][c];
							if (val > maximum - 25)
								goto skip_block;
							if ((val -= cblack[c]) < 0)
								val = 0;
							sum[c] += val;
							sum[c + 4]++;
							if (filters)
								break;
						}
				for (unsigned int c = 0; c < 8; c++) dsum[c] += sum[c];
				skip_block:;
			}
		for (unsigned int c = 0; c < 4; c++) if (dsum[c]) autowb[c] = dsum[c + 4] / dsum[c];
		for (dmin = DBL_MAX, dmax = c = 0; c < 4; c++)
		{
			if (dmin > autowb[c])
				dmin = autowb[c];
			if (dmax < autowb[c])
				dmax = autowb[c];
		}
		if (!imgdata.params.highlight) dmax = dmin;
		for (c = 0; c < 4; c++) autowb[c] /= float(dmax);

	}
	return autowb;
}

#include "libraw/libraw_types.h"
#define LIBRAW_LIBRARY_BUILD
#define LIBRAW_IO_REDEFINED
#include "libraw/libraw.h"
#include "internal/defines.h"
#define SRC_USES_SHRINK
#define SRC_USES_BLACK
#define SRC_USES_CURVE

const double LibRaw_constants::xyz_rgb[3][3] = {
    {0.4124564, 0.3575761, 0.1804375}, {0.2126729, 0.7151522, 0.0721750}, {0.0193339, 0.1191920, 0.9503041}};

const float LibRaw_constants::d65_white[3] = {0.95047f, 1.0f, 1.08883f};

//rgb_constants
#define xyz_rgb         (LibRaw_constants::xyz_rgb)
#define d65_white       (LibRaw_constants::d65_white)

/* WF filtering is allowed to triple libraw license */
//#define wf_deband_treshold	(exparams.wf_deband_treshold)
//#include "internal/libraw_internal_funcs.h"
//#include "internal/wf_filtering.cpp"
/* DHT and AAHD are LGPL licensed, so include them */
//#pragma warning( disable : 4244 )
//#pragma warning( disable : 4305 )
//#include "internal/dht_demosaic.cpp"
//#include "internal/aahd_demosaic.cpp"

#include "internal/var_defines.h" 

#define CLASS LibRawEx::

// VCD
#define eeci_refine    (exparams.eeci_refine)
#define es_med_passes  (exparams.es_med_passes)

/*AHD-Mod*/
#include <ahd_interpolate_mod.c>
/*AFD*/
#include <ahd_partial_interpolate.c>
#include <afd_interpolate_pl.c>
/* VCD*/
#include <refinement.c>
#include <vcd_interpolate.c>
#include <es_median_filter.c>
#include <median_filter_new.c>
/*LMMSE*/
#include <lmmse_interpolate.c>
/*AMaZE*/
#include <amaze_demosaic_RT.cc>
#include <CA_correct_RT.cc>
#include <cfa_linedn_new.c>
#include <cfa_impulse_gauss.c>
//#pragma warning( default : 4305 )
//#pragma warning( default : 4244 )

#undef CLASS


void demosaic_pack_pre_interpolate_cb(void *ctx)
{
	if (ctx)
	{
		try
		{
			((LibRawEx*)ctx)->pre_interpolate();
		}
		catch(...)
		{
			return;
		}
	}
}

void demosaic_pack_interpolate_bayer_cb(void *ctx)
{
	if (ctx)
	{
		try
		{
			((LibRawEx*)ctx)->interpolate();
		}
		catch(...)
		{
			return;
		}
	}
}

void demosaic_pack_post_interpolate_cb(void *ctx)
{
	if (ctx)
	{
		try
		{
			((LibRawEx*)ctx)->post_interpolate();
		}
		catch(...)
		{
			return;
		}
	}	
}

void LibRawEx::set_pre_interpolate_cb(process_step_callback cb, bool def_cb)
{
	callbacks.pre_interpolate_cb = def_cb ? demosaic_pack_pre_interpolate_cb : callbacks.pre_interpolate_cb = cb;
}

void LibRawEx::set_interpolate_bayer_cb(process_step_callback cb, bool def_cb)
{
	callbacks.interpolate_bayer_cb = def_cb ? demosaic_pack_interpolate_bayer_cb : callbacks.interpolate_bayer_cb = cb;
}

void LibRawEx::set_post_interpolate_cb(process_step_callback cb, bool def_cb)
{
	callbacks.post_interpolate_cb = def_cb ? demosaic_pack_post_interpolate_cb : callbacks.post_interpolate_cb = cb;
}

void LibRawEx::pre_interpolate()
{
	float cared=0,cablue=0;
	float linenoise=0;
	float lclean=0,cclean=0;
	if (exparams.ca_correc >0 ) {cablue=exparams.cablue; cared=exparams.cared; CA_correct_RT(cablue, cared);}
	if (exparams.cfaline >0 ) {linenoise=exparams.linenoise; cfa_linedn(linenoise);}
	if (exparams.cfa_clean >0 ) {lclean=exparams.lclean; cclean=exparams.cclean; cfa_impulse_gauss(lclean,cclean);}
}

void LibRawEx::interpolate()
{
	// LIBRAW_DEMOSAIC_PACK_GPL3
	if (imgdata.params.user_qual == 5)
		ahd_interpolate_mod();
	else if (imgdata.params.user_qual == 6)
		afd_interpolate_pl(2, 1);
	else if (imgdata.params.user_qual == 7)
		vcd_interpolate(0);
	else if (imgdata.params.user_qual == 8)
		vcd_interpolate(12);
	else if (imgdata.params.user_qual == 9)
		lmmse_interpolate(1);

	// LIBRAW_DEMOSAIC_PACK_GPL3
	else if (imgdata.params.user_qual == 10)
		amaze_demosaic_RT();
}

void LibRawEx::post_interpolate()
{
	int eeci_refine_fl=0, es_med_passes_fl=0;
    if (eeci_refine >=0 ) eeci_refine_fl = eeci_refine;
    if (es_med_passes >0 ) es_med_passes_fl = es_med_passes;

	if (imgdata.params.user_qual == 8)
	{
		if (eeci_refine_fl == 1) refinement();
		if (med_passes > 0)    median_filter_new();
		if (es_med_passes_fl > 0) es_median_filter();
	}
}