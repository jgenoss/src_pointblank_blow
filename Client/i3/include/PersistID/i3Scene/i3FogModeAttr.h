#if !defined( __I3_FOG_MODE_ATTR_H)
#define __I3_FOG_MODE_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3FogModeAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3FogModeAttr);

protected:
	I3G_FOG_MODE		m_Mode;

public:
	i3FogModeAttr(void);
	virtual ~i3FogModeAttr(void);

	void			Set( I3G_FOG_MODE mode)
	{
		m_Mode = mode;
	}

	I3G_FOG_MODE	Get(void)
	{
		return m_Mode;
	}

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
