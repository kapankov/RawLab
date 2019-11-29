// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

constexpr DWORD adobergb_profile[285] = {
	0x74040000, 0x736D636C, 0x00003002, 0x72746E6D, 0x20424752, 0x205A5958, 0x0100D807, 0x0F000A00,
	0x1F000300, 0x70736361, 0x5446534D, 0x00000000, 0x736D636C, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0xD6F60000, 0x00000100, 0x2DD30000, 0x736D636C, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x0D000000, 0x646E6D64, 0x20010000, 0x65000000, 0x63736564, 0x88010000, 0x62000000, 0x64646D64,
	0xEC010000, 0x62000000, 0x74707477, 0x50020000, 0x14000000, 0x5A595872, 0x64020000, 0x14000000,
	0x5A595862, 0x78020000, 0x14000000, 0x5A595867, 0x8C020000, 0x14000000, 0x43525472, 0xA0020000,
	0x0E000000, 0x43525467, 0xB0020000, 0x0E000000, 0x43525462, 0xC0020000, 0x0E000000, 0x6D726863,
	0xD0020000, 0x24000000, 0x74727063, 0xF4020000, 0x69010000, 0x74706B62, 0x60040000, 0x14000000,
	0x63736564, 0x00000000, 0x0B000000, 0x7474696C, 0x6320656C, 0x0000736D, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x63736564, 0x00000000, 0x08000000, 0x626F6441, 0x00383965, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x63736564, 0x00000000, 0x08000000, 0x626F6441, 0x00383965,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x205A5958, 0x00000000, 0x3EF30000, 0x00000100,
	0xA3160100, 0x205A5958, 0x00000000, 0x109C0000, 0xA04F0000, 0xFC040000, 0x205A5958, 0x00000000,
	0x30260000, 0x2E100000, 0x9ABE0000, 0x205A5958, 0x00000000, 0x96340000, 0x32A00000, 0x970F0000,
	0x76727563, 0x00000000, 0x01000000, 0x00003302, 0x76727563, 0x00000000, 0x01000000, 0x00003302,
	0x76727563, 0x00000000, 0x01000000, 0x00003302, 0x6D726863, 0x00000000, 0x00000300, 0xD7A30000,
	0x7B540000, 0xC3350000, 0xC3B50000, 0x66260000, 0x5C0F0000, 0x74786574, 0x00000000, 0x79706F43,
	0x68676972, 0x63282074, 0x30322029, 0x202C3830, 0x646E6153, 0x634D2079, 0x66667547, 0x202E676F,
	0x656D6F53, 0x67697220, 0x20737468, 0x65736572, 0x64657672, 0x68540A2E, 0x77207369, 0x206B726F,
	0x6C207369, 0x6E656369, 0x20646573, 0x65646E75, 0x68742072, 0x72432065, 0x69746165, 0x43206576,
	0x6F6D6D6F, 0x4120736E, 0x69727474, 0x69747562, 0x532D6E6F, 0x65726168, 0x696C4120, 0x3320656B,
	0x5520302E, 0x6574696E, 0x74532064, 0x73657461, 0x63694C20, 0x65736E65, 0x6F54202E, 0x65697620,
	0x20612077, 0x79706F63, 0x20666F20, 0x73696874, 0x63696C20, 0x65736E65, 0x6976202C, 0x20746973,
	0x70747468, 0x632F2F3A, 0x74616572, 0x63657669, 0x6F6D6D6F, 0x6F2E736E, 0x6C2F6772, 0x6E656369,
	0x2F736573, 0x732D7962, 0x2E332F61, 0x73752F30, 0x726F202F, 0x6E657320, 0x20612064, 0x7474656C,
	0x74207265, 0x7243206F, 0x69746165, 0x43206576, 0x6F6D6D6F, 0x202C736E, 0x20313731, 0x6F636553,
	0x5320646E, 0x65657274, 0x53202C74, 0x65746975, 0x30303320, 0x6153202C, 0x7246206E, 0x69636E61,
	0x2C6F6373, 0x6C614320, 0x726F6669, 0x2C61696E, 0x31343920, 0x202C3530, 0x2E415355, 0x00000000,
	0x205A5958, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

//Decompress the JPEG Image
RgbBuffPtr GetBufFromJpeg(void* pBuff, unsigned long ulSize, bool mirror, bool fAdobeToSrgb)
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

	// CMS convert from AdobeRGB to sRGB
	cmsHPROFILE hInProfile, hOutProfile;
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
	}

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
			if (fAdobeToSrgb && hTransform)
				cmsDoTransform(hTransform, lpbDst, lpbDst, cinfo.output_width);
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

	if (fAdobeToSrgb)
	{
		if (hTransform) cmsDeleteTransform(hTransform);
		if (hOutProfile) cmsCloseProfile(hOutProfile);
		if (hInProfile) cmsCloseProfile(hInProfile);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	ptr->m_width = cinfo.output_width;
	ptr->m_height = cinfo.output_height;

	return ptr;
}
