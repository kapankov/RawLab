// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

LibRawEx::LibRawEx()
{
	m_jpegQuality = 90;
	m_defaultLibrawOutputParamsPtr = std::make_unique<libraw_output_params_t>(imgdata.params);
	auto_mul[0] = auto_mul[1] = auto_mul[2] = 1.0f;
	auto_mul[3] = .0f;
	// init exparams
	exparams.eeci_refine = 0;
	exparams.es_med_passes = 0;
	exparams.ca_correc = 0;
	exparams.cared = 0;
	exparams.cablue = 0;
	exparams.cfaline = 0;
	exparams.linenoise = 0;
	exparams.cfa_clean = 0;
	exparams.lclean = 0;
	exparams.cclean = 0;

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
//	ushort (* const (&image))[4] = imgdata.image;
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
						for (/*unsigned int */c = 0; c < 4; c++)
						{
							if (filters)
							{
								c = fcol(y, x);
								val = imgdata.image[((y) >> shrink) * iwidth + ((x) >> shrink)][fcol(y, x)];
							}
							else
								val = imgdata.image[y * width + x][c];
							if (val > (int)(maximum - 25))
								goto skip_block;
							if ((val -= cblack[c]) < 0)
								val = 0;
							sum[c] += val;
							sum[c + 4]++;
							if (filters)
								break;
						}
				for (/*unsigned int */c = 0; c < 8; c++) dsum[c] += sum[c];
				skip_block:;
			}
		for (/*unsigned int */c = 0; c < 4; c++) if (dsum[c]) autowb[c] = dsum[c + 4] / dsum[c];
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

#define FORC(cnt) for (c = 0; c < cnt; c++)
#define FORC4 FORC(4)
#define TOFF(ptr) ((char *)(&(ptr)) - (char *)th)

#define EXIF_MARKER  (JPEG_APP0 + 1)     /* JPEG marker code for EXIF */
#define EXIF_OVERHEAD_LEN  6            /* size of non-profile data in APP2 */

void tiff_set(struct tiff_hdr* th, ushort* ntag, ushort tag,
	ushort type, int count, int val)
{
	struct libraw_tiff_tag* tt;
	int c;

	tt = (struct libraw_tiff_tag*)(ntag + 1) + (*ntag)++;
	tt->val.i = val;
	if (type == 1 && count <= 4)
		FORC(4) tt->val.c[c] = val >> (c << 3);
	else if (type == 2)
	{
		count = strnlen((char*)th + val, count - 1) + 1;
		if (count <= 4)
			FORC(4) tt->val.c[c] = ((char*)th)[val + c];
	}
	else if (type == 3 && count <= 2)
		FORC(2) tt->val.s[c] = val >> (c << 4);
	tt->count = count;
	tt->type = type;
	tt->tag = tag;
}

