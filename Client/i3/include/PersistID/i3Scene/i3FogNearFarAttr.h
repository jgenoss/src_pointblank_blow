#if !defined( __I3_FOG_NEARFAR_ATTR_H)
#define __I3_FOG_NEARFAR_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3FogNearFarAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3FogNearFarAttr);

protected:
	RT_REAL32		m_fNear;
	RT_REAL32		m_fFar;

public:
	i3FogNearFarAttr(void);
	virtual ~i3FogNearFarAttr(void);

	void		SetNear( RT_REAL32 val)
	{
		m_fNear = val;
	}

	void		SetFar( RT_REAL32 val)
	{
		m_fFar = val;
	}

	RT_REAL32		GetNear(void)
	{
		return m_fNear;
	}

	RT_REAL32		GetFar(void)
	{
		return m_fFar;
	}

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
