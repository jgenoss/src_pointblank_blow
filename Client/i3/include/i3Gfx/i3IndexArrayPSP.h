#if !defined( __I3_INDEX_ARRAY_PSP_H)
#define __I3_INDEX_ARRAY_PSP_H

#if defined( I3G_PSP)
#include "i3IndexArray.h"

/*

// Performance의 향상을 위해 i3IndexArray에서 파생하지 않고, 직접 PSP Version Code를
// 추가하는 것으로 수정함.

class i3IndexArrayPSP : public i3IndexArray
{
	I3_CLASS_DEFINE( i3IndexArrayPSP);

protected:
	UINT16 *						m_pData;
	
public:
	i3IndexArrayPSP(void);
	virtual ~i3IndexArrayPSP(void);

	virtual BOOL Create( UINT32 count, I3G_USAGE usage);

	virtual void	Lock( UINT32 StartIdx = 0, UINT32 count = 0, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock(void);

	virtual void	SetIndex16( UINT32 idx, UINT16 val);
	virtual UINT16	GetIndex16( UINT32 idx);

	virtual UINT16 *	GetData(void);
};

*/

#endif

#endif