/* 
	было бы неплохо использовать void LibRaw::tiff_head(struct tiff_hdr *th, int full) 
	пришлось использовать слегка модифицированную копию
*/
void LibRawEx::make_tiff_head(struct tiff_hdr* th, int full)
{
	int c, psize = 0;
	struct tm* t;
	libraw_imgother_t& other = imgdata.other;
	libraw_iparams_t& idata = imgdata.idata;
	libraw_image_sizes_t& sizes = imgdata.sizes;
	libraw_output_params_t& params = imgdata.params;
	unsigned int* oprof = libraw_internal_data.output_data.oprof;

	memset(th, 0, sizeof * th);
	th->t_order = htonl(0x4d4d4949) >> 16;
	th->magic = 42;	// 2A00
	th->ifd = 10;	// 0A000000;
	th->rat[0] = th->rat[2] = 300;
	th->rat[1] = th->rat[3] = 1;
	FORC(6) th->rat[4 + c] = 1000000;

	th->rat[4] *= other.shutter;
	th->rat[6] *= other.aperture;
	th->rat[8] *= other.focal_len;
	strncpy(th->t_desc, other.desc, 512);
	strncpy(th->t_make, idata.make, 64);
	strncpy(th->t_model, idata.model, 64);
	strcpy(th->soft, "rawlab");
	t = localtime(&other.timestamp);
	sprintf(th->date, "%04d:%02d:%02d %02d:%02d:%02d", t->tm_year + 1900,
		t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	strncpy(th->t_artist, other.artist, 64);
	if (full)
	{
		tiff_set(th, &th->ntag, 254, 4, 1, 0);	// SubfileType
		tiff_set(th, &th->ntag, 256, 4, 1, sizes.width);	// ImageWidth
		tiff_set(th, &th->ntag, 257, 4, 1, sizes.height);	// ImageHeight
		tiff_set(th, &th->ntag, 258, 3, idata.colors, params.output_bps);	// BitsPerSample
		if (idata.colors > 2)
			th->tag[th->ntag - 1].val.i = TOFF(th->bps);
		FORC4 th->bps[c] = params.output_bps;
		tiff_set(th, &th->ntag, 259, 3, 1, 1);	// Compression
		tiff_set(th, &th->ntag, 262, 3, 1, 1 + (idata.colors > 1));	// PhotometricInterpretation
	}
	tiff_set(th, &th->ntag, 270, 2, 512, TOFF(th->t_desc));	// ImageDescription
	tiff_set(th, &th->ntag, 271, 2, 64, TOFF(th->t_make));	// Make
	tiff_set(th, &th->ntag, 272, 2, 64, TOFF(th->t_model));	// 	Model
	if (full)
	{
		if (oprof)
			psize = ntohl(oprof[0]);
		tiff_set(th, &th->ntag, 273, 4, 1, sizeof(tiff_hdr) + psize);	// StripOffsets ???
		tiff_set(th, &th->ntag, 277, 3, 1, idata.colors);	// SamplesPerPixel
		tiff_set(th, &th->ntag, 278, 4, 1, sizes.height);	// RowsPerStrip
		tiff_set(th, &th->ntag, 279, 4, 1,
			sizes.height * sizes.width * idata.colors * params.output_bps / 8);	// StripByteCounts
	}
	else
		tiff_set(th, &th->ntag, 274, 3, 1, "12435867"[sizes.flip] - '0');	// Orientation
	tiff_set(th, &th->ntag, 282, 5, 1, TOFF(th->rat[0]));	// 	XResolution
	tiff_set(th, &th->ntag, 283, 5, 1, TOFF(th->rat[2]));	// 	YResolution
	tiff_set(th, &th->ntag, 284, 3, 1, 1);	// PlanarConfiguration = Chunky
	tiff_set(th, &th->ntag, 296, 3, 1, 2);	// ResolutionUnit = inches
	tiff_set(th, &th->ntag, 305, 2, 32, TOFF(th->soft));	// Software
	tiff_set(th, &th->ntag, 306, 2, 20, TOFF(th->date));	// ModifyDate
	tiff_set(th, &th->ntag, 315, 2, 64, TOFF(th->t_artist));	// Artist
	tiff_set(th, &th->ntag, 34665, 4, 1, TOFF(th->nexif));	// ExifOffset
	if (psize)
		tiff_set(th, &th->ntag, 34675, 7, psize, sizeof(tiff_hdr));	// ICC_Profile
	tiff_set(th, &th->nexif, 33434, 5, 1, TOFF(th->rat[4]));	// ExposureTime
	tiff_set(th, &th->nexif, 33437, 5, 1, TOFF(th->rat[6]));	// FNumber
	tiff_set(th, &th->nexif, 34855, 3, 1, other.iso_speed);	// ISO
	tiff_set(th, &th->nexif, 37386, 5, 1, TOFF(th->rat[8]));	// 	FocalLength
	// 	GPSInfo
	if (other.gpsdata[1])
	{
		tiff_set(th, &th->ntag, 34853, 4, 1, TOFF(th->ngps));
		tiff_set(th, &th->ngps, 0, 1, 4, 0x202);
		tiff_set(th, &th->ngps, 1, 2, 2, other.gpsdata[29]);
		tiff_set(th, &th->ngps, 2, 5, 3, TOFF(th->gps[0]));
		tiff_set(th, &th->ngps, 3, 2, 2, other.gpsdata[30]);
		tiff_set(th, &th->ngps, 4, 5, 3, TOFF(th->gps[6]));
		tiff_set(th, &th->ngps, 5, 1, 1, other.gpsdata[31]);
		tiff_set(th, &th->ngps, 6, 5, 1, TOFF(th->gps[18]));
		tiff_set(th, &th->ngps, 7, 5, 3, TOFF(th->gps[12]));
		tiff_set(th, &th->ngps, 18, 2, 12, TOFF(th->gps[20]));
		tiff_set(th, &th->ngps, 29, 2, 12, TOFF(th->gps[23]));
		memcpy(th->gps, other.gpsdata, sizeof th->gps);
	}
}

void jpeg_write_tiff_hdr(j_compress_ptr cinfo, const JOCTET* data_ptr, const unsigned int data_len)
{
	unsigned int length = data_len;
	/* Write the JPEG marker header (APP1 code and marker length) */
	jpeg_write_m_header(cinfo, EXIF_MARKER,
		(unsigned int)(length + EXIF_OVERHEAD_LEN));

	/* Write the marker identifying string "Exif" (null-terminated).  We
	 * code it in this less-than-transparent way so that the code works even if
	 * the local character set is not ASCII.
	 */
	jpeg_write_m_byte(cinfo, 0x45);
	jpeg_write_m_byte(cinfo, 0x78);
	jpeg_write_m_byte(cinfo, 0x69);
	jpeg_write_m_byte(cinfo, 0x66);
	jpeg_write_m_byte(cinfo, 0x0);
	jpeg_write_m_byte(cinfo, 0x0);

	/* Add the EXIF data */
	while (length--) {
		jpeg_write_m_byte(cinfo, *data_ptr);
		data_ptr++;
	}

}

int LibRawEx::rawlab_jpeg_writer(const char* filename)
{
	int ret = 0;
	int width, height, colors, bps;
	size_t row_stride;
	CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

	if (!imgdata.image)
		return LIBRAW_OUT_OF_ORDER_CALL;

	if (!filename)
		return ENOENT;

	get_mem_image_format(&width, &height, &colors, &bps);
	bps = 8;
	row_stride = width * colors;

	unsigned char* imgBuff = new unsigned char[row_stride * static_cast<size_t>(height)];
	std::swap(bps, imgdata.params.output_bps);
	ret = copy_mem_image(imgBuff, static_cast<int>(row_stride), 0);
	std::swap(bps, imgdata.params.output_bps);
	
	if (!ret)
	{
		FILE* f;
		if (fopen_s(&f, filename, "wb") == 0)
		{
			struct jpeg_compress_struct cinfo;
			struct jpeg_error_mgr jerr;
			JSAMPROW row_pointer[1]; /* pointer to JSAMPLE row[s] */

			cinfo.err = jpeg_std_error(&jerr);
			jpeg_create_compress(&cinfo);
			jpeg_stdio_dest(&cinfo, f);
			cinfo.image_width = width;      /* image width and height, in pixels */
			cinfo.image_height = height;
			cinfo.input_components = 3;           /* # of color components per pixel */
			cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */
			jpeg_set_defaults(&cinfo);
			jpeg_set_quality(&cinfo, m_jpegQuality, TRUE);
			cinfo.write_JFIF_header = false;
			jpeg_start_compress(&cinfo, TRUE);
			/* add exif with icc profile */
			unsigned int* oprof = libraw_internal_data.output_data.oprof;
			if (JOCTET* th = new JOCTET[oprof ? sizeof(tiff_hdr) + ntohl(oprof[0]) : sizeof(tiff_hdr)])
			{
				make_tiff_head((tiff_hdr*)th, 1);
				if (oprof)
					memcpy(&th[sizeof(tiff_hdr)], (const JOCTET*)oprof, ntohl(oprof[0]));
				jpeg_write_tiff_hdr(&cinfo, th, oprof ? sizeof(tiff_hdr) + ntohl(oprof[0]) : sizeof(tiff_hdr));

				delete [] th;
			}

			while (cinfo.next_scanline < cinfo.image_height) {
				row_pointer[0] = &imgBuff[cinfo.next_scanline * row_stride];
				(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
			}
			jpeg_finish_compress(&cinfo);
			fclose(f);
			jpeg_destroy_compress(&cinfo);
		}
		else ret = -1;
	}
	delete[] imgBuff;
	return ret;
}

void LibRawEx::setJpegQuality(int iQuality)
{
	m_jpegQuality = iQuality;
}

int LibRawEx::getJpegQuality()
{
	return m_jpegQuality;
}

#include "libraw/libraw_types.h"
#define LIBRAW_LIBRARY_BUILD
#define LIBRAW_IO_REDEFINED
#include "libraw/libraw.h"
#pragma warning(push)
#pragma warning(disable: 4505)
#include "internal/defines.h"
#pragma warning(pop)
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