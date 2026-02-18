#if !defined( __I3_DISPLAY_LIST_H)
#define __I3_DISPLAY_LIST_H

#include "i3GfxResource.h"

#define I3G_DSPL_STATE_VALID		0x00000001

class I3_EXPORT_GFX i3DisplayList : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3DisplayList, i3GfxResource);
protected:
	UINT32						m_State = 0;

#if defined( I3G_DX)
	IDirect3DStateBlock9 *		m_pStateBlock = nullptr;
#endif

public:
	i3DisplayList(void);
	virtual ~i3DisplayList(void);

#if defined( I3G_DX)
	IDirect3DStateBlock9 *		getDXStateBlock(void)			{ return m_pStateBlock; }
	void						setDXStateBlock( IDirect3DStateBlock9 * pStateBlock)
	{
		m_pStateBlock = pStateBlock;
	}
#endif

	bool				isValid(void)							{ return (m_State & I3G_DSPL_STATE_VALID) != 0; }

	bool				Create(void);

	virtual bool		Destroy( bool bLostDevice) override;
	virtual bool		isDestroyed(void) override { return true; }
	virtual bool		Revive(i3RenderContext * pCtx) override { return true; }
};

#endif
