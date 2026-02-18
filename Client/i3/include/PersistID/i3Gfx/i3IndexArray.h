#if !defined( __I3_INDEX_ARRAY_H)
#define __I3_INDEX_ARRAY_H

#include "i3GfxResource.h"

class I3_EXPORT_GFX i3IndexArray : public i3GfxResource
{
	I3_CLASS_DEFINE( i3IndexArray);

protected:
	UINT32			m_Count;
	UINT32			m_LockStartIdx;
	UINT16 *		m_pLockedData;

	#if defined( I3G_DX) || defined( I3G_XBOX)
		IDirect3DIndexBuffer9 *			m_pBuffer;
	#elif defined( I3G_PSP) || defined( I3G_OGLES)
		UINT16 *						m_pData;
	#endif

public:
	i3IndexArray(void);
	virtual ~i3IndexArray(void);

	virtual BOOL Create( UINT32 count, I3G_USAGE usage);

	UINT32			GetCount(void)							{ return m_Count; }
	UINT16 *		GetData(void)							{ return m_pLockedData; }

	#if defined( I3G_DX) || defined( I3G_XBOX)
		IDirect3DIndexBuffer9 *		GetDxBuffer(void)					{ return m_pBuffer; }
	#elif defined( I3G_PSP) || defined( I3G_OGLES)
		UINT16 *	GetInstance(void)									{ return m_pData; }
	#endif

	virtual void	Lock( UINT32 StartIdx = 0, UINT32 Count = 0, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock(void);

	void	SetIndex16( UINT32 idx, UINT16 val);
	void	SetIndices16( UINT32 Idx, UINT32 count, UINT16 * pIndices, UINT16 off = 0);
	UINT16	GetIndex16( UINT32 idx);

	virtual BOOL		Destroy( bool bLostDevice);
	virtual BOOL		isDestroyed( void);
	virtual BOOL		Revive( i3RenderContext * pCtx);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
