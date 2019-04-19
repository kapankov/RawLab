#ifndef RAWLAB_JPEG_H
#define RAWLAB_JPEG_H

struct RgbBuff
{
	unsigned char* m_buff;
	int m_width;
	int m_height;
	RgbBuff() : m_buff(nullptr), m_width(0), m_height(0) {}
	~RgbBuff() { delete[] m_buff; }
};

using RgbBuffPtr = std::unique_ptr<RgbBuff>;

RgbBuffPtr GetBufFromJpeg(void* pBuff, unsigned long ulSize, bool mirror=false/*, BOOL fAdobeToSrgb = FALSE*/);

#endif
