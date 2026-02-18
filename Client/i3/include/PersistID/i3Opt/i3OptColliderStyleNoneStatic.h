#if !defined( __I3_OPT_COLLIDER_STYLE_NONESTATIC_H)
#define __I3_OPT_COLLIDER_STYLE_NONESTATIC_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptColliderStyleNoneStatic : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptColliderStyleNoneStatic);

protected:
	void	OnColliderSet( MATRIX * pMat, i3ColliderSet * pColl);
public:
	i3OptColliderStyleNoneStatic(void);
	virtual ~i3OptColliderStyleNoneStatic(void);

	virtual BOOL OnPreNode( i3Node * pNode);
	virtual BOOL OnNode( i3Node * pNode);
	virtual void Trace( i3Node * pRoot);
};

#endif // __I3_OPT_COLLIDER_STYLE_NONESTATIC_H
