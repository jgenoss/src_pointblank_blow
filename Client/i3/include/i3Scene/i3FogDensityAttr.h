#if !defined( __I3_FOG_DENSITY_H)
#define __I3_FOG_DENSITY_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3FogDensityAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3FogDensityAttr, i3RenderAttr);

protected:
	REAL32		m_fDensity = 1.0f;

public:
	i3FogDensityAttr(void);

	void		Set( REAL32 val)
	{
		m_fDensity = val;
	}

	REAL32		Get(void)
	{
		return m_fDensity;
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
