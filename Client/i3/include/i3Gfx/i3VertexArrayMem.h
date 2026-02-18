#if !defined( __I3_VERTEX_ARRAY_MEM_H)
#define __I3_VERTEX_ARRAY_MEM_H

#include "i3VertexArray.h"

class I3_EXPORT_GFX i3VertexArrayMem : public i3VertexArray
{
	I3_EXPORT_GHOST_CLASS_DEFINE(i3VertexArrayMem, i3VertexArray);
protected:

	virtual UINT32		CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count);

public:
	i3VertexArrayMem(void) {}
	virtual ~i3VertexArrayMem(void);

	virtual bool	Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE UsageFlags);
	virtual bool	Lock( I3G_LOCKMETHOD lock = I3G_LOCK_OVERWRITE, UINT32 StartIdx = 0, UINT32 Count = 0);
	virtual void	Unlock(void);

	virtual bool	SafeLock( I3G_LOCKMETHOD lock = I3G_LOCK_NONE, UINT32 StartIdx = 0, UINT32 Count = 0);
	virtual bool	SafeUnlock(void);

	virtual bool	Destroy(bool bLostDevice) { return true; }
	virtual bool	isDestroyed(void) { return true; }
	virtual bool	Revive(i3RenderContext * pCtx) { return true; }
};

#endif
