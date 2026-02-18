#if !defined( I3SG_TEXTURE_COORD_OFFSET_ATTR_H)
#define I3SG_TEXTURE_COORD_OFFSET_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextureCoordOffsetAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3TextureCoordOffsetAttr, i3RenderAttr);
protected:
	INT32				m_OutputElementCount = 0;
	VEC3D				m_Offset;
	I3G_TEXTURE_BIND	m_BindType = I3G_TEXTURE_BIND_DIFFUSE;

public:
	i3TextureCoordOffsetAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND bind)		{ m_BindType = bind; }
	I3G_TEXTURE_BIND	GetBindType(void)						{ return m_BindType; }

	void				SetOutputElementCount( INT32 Count)		{ m_OutputElementCount = Count; }
	INT32				GetOutputElementCount(void)				{ return m_OutputElementCount; }

	VEC3D *				GetOffset(void)							{ return &m_Offset; }
	void				SetOffset( VEC3D * pVec)				{ i3Vector::Copy( &m_Offset, pVec); }

	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual void		Apply( i3RenderContext * pContext) override;

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void		Dump(void) override;
	#endif
};

#endif

