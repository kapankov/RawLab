#ifndef RAWLAB_JPEG_H
#define RAWLAB_JPEG_H

using RgbBuffPtr = std::unique_ptr<RgbBuff>;

RgbBuffPtr GetBufFromJpeg(void* pBuff, unsigned long ulSize, bool mirror=false/*, BOOL fAdobeToSrgb = FALSE*/);

#endif
