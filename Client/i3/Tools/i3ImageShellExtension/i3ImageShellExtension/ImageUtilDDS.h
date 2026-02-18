#include <dds.h>
#include "i3ImageDefine.h"

#pragma once

I3G_IMAGE_FORMAT FindImageFormat(UINT32 rmask, UINT32 gmask, UINT32 bmask, UINT32 amask);

HRESULT LoadCubeMapDDS(IStream * pStream, BYTE * pDest, uint32_t width, uint32_t height, UINT32 imageSize, I3G_IMAGE_FORMAT format);

HRESULT LoadCompressedDDS(IStream * pStream, BYTE * pDest, uint32_t width, uint32_t height, I3G_IMAGE_FORMAT format);

HRESULT LoadUnCompressedDDS(IStream * pStream, BYTE * pDest, uint32_t width, uint32_t height, I3G_IMAGE_FORMAT format);

HRESULT LoadRasterData(IStream * pStream, char * pSrc, BYTE * pDest, UINT32 width, UINT32 height, I3G_IMAGE_FORMAT srcFormat, I3G_IMAGE_FORMAT destFormat);