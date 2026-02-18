#if !defined( I3OPT_VERTEX_FORMAT_H)
#define I3OPT_VERTEX_FORMAT_H

#include "i3SceneOptimizer.h"
#include "i3OptGeometryBase.h"

class I3_EXPORT_OPT i3OptVertexFormat : public i3OptGeometryBase
{
	I3_EXPORT_CLASS_DEFINE( i3OptVertexFormat, i3OptGeometryBase);

protected:
	i3VertexArray *	_ClampTexCoordSet( i3VertexArray * pSrc, INT32 maxSet);

public:
	i3OptVertexFormat(void);

public:
	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Trace( i3Node * pRoot) override;
};

#endif