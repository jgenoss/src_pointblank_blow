#if !defined( __I3_OPT_SKIN_TO_SKIN2_H)
#define __I3_OPT_SKIN_TO_SKIN2_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptSkinToSkin2 : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptSkinToSkin2);
protected:
	i3Node *	m_pLastParent;
protected:
	void	_MoveToSkin2( i3Skin * pSkin, i3Skin2 * pSkin2, i3SkinLOD * pSkinLOD);

public:
	i3OptSkinToSkin2(void);
	virtual ~i3OptSkinToSkin2(void);

	virtual void	Trace( i3Node * pRoot);
};

#endif
