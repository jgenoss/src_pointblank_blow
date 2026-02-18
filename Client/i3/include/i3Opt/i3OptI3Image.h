#if !defined( __I3_OPT_PSP_I3I_IMAGE_H)
#define __I3_OPT_PSP_I3I_IMAGE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptI3Image : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptI3Image, i3SceneOptimizer);

protected:
	void		ReplaceTextureBind( i3::vector<i3RenderAttr*>& BindList, i3Texture * pNewTex, i3Texture * pSrcTex);

public:
	i3OptI3Image(void);

	UINT32	GetColorIndexCount(i3Texture* pSrcTex);
	i3Texture* MakeI3I(i3Texture* pSrcTex);
	i3Texture* MakeI3IMipmap(i3Texture** pSrcTex, UINT32 MipmapLevelCount);

	virtual void	Trace( i3Node * pRoot) override;
};

#endif