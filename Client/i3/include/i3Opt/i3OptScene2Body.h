#if !defined( __I3_OPT_SCENE_2_BODY_H)
#define __I3_OPT_SCENE_2_BODY_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptScene2Body : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptScene2Body, i3SceneOptimizer);
protected:
	i3Node *	m_pLastParent = nullptr;
	INT32		m_BoneIndex = 0;

protected:
	virtual void	_Skin2Body( i3Skin * pSkin, i3Body * pBody, i3LOD * pLOD);
	virtual void	_Scene2Body( i3Node * pNode, i3Body * pBody, i3LOD * pLOD);
	void			_Rec_BuildBone( i3Node * pNode, INT32 depth, i3Skeleton * pSkel, i3LOD * pLOD);
	i3Node *		_Rec_RemoveTransform( i3Node * pNode, i3LOD * pLOD);
	i3Node *		_FindParentBone( i3Node * pNode, i3Node ** pPrevParent);

public:
	i3OptScene2Body(void);
	virtual void	Trace( i3Node * pRoot) override;
};

#endif
