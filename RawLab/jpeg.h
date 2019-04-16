#ifndef RAWLAB_JPEG_H
#define RAWLAB_JPEG_H

struct RgbBuff
{
	unsigned char* m_buff;
	unsigned int m_uiWidth;
	unsigned int m_uiHeight;
	RgbBuff() : m_buff(nullptr), m_uiWidth(0), m_uiHeight(0) {}
	~RgbBuff() { delete[] m_buff; }
};

using RgbBuffPtr = std::unique_ptr<RgbBuff>;

RgbBuffPtr GetBufFromJpeg(void* pBuff, unsigned long ulSize, bool mirror=false/*, BOOL fAdobeToSrgb = FALSE*/);

#endif
