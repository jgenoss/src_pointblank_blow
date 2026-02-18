#if !defined( __I3_VERTEX_ARRAY_MEM_H)
#define __I3_VERTEX_ARRAY_MEM_H

#include "i3VertexArray.h"

class I3_EXPORT_GFX i3VertexArrayMem : public i3VertexArray
{
	I3_CLASS_DEFINE( i3VertexArrayMem);

protected:

	virtual UINT32		CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count);

public:
	i3VertexArrayMem(void);
	virtual ~i3VertexArrayMem(void);

	virtual BOOL	Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE UsageFlags);
	virtual BOOL	Lock( I3G_LOCKMETHOD lock = I3G_LOCK_OVERWRITE, UINT32 StartIdx = 0, UINT32 EndIdx = 0);
	virtual void	Unlock(void);
};

#endif
