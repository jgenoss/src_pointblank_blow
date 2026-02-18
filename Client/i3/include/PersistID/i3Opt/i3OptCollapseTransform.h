#if !defined( __I3_OPT_COLLAPSE_TRANSFORM_H)
#define __I3_OPT_COLLAPSE_TRANSFORM_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptCollapseTransform : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptCollapseTransform);
protected:
	i3List		m_TransList;

protected:

	void	OnGeometry( MATRIX * pMat, i3Geometry * pGeo);
	void	OnTransform( MATRIX * pMat, i3Transform * pTrans);
	void	OnColliderSet( MATRIX * pMat, i3ColliderSet * pColl);

	bool	_IsNeedlessTransform( i3Transform * pTrans);

public:
	i3OptCollapseTransform(void);
	virtual ~i3OptCollapseTransform(void);

	void	CollapseTransform( MATRIX * pMat, i3Node * pNode);

	virtual BOOL OnNode( i3Node * pNode);
	virtual void	Trace( i3Node * pRoot);
};

#endif
