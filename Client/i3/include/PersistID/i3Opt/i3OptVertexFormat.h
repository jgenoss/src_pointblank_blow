#if !defined( I3OPT_VERTEX_FORMAT_H)
#define I3OPT_VERTEX_FORMAT_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptVertexFormat : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptVertexFormat);

protected:
	i3VertexArray *	_ClampTexCoordSet( i3VertexArray * pSrc, INT32 maxSet);

public:
	i3OptVertexFormat(void);
	virtual ~i3OptVertexFormat(void);

public:
	virtual BOOL	OnNode( i3Node * pNode);
};

#endif