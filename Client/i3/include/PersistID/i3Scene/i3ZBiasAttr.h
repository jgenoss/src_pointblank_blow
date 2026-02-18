#if !defined( __I3_Z_BIAS_ATTR_H)
#define __I3_Z_BIAS_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ZBiasAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3ZBiasAttr);
protected:
	REAL32		m_fSlope;
	REAL32		m_fBias;

public:
	i3ZBiasAttr(void);
	virtual ~i3ZBiasAttr(void);

	REAL32		getSlope(void)					{ return m_fSlope; }
	void		setSlope( REAL32 fVal)			{ m_fSlope = fVal; }

	REAL32		getBias(void)					{ return m_fBias; }
	void		setBias( REAL32 fVal)			{ m_fBias = fVal; }

	virtual void	Apply( i3RenderContext * pCtx);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void Dump(void);
	#endif
};

#endif
