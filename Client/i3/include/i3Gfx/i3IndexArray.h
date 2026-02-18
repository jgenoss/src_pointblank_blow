#if !defined( __I3_INDEX_ARRAY_H)
#define __I3_INDEX_ARRAY_H

#include "i3GfxResource.h"

enum I3G_IDX_TYPE
{
	I3G_IDX_16BITS,
	I3G_IDX_32BITS
};

#define	I3G_INDEXARRAY_STATE_BLENDSORTED		0x00000001
#define I3G_INDEXARRAY_STATE_LOCKED				0x00000002

class I3_EXPORT_GFX i3IndexArray : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3IndexArray, i3GfxResource);

protected:
	UINT32			m_Count = 0;
	I3G_IDX_TYPE	m_DataType = I3G_IDX_16BITS;

	UINT32			m_LockStartIdx = 0;
	UINT32			m_LockCount = 0;
	UINT32			m_State = 0;

	#if defined( I3G_DX) || defined( I3G_XBOX)
		IDirect3DIndexBuffer9 *			m_pBuffer = nullptr;
	#elif defined( I3G_PSP) || defined( I3G_OGLES)
		UINT16 *						m_pData;
	#endif

	i3Mutex			m_Mutex;

public:
	char *			m_pLockedData = nullptr;

public:
	i3IndexArray(void);
	virtual ~i3IndexArray(void);

	virtual bool Create( UINT32 count, I3G_USAGE usage, UINT32 vaCount);
	virtual bool Create( UINT32 count, I3G_USAGE usage, I3G_IDX_TYPE type);

	bool			IsLocked( void)							{ return (m_State & I3G_INDEXARRAY_STATE_LOCKED) == I3G_INDEXARRAY_STATE_LOCKED; }

	UINT32			GetCount(void)							{ return m_Count; }
	char *			GetData(void)							{ return m_pLockedData; }

	I3G_IDX_TYPE	GetDataType(void)						{ return m_DataType; }
	void			SetDataType( I3G_IDX_TYPE type);

	bool			isOptimizedForVertexBlend(void)			{ return (m_State & I3G_INDEXARRAY_STATE_BLENDSORTED) != 0; }
	void			setOptimizedForVertexBlend( bool bState)
	{
		if( bState)
			m_State |= I3G_INDEXARRAY_STATE_BLENDSORTED;
		else
			m_State &= ~I3G_INDEXARRAY_STATE_BLENDSORTED;
	}

	#if defined( I3G_DX) || defined( I3G_XBOX)
		IDirect3DIndexBuffer9 *		GetDxBuffer(void)					{ return m_pBuffer; }
	#elif defined( I3G_PSP) || defined( I3G_OGLES)
		UINT16 *	GetInstance(void)									{ return m_pData; }
	#endif

	virtual bool	Lock( UINT32 StartIdx = 0, UINT32 Count = 0, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock(void);

	virtual bool	SafeLock( UINT32 StartIdx = 0, UINT32 Count = 0, I3G_LOCKMETHOD lock = 0);
	virtual bool	SafeUnlock(void);

	void	SetIndex( UINT32 idx, UINT32 val);
	void	SetIndices( UINT32 idx, UINT32 cnt, UINT16 * pArray, UINT32 off = 0);
	void	SetIndices( UINT32 idx, UINT32 cnt, UINT32 * pArray, UINT32 off = 0);
	UINT32	GetIndex( UINT32 idx);

	virtual void		MakeRestoreData( void) override;
	virtual bool		Destroy( bool bLostDevice) override;
	virtual bool		isDestroyed( void) override;
	virtual bool		Revive( i3RenderContext * pCtx) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

#if defined( I3_DEBUG)
	virtual void		OnQueryStat( I3G_RES_STAT * pInfo) override;
#endif


private:
	bool		m_bInThread = false;

public:
	void		setThreadMode( bool bEnable)			{ m_bInThread = bEnable; }
	bool		getThreadMode( void)					{ return m_bInThread; }

	UINT32		CreateDXBuffer( UINT32 Sz, UINT32 Usage, D3DFORMAT fmt, D3DPOOL Pool);
	UINT32		LockDXBuffer( UINT32 start, UINT32 size, I3G_LOCKMETHOD lock);
	UINT32		UnlockDXBuffer( void);

	UINT32		GetReqResult( UINT32 req);

	static void UpdateForThread( void);
	//static void LostDeviceForThread( void);
};

#endif
