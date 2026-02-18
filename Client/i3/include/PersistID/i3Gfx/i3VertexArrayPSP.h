#if !defined( __I3_VERTEX_ARRAY_PSP_H)
#define __I3_VERTEX_ARRAY_PSP_H

#if !defined(I3G_PSP)
#error "This header file is only for SONY PSP."
#endif

#include "i3VertexArray.h"

class i3VertexArrayPSP : public i3VertexArray
{
	I3_CLASS_DEFINE( i3VertexArrayPSP);

protected:

	virtual UINT32		CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count);

public:
	i3VertexArrayPSP(void);
	virtual ~i3VertexArrayPSP(void);

	virtual BOOL	Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE UsageFlags);
	virtual BOOL	Lock( I3G_LOCKMETHOD lock = I3G_LOCK_OVERWRITE, UINT32 StartIdx = 0, UINT32 EndIdx = 0);
	virtual void	Unlock(void);
	virtual void	MakeRuntimeFormat( I3_PERSIST_VERTEX_OFFSET * pInfo);
};

#endif
