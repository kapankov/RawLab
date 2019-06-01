#ifndef RAWLAB_BITMAP_H
#define RAWLAB_BITMAP_H

// выполняет перекодирование битмапа с построчным выравниванием (4 байта)
RgbBuffPtr GetBufFromBitmap(const BmpBuff* bmp, bool mirror = false);

#endif
