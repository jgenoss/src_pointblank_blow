#if !defined( __I3TRANSFORMREF_H__)
#define __I3TRANSFORMREF_H__

#include "i3Node.h"
#include "i3Transform.h"

class I3_EXPORT_SCENE i3TransformRef : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3TransformRef, i3Node);

private:
	i3Transform *		m_pRefTrans = nullptr;
	
public:
	i3TransformRef();
	virtual ~i3TransformRef();
	void			SetTransform( i3Transform * pTrans);
	i3Transform*	GetTransform( void) {	return m_pRefTrans; }
};

#endif
