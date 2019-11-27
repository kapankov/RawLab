// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

//Decompress the JPEG Image
RgbBuffPtr GetBufFromJpeg(void* pBuff, unsigned long ulSize, bool mirror/*, BOOL fAdobeToSrgb*/)
{
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, static_cast<unsigned char*>(pBuff), ulSize);

	jpeg_read_header(&cinfo, TRUE);
	jpeg_calc_output_dimensions(&cinfo);
	jpeg_start_decompress(&cinfo);
	JDIMENSION ulDwAlgnRowSize = cinfo.output_width * 3;
	if (ulDwAlgnRowSize & 3) ulDwAlgnRowSize += SIZEOFDWORD - (ulDwAlgnRowSize & 3);

	auto ptr = std::make_unique<RgbBuff>();
	ptr->m_buff = new unsigned char[static_cast<size_t>(ulDwAlgnRowSize) * static_cast<size_t>(cinfo.output_height)];

	int row_stride = cinfo.output_width * cinfo.output_components;

	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)(reinterpret_cast<j_common_ptr>(&cinfo), JPOOL_IMAGE, row_stride, 1);

/*	cmsHPROFILE hInProfile, hOutProfile;
	cmsHTRANSFORM hTransform;
	if (fAdobeToSrgb)
	{
		hInProfile = cmsOpenProfileFromMem(adobergb_profile, sizeof(adobergb_profile));
		if (hInProfile)
		{
			hOutProfile = cmsCreate_sRGBProfile();
			if (hOutProfile)
				hTransform = cmsCreateTransform(hInProfile, TYPE_BGR_8, hOutProfile, TYPE_BGR_8, INTENT_PERCEPTUAL, 0);
		}
	}*/
	if (cinfo.out_color_components == 3)
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			JDIMENSION rslr = jpeg_read_scanlines(&cinfo, buffer, 1);
			unsigned char* lpbDst = &ptr->m_buff[
				static_cast<size_t>(mirror? cinfo.output_height-cinfo.output_scanline : cinfo.output_scanline - 1)*
				static_cast<size_t>(ulDwAlgnRowSize)];
			unsigned char* lpbSrc = buffer[0];
			for (size_t count = 0; count < static_cast<size_t>(cinfo.output_width); count++)
			{
				*(lpbDst + count * 3 + 0) = *(lpbSrc + count * 3 + 0);
				*(lpbDst + count * 3 + 1) = *(lpbSrc + count * 3 + 1);
				*(lpbDst + count * 3 + 2) = *(lpbSrc + count * 3 + 2);
			}
/*			if (fAdobeToSrgb && hTransform)
				cmsDoTransform(hTransform, lpbDst, lpbDst, cinfo.output_width);*/
		}
	}
	else if (cinfo.out_color_components == 1)
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			JDIMENSION rslr = jpeg_read_scanlines(&cinfo, buffer, 1);
			unsigned char* lpbDst = &ptr->m_buff[
				static_cast<size_t>(mirror ? cinfo.output_height - cinfo.output_scanline : cinfo.output_scanline - 1)*
				static_cast<size_t>(ulDwAlgnRowSize)];
			unsigned char* lpbSrc = buffer[0];
			for (size_t count = 0; count < static_cast<size_t>(cinfo.output_width); count++)
			{
				*(lpbDst + count * 3 + 0) =
					*(lpbDst + count * 3 + 1) =
					*(lpbDst + count * 3 + 2) = *(lpbSrc + count);
			}
		}
	}

/*	if (fAdobeToSrgb)
	{
		if (hTransform) cmsDeleteTransform(hTransform);
		if (hOutProfile) cmsCloseProfile(hOutProfile);
		if (hInProfile) cmsCloseProfile(hInProfile);
	}*/

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	ptr->m_width = cinfo.output_width;
	ptr->m_height = cinfo.output_height;

	return ptr;
}
