#if !defined( __I3_SHADE_MODE_ATTR_H)
#define __I3_SHADE_MODE_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ShadeModeAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3ShadeModeAttr);
protected:
	I3G_SHADE_MODE	m_Mode;

public:
	i3ShadeModeAttr(void);
	virtual ~i3ShadeModeAttr(void);

	void			Set( I3G_SHADE_MODE mode)
	{
		m_Mode = mode;
	}

	I3G_SHADE_MODE	Get(void)
	{
		return m_Mode;
	}

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);
	virtual UINT32 OnSave( i3ResourceFile * pResFile);
	virtual UINT32 OnLoad( i3ResourceFile * pResFile);
};

#endif
