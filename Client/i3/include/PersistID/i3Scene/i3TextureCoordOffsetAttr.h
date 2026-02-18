#if !defined( I3SG_TEXTURE_COORD_OFFSET_ATTR_H)
#define I3SG_TEXTURE_COORD_OFFSET_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextureCoordOffsetAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3TextureCoordOffsetAttr);
protected:
	INT32				m_OutputElementCount;
	VEC3D				m_Offset;
	I3G_TEXTURE_BIND	m_BindType;

public:
	i3TextureCoordOffsetAttr(void);
	virtual ~i3TextureCoordOffsetAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND bind)		{ m_BindType = bind; }
	I3G_TEXTURE_BIND	GetBindType(void)						{ return m_BindType; }

	void				SetOutputElementCount( INT32 Count)		{ m_OutputElementCount = Count; }
	INT32				GetOutputElementCount(void)				{ return m_OutputElementCount; }

	VEC3D *				GetOffset(void)							{ return &m_Offset; }
	void				SetOffset( VEC3D * pVec)				{ i3Vector::Copy( &m_Offset, pVec); }

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

