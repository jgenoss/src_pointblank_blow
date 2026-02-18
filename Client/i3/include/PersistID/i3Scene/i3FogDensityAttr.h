#if !defined( __I3_FOG_DENSITY_H)
#define __I3_FOG_DENSITY_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3FogDensityAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3FogDensityAttr);

protected:
	RT_REAL32		m_fDensity;

public:
	i3FogDensityAttr(void);
	virtual ~i3FogDensityAttr(void);

	void		Set( RT_REAL32 val)
	{
		m_fDensity = val;
	}

	RT_REAL32		Get(void)
	{
		return m_fDensity;
	}

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
