#if !defined( __I3_VERTEX_ARRAY_DX_H)
#define __I3_VERTEX_ARRAY_DX_H

#include "i3VertexArray.h"

class i3RenderContext;

#if defined( I3G_DX)

class I3_EXPORT_GFX i3VertexArrayDX : public i3VertexArray
{
	I3_CLASS_DEFINE( i3VertexArrayDX);
protected:
	IDirect3DVertexBuffer9 *		m_pVB;
	IDirect3DVertexDeclaration9 *	m_pVBDecl;

	virtual UINT32	CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count);

public:
	i3VertexArrayDX(void);
	virtual ~i3VertexArrayDX(void);

	IDirect3DVertexDeclaration9	*	getDXVertexDecl(void)			{ return m_pVBDecl; }

	virtual BOOL	Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE usage);
	virtual BOOL	Lock( I3G_LOCKMETHOD lock = I3G_LOCK_NONE, UINT32 StartIdx = 0, UINT32 Count = 0);
	virtual void	Unlock(void);

	virtual BOOL	Destroy( bool bLostDevice);
	virtual BOOL	isDestroyed( void);
	virtual BOOL	Revive( i3RenderContext * pCtx);
};

#endif

#endif