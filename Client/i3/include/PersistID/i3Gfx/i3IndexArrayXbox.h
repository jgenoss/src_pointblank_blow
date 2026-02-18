#ifndef __I3_INDEX_ARRAY_XBOX_H
#define __I3_INDEX_ARRAY_XBOX_H

#if !defined(I3_XBOX) || !defined(I3G_XBOX)
#error "This header file is only for Xbox."
#endif

#include "i3IndexArray.h"

class i3IndexArrayXbox : public i3IndexArray
{
	I3_CLASS_DEFINE( i3IndexArrayXbox );

protected:
	IDirect3DIndexBuffer8 *			m_pBuffer;
	UINT16 *						m_pLockedData;
	
public:
	i3IndexArrayXbox(void);
	virtual ~i3IndexArrayXbox(void);

	IDirect3DIndexBuffer8 *		GetDxBuffer(void)					{ return m_pBuffer; }

	virtual void SetUsageFlag( UINT32 flag);

	virtual BOOL Create( UINT32 count, I3G_USAGE usage);

	virtual void	Lock( UINT32 StartIdx = 0, UINT32 count = 0, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock(void);

	virtual void	SetIndex16( UINT32 idx, UINT16 val);
	virtual UINT16	GetIndex16( UINT32 idx);

	virtual UINT16 *	GetData(void);
};

typedef i3IndexArrayXbox i3IndexArrayPlatform;

#endif