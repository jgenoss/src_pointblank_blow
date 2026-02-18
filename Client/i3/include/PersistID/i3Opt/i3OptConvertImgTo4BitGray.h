#ifndef I3_OPT_CONVERTIMG_TO_4BITGRAY_H__
#define I3_OPT_CONVERTIMG_TO_4BITGRAY_H__
#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptConvertImgTo4BitGray :	public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptConvertImgTo4BitGray );
protected:


public:
	i3OptConvertImgTo4BitGray(void);
	virtual ~i3OptConvertImgTo4BitGray(void);

public:
	i3Texture * ConvertImgTo4BitGray( i3Texture * pSrcTex, BOOL bSwizzled = TRUE);
	i3Texture * ConvertImgTo8Gray( i3Texture * pSrcTex );
	i3Texture * Convert8to4( i3Texture * pSrcTex );

	BOOL		ConvertClut8To4( i3Texture * pDestTex, i3Texture * pSrcTex );
	BOOL		ConvertPixel8To4( i3Texture * pDestTex, i3Texture * pSrcTex );
	UINT8		GetIndexAtClut4( i3Clut * pSrcClut, i3Clut * pDestClut, UINT8	nIndexAt8);//주어진 4bit Clut인덱스중에서 8bit Clut인덱스가 위치하는곳을 리턴
};

#endif
