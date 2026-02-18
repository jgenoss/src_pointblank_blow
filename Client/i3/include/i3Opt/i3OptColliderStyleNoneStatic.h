#if !defined( __I3_OPT_COLLIDER_STYLE_NONESTATIC_H)
#define __I3_OPT_COLLIDER_STYLE_NONESTATIC_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptColliderStyleNoneStatic : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptColliderStyleNoneStatic, i3SceneOptimizer);

protected:
	void	OnColliderSet( MATRIX * pMat, i3ColliderSet * pColl);

public:
	i3OptColliderStyleNoneStatic(void);

	virtual bool OnPreNode( i3Node * pNode, i3Node * pParent) override;
	virtual bool OnNode(i3Node * pNode, i3Node * pParent) override { return true; }
	virtual void Trace( i3Node * pRoot) override;
};

#endif // __I3_OPT_COLLIDER_STYLE_NONESTATIC_H
