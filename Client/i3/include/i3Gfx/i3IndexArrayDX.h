#if !defined( __I3_INDEX_ARRAY_DX_H)
#define __I3_INDEX_ARRAY_DX_H

#if defined( I3G_DX)
#include "i3IndexArray.h"

class i3IndexArrayDX : public i3IndexArray
{
	I3_CLASS_DEFINE( i3IndexArrayDX);

protected:
	IDirect3DIndexBuffer8 *			m_pBuffer;
	UINT16 *						m_pLockedData;
	
public:
	i3IndexArrayDX(void);
	virtual ~i3IndexArrayDX(void);

	IDirect3DIndexBuffer8 *		GetDxBuffer(void)					{ return m_pBuffer; }

	virtual void SetUsageFlag( UINT32 flag);

	virtual BOOL Create( UINT32 count, I3G_USAGE usage, UINT32 vaCount);
	virtual BOOL Create( UINT32 count, I3G_USAGE usage, I3G_IDX_TYPE type);

	virtual void	Lock( UINT32 StartIdx = 0, UINT32 count = 0, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock(void);

	virtual void	SetIndex16( UINT32 idx, UINT16 val);
	virtual UINT16	GetIndex16( UINT32 idx);

	virtual UINT16 *	GetData(void);
};

typedef i3IndexArrayDX i3IndexArrayPlatform;

#endif

#endif