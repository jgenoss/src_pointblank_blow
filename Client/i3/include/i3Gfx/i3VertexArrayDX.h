#if !defined( __I3_VERTEX_ARRAY_DX_H)
#define __I3_VERTEX_ARRAY_DX_H

#include "i3VertexArray.h"

class i3RenderContext;

#if defined( I3G_DX)

struct i3VBUsageIndex
{
	BYTE Pos = 0;
	BYTE Weight = 0;
	BYTE Normal = 0;
	BYTE Color = 0;
	BYTE TexCoord = 0;
	BYTE BlendIdx = 0;
	BYTE Tangent = 0;
	BYTE Binormal = 0;
};

class I3_EXPORT_GFX i3VertexArrayDX : public i3VertexArray
{
	I3_EXPORT_CLASS_DEFINE_BY_TYPE(i3VertexArrayDX, i3VertexArray, i3::cms_concrete_d3d9::value);

protected:
	IDirect3DVertexBuffer9 *		m_pVB = nullptr;
	IDirect3DVertexDeclaration9 *	m_pVBDecl = nullptr;

	INT32		m_TryLockCount = 0;

	virtual UINT32	CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count);

public:
	i3VertexArrayDX(void);
	virtual ~i3VertexArrayDX(void);

	IDirect3DVertexDeclaration9	*	getDXVertexDecl(void)			{ return m_pVBDecl; }
	IDirect3DVertexBuffer9 *		getDXInstance(void)				{ return m_pVB; }

	virtual bool	Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE usage);
	virtual bool	Lock( I3G_LOCKMETHOD lock = I3G_LOCK_NONE, UINT32 StartIdx = 0, UINT32 Count = 0);
	virtual void	Unlock(void);

	virtual bool	SafeLock( I3G_LOCKMETHOD lock = I3G_LOCK_NONE, UINT32 StartIdx = 0, UINT32 Count = 0);
	virtual bool	SafeUnlock(void);

	virtual bool	Destroy( bool bLostDevice);
	virtual bool	isDestroyed( void);
	virtual bool	Revive( i3RenderContext * pCtx);

	virtual void	MakeRestoreData(void);

	#if defined( I3_DEBUG)
	void	OnQueryStat( I3G_RES_STAT * pInfo);
	#endif

	static IDirect3DVertexDeclaration9 *	GetCombinedVBDecl( i3VertexFormat * pStream0, i3VertexFormat * pStream1);
	static UINT32							BuildDecl( INT32 * pidx, i3VertexFormat * pFormat, WORD Stream, i3VBUsageIndex * pUsage);


public:
	UINT32		CalcBufferSizeForThread( i3VertexFormat * pFormat, UINT32 Count);
	UINT32		CreateDXBuffer( UINT32 BuffSz, UINT32 Usage, D3DPOOL Pool);
	UINT32		LockDXBuffer( UINT32 StartOff, UINT32 Size, I3G_LOCKMETHOD lock);
	UINT32		UnlockDXBuffer( void);

	static void UpdateForThread( void);
	static void	LostDeviceForThread( void);

	UINT32		GetDXVAReqInfoIdx( void);
	UINT32		GetReqResult( UINT32 req);
};

#endif

#endif