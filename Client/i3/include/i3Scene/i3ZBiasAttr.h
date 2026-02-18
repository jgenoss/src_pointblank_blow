#if !defined( __I3_Z_BIAS_ATTR_H)
#define __I3_Z_BIAS_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ZBiasAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3ZBiasAttr, i3RenderAttr);
protected:
	REAL32		m_fSlope = 0.0f;
	REAL32		m_fBias = 0.0f;

public:
	i3ZBiasAttr(void);

	REAL32		getSlope(void)					{ return m_fSlope; }
	void		setSlope( REAL32 fVal)			{ m_fSlope = fVal; }

	REAL32		getBias(void)					{ return m_fBias; }
	void		setBias( REAL32 fVal)			{ m_fBias = fVal; }

	virtual void	Apply( i3RenderContext * pCtx) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual bool	IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void Dump(void) override;
	#endif
};

#endif
