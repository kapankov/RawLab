// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include <cassert>

template<typename T>
RgbBuffPtr GetAlignedBufFromBitmapTmpl(const RgbBuff* bmp, bool mirror)
{
	size_t height = static_cast<size_t>(bmp->m_height);
	size_t width = static_cast<size_t>(bmp->m_width);
	size_t ulDwAlgnRowSize = width * 3 * bmp->m_bits/8;
	if (ulDwAlgnRowSize & 3) ulDwAlgnRowSize += SIZEOFDWORD - (ulDwAlgnRowSize & 3);

	auto ptr = std::make_unique<RgbBuff>();
	ptr->m_buff = new unsigned char[ulDwAlgnRowSize * height];

	if (bmp->m_colors == 3)
	{
		size_t szScanline = 0;
		while (szScanline < height)
		{
			T* lpDst = reinterpret_cast<T*>(&ptr->m_buff[(mirror ? height - szScanline - 1 : szScanline) * ulDwAlgnRowSize]);
			T* lpSrc = reinterpret_cast<T*>(&(bmp->m_buff)[szScanline * (width * bmp->m_colors * bmp->m_bits / 8)]);
			for (size_t count = 0; count < width; count++)
			{
				*(lpDst + count * 3 + 0) = *(lpSrc + count * 3 + 0);
				*(lpDst + count * 3 + 1) = *(lpSrc + count * 3 + 1);
				*(lpDst + count * 3 + 2) = *(lpSrc + count * 3 + 2);
			}
			szScanline++;
		}
	}
	else if (bmp->m_colors == 1)
	{
		size_t szScanline = 0;
		while (szScanline < height)
		{
			T* lpDst = reinterpret_cast<T*>(&ptr->m_buff[(mirror ? height - szScanline - 1 : szScanline) * ulDwAlgnRowSize]);
			T* lpSrc = reinterpret_cast<T*>(&(bmp->m_buff)[szScanline * (width * bmp->m_colors * bmp->m_bits / 8)]);
			for (size_t count = 0; count < width; count++)
			{
				*(lpDst + count * 3 + 0) =
					*(lpDst + count * 3 + 1) =
					*(lpDst + count * 3 + 2) = *(lpSrc + count);
			}
			szScanline++;
		}
	}
	ptr->m_width = bmp->m_width;
	ptr->m_height = bmp->m_height;
	ptr->m_colors = 3;
	ptr->m_bits = sizeof(T) * 8;
	return ptr;
}

RgbBuffPtr GetAlignedBufFromBitmap(const RgbBuff * bmp, bool mirror)
{
	if (!bmp) return nullptr;

	// в теории должно работать и с 16-битными битмапами, но не проверялось
	assert(bmp->m_bits==8);

	return bmp->m_bits / 8 > 1 ? GetAlignedBufFromBitmapTmpl<unsigned short>(bmp, mirror) : GetAlignedBufFromBitmapTmpl<unsigned char>(bmp, mirror);
}
