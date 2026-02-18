#if !defined( __I3_OPT_COLLAPSE_TRANSFORM_H)
#define __I3_OPT_COLLAPSE_TRANSFORM_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptCollapseTransform : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptCollapseTransform, i3SceneOptimizer);
protected:
	i3::vector<i3Transform*> m_TransList;
	i3::vector<i3Transform2*> m_ShapeTransList;

protected:

	void	OnGeometry( MATRIX * pMat, i3Geometry * pGeo);
	void	OnTransform( MATRIX * pMat, i3Transform * pTrans);
	void	OnColliderSet( MATRIX * pMat, i3ColliderSet * pColl);
	void	OnShapeSetContainer( MATRIX * pMat, i3ShapeSetContainer * pShapeContainer);

	bool	_IsNeedlessTransform( i3Transform * pTrans);

public:
	i3OptCollapseTransform(void);

	void	CollapseTransform( MATRIX * pMat, i3Node * pNode);

	virtual bool OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Trace( i3Node * pRoot) override;
};

#endif
