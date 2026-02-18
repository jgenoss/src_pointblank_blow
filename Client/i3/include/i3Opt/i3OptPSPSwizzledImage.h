#if !defined( __I3_OPT_PSP_SWIZZLED_IMAGE_H)
#define __I3_OPT_PSP_SWIZZLED_IMAGE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptPSPSwizzledImage : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptPSPSwizzledImage, i3SceneOptimizer);
protected:

public:
	i3OptPSPSwizzledImage(void);

	i3Texture *		MakeSwizzledImage( i3Texture * pTex);

	virtual void	Trace( i3Node * pRoot) override;
};

#endif