#include "i3ImageDefine.h"
#include "ImageUtil16bits.h"
#include "ImageUtil24bits.h"
#include "ImageUtil32bits.h"
#include "ImageUtilDXT.h"
#include "ImageUtilDDS.h"

#pragma once

void i3Image_Convert(I3G_IMAGE_FORMAT srcFormat, char * pSrc, INT32 Width, INT32 Height, INT32 srcAlign,
	I3G_IMAGE_FORMAT destFormat, char * pDest, INT32 destAlign);


D3DFORMAT			i3DXUT_GetNativeImageFormat(I3G_IMAGE_FORMAT format, bool *bExact = 0);
I3G_IMAGE_FORMAT	i3DXUT_GetI3ImageFormat(D3DFORMAT format, bool *bExact);