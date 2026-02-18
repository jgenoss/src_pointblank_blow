#if !defined( __I3_DISPLAY_LIST_H)
#define __I3_DISPLAY_LIST_H

#include "i3GfxResource.h"

#define I3G_DSPL_STATE_VALID		0x00000001

class I3_EXPORT_GFX i3DisplayList : public i3GfxResource
{
	I3_CLASS_DEFINE( i3DisplayList);
protected:
	UINT32						m_State;

#if defined( I3G_DX)
	IDirect3DStateBlock9 *		m_pStateBlock;
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

	virtual BOOL		Destroy( bool bLostDevice);
	virtual BOOL		isDestroyed( void);
	virtual BOOL		Revive( i3RenderContext * pCtx);

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
};

#endif
