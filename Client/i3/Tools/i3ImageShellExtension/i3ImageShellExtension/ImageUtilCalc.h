#include "i3ImageDefine.h"

#pragma once

INT32 CalcRasterBufferSize(INT32 width, INT32 height, INT32 alignbits, I3G_IMAGE_FORMAT format);
INT32 GetImageFormatBitCount(I3G_IMAGE_FORMAT format);