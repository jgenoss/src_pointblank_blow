#if !defined( __I3_PIXEL_SHADER_ATTR_H)
#define __I3_PIXEL_SHADER_ATTR_H

#include "i3RenderAttr.h"

class i3PixelShaderAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3PixelShaderAttr);
protected:
	i3PixelShader *	m_pShader;

public:
	i3PixelShaderAttr(void);
	virtual ~i3PixelShaderAttr(void);

	i3PixelShader *		getShader(void)				{ return m_pShader; }
	void				SetShader( i3PixelShader * pShader);

	virtual void		Apply( i3RenderContext * pContext);

	virtual	void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
