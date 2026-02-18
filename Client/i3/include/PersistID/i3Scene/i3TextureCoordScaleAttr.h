#if !defined( I3SG_TEXTURE_COORD_SCALE_ATTR_H)
#define I3SG_TEXTURE_COORD_SCALE_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextureCoordScaleAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3TextureCoordScaleAttr);
protected:
	INT32				m_OutputElementCount;
	VEC3D				m_ScaleFactor;
	I3G_TEXTURE_BIND	m_BindType;

public:
	i3TextureCoordScaleAttr(void);
	virtual ~i3TextureCoordScaleAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND idx)		{ m_BindType = idx; }
	I3G_TEXTURE_BIND	GetBindType(void)						{ return m_BindType; }

	void				SetOutputElementCount( INT32 Count)		{ m_OutputElementCount = Count; }
	INT32				GetOutputElementCount(void)				{ return m_OutputElementCount; }

	VEC3D *				GetScaleFactor(void)					{ return &m_ScaleFactor; }
	void				SetScaleFactor( VEC3D * pVec)			{ i3Vector::Copy( &m_ScaleFactor, pVec); }

	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual void		Apply( i3RenderContext * pContext);
	virtual BOOL		IsSame( i3RenderAttr * pAttr);

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void		Dump(void);
	#endif
};

#endif

