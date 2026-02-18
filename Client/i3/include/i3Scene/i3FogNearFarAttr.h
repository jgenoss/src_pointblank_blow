#if !defined( __I3_FOG_NEARFAR_ATTR_H)
#define __I3_FOG_NEARFAR_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3FogNearFarAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3FogNearFarAttr, i3RenderAttr);

protected:
	REAL32		m_fNear = 0.0f;
	REAL32		m_fFar = 1.0f;

public:
	i3FogNearFarAttr(void);

	void		SetNear( REAL32 val)
	{
		m_fNear = val;
	}

	void		SetFar( REAL32 val)
	{
		m_fFar = val;
	}

	REAL32		GetNear(void)
	{
		return m_fNear;
	}

	REAL32		GetFar(void)
	{
		return m_fFar;
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
