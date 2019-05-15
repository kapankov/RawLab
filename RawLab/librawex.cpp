#include "stdafx.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

LibRawEx::LibRawEx()
{
//#pragma premsg(проинициализировать exparams) 
	// init exparams
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