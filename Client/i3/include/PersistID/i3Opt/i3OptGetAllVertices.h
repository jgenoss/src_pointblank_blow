#if !defined( __I3_OPT_GET_ALLVERTICES_H)
#define __I3_OPT_GET_ALLVERTICES_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptGetAllVertices : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptGetAllVertices);
protected:
	INT32					m_Count;
	i3Vector3Array *		m_pVA;

public:
	i3OptGetAllVertices(void);
	virtual ~i3OptGetAllVertices(void);

	i3Vector3Array *	getVectorArray(void)			{ return m_pVA; }

	virtual BOOL	OnNode( i3Node * pNode);
	virtual void	Trace( i3Node * pNode);
};

#endif
