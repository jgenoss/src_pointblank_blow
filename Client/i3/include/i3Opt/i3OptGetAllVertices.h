#if !defined( __I3_OPT_GET_ALLVERTICES_H)
#define __I3_OPT_GET_ALLVERTICES_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptGetAllVertices : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptGetAllVertices, i3SceneOptimizer);
protected:
	INT32					m_Count = 0;
	i3Vector3Array *		m_pVA = nullptr;

public:
	i3OptGetAllVertices(void);
	virtual ~i3OptGetAllVertices(void);

	i3Vector3Array *	getVectorArray(void)			{ return m_pVA; }

	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Trace( i3Node * pNode) override;
};

#endif
