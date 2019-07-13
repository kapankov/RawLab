// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

RgbBuffPtr GetBufFromBitmap(const BmpBuff * bmp, bool mirror)
{
	if (!bmp) return nullptr;
	size_t height = static_cast<size_t>(bmp->m_height);
	size_t width = static_cast<size_t>(bmp->m_width);
	size_t ulDwAlgnRowSize = width * 3;
	if (ulDwAlgnRowSize & 3) ulDwAlgnRowSize += SIZEOFDWORD - (ulDwAlgnRowSize & 3);

	auto ptr = std::unique_ptr<RgbBuff>(new RgbBuff());
	ptr->m_buff = new unsigned char[ulDwAlgnRowSize * height];

	if (bmp->m_colors == 3)
	{
		size_t szScanline = 0;
		while (szScanline < height)
		{
			unsigned char* lpbDst = &ptr->m_buff[(mirror ? height - szScanline - 1 : szScanline)*ulDwAlgnRowSize];
			unsigned char* lpbSrc = &(bmp->m_buff)[szScanline*(width * bmp->m_colors * bmp->m_bits / 8)];
			for (size_t count = 0; count < width; count++)
			{
				*(lpbDst + count * 3 + 0) = *(lpbSrc + count * 3 + 0);
				*(lpbDst + count * 3 + 1) = *(lpbSrc + count * 3 + 1);
				*(lpbDst + count * 3 + 2) = *(lpbSrc + count * 3 + 2);
			}
			szScanline++;
		}
	}
	else if (bmp->m_colors == 1)
	{
		size_t szScanline = 0;
		while (szScanline < height)
		{
			unsigned char* lpbDst = &ptr->m_buff[(mirror ? height - szScanline - 1 : szScanline)*ulDwAlgnRowSize];
			unsigned char* lpbSrc = &(bmp->m_buff)[szScanline*(width * bmp->m_colors * bmp->m_bits / 8)];
			for (size_t count = 0; count < width; count++)
			{
				*(lpbDst + count * 3 + 0) =
					*(lpbDst + count * 3 + 1) =
					*(lpbDst + count * 3 + 2) = *(lpbSrc + count);
			}
			szScanline++;
		}
	}
	ptr->m_width = bmp->m_width;
	ptr->m_height = bmp->m_height;
	return ptr;
}
