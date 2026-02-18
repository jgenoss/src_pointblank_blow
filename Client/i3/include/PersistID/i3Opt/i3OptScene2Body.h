#if !defined( __I3_OPT_SCENE_2_BODY_H)
#define __I3_OPT_SCENE_2_BODY_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptScene2Body : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptScene2Body);
protected:
	i3Node *	m_pLastParent;
	INT32		m_BoneIndex;

protected:
	void			_Skin2Body( i3Skin * pSkin, i3Body * pBody, i3LOD * pLOD);
	void			_Scene2Body( i3Node * pNode, i3Body * pBody, i3LOD * pLOD);
	void			_Rec_BuildBone( i3Node * pNode, INT32 depth, i3Skeleton * pSkel, i3LOD * pLOD);
	i3Node *		_Rec_RemoveTransform( i3Node * pNode, i3LOD * pLOD);
	i3Node *		_FindParentBone( i3Node * pNode, i3Node ** pPrevParent);

public:
	i3OptScene2Body(void);
	virtual ~i3OptScene2Body(void);

	virtual void	Trace( i3Node * pRoot);
};

#endif
