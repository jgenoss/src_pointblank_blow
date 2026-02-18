#if !defined( __I3_INDEX_ARRAY_MEM_H)
#define __I3_INDEX_ARRAY_MEM_H

#include "i3GfxResource.h"
#include "i3IndexArray.h"

class I3_EXPORT_GFX i3IndexArrayMem : public i3IndexArray
{
	I3_CLASS_DEFINE( i3IndexArrayMem);

protected:
	UINT16 *		m_pIdxBuffer;

public:
	i3IndexArrayMem(void);
	virtual ~i3IndexArrayMem(void);

	virtual BOOL Create( UINT32 count, I3G_USAGE usage);
	virtual void	Lock( UINT32 StartIdx = 0, UINT32 Count = 0, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock(void);
};

#endif
