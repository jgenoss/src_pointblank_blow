#if !defined( __I3_INDEX_ARRAY_MEM_H)
#define __I3_INDEX_ARRAY_MEM_H

#include "i3GfxResource.h"
#include "i3IndexArray.h"

class I3_EXPORT_GFX i3IndexArrayMem : public i3IndexArray
{
	I3_EXPORT_GHOST_CLASS_DEFINE( i3IndexArrayMem, i3IndexArray);
protected:
	char *		m_pIdxBuffer = nullptr;

public:
	virtual ~i3IndexArrayMem(void);

	virtual bool	Create( UINT32 count, I3G_USAGE usage, UINT32 vaCount) override;
	virtual bool	Create( UINT32 count, I3G_USAGE usage, I3G_IDX_TYPE type) override;
	virtual bool	Lock( UINT32 StartIdx = 0, UINT32 Count = 0, I3G_LOCKMETHOD lock = 0) override;
	virtual void	Unlock(void) override;

	virtual bool	SafeLock( UINT32 StartIdx = 0, UINT32 Count = 0, I3G_LOCKMETHOD lock = 0) override;
	virtual bool	SafeUnlock(void) override;

	virtual bool		Destroy(bool bLostDevice) override { return true; }
	virtual bool		isDestroyed(void) override { return true; }
	virtual bool		Revive(i3RenderContext * pCtx) override { return true; }
};

#endif
