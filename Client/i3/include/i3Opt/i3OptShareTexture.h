#if !defined( __I3_OPT_SHARE_TEXTURE_H)
#define __I3_OPT_SHARE_TEXTURE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptShareTexture : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptShareTexture, i3SceneOptimizer);
protected:

	void	_ReplaceTexBind( i3::vector<i3RenderAttr*>& List, i3Texture * pFindTex, i3Texture * pNewTex);

public:
	i3OptShareTexture(void);

	virtual void	Trace( i3Node * pRoot) override;
};

#endif
