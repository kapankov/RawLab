#ifndef RAWLAB_BITMAP_H
#define RAWLAB_BITMAP_H

// выполняет перекодирование битмапа в буфер с построчным выравниванием (4 байта)
RgbBuffPtr GetAlignedBufFromBitmap(const RgbBuff* bmp, bool mirror = false);

#endif
