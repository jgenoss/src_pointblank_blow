#if !defined( __I3_OPT_COMPRESS_IMAGE_H)
#define __I3_OPT_COMPRESS_IMAGE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptCompressImage : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptCompressImage, i3SceneOptimizer);
protected:

public:
	i3OptCompressImage(void);

	i3Texture *		CompressImage( i3Texture * pTex);
	i3Texture *		DecompressImage( i3Texture * pTex);

	virtual void	Trace( i3Node * pRoot) override;
};

#endif