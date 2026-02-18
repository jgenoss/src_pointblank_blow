#if !defined( __I3G_SHADER_PARAM_MATRIX_H)
#define __I3G_SHADER_PARAM_MATRIX_H

#include "i3ShaderParam.h"

class I3_EXPORT_GFX i3ShaderParamMatrix : public i3ShaderParam
{
	I3_EXPORT_CLASS_DEFINE( i3ShaderParamMatrix, i3ShaderParam);
public:
	enum TYPE
	{
		TYPE_CONSTANT,
		TYPE_WORLD,
		TYPE_VIEW,
		TYPE_INVVIEW,
		TYPE_PROJECT
	};
protected:
	TYPE		m_Type = TYPE_CONSTANT;
	MATRIX		m_Matrix;

public:
	i3ShaderParamMatrix(void);

	TYPE			getType(void)				{ return m_Type; }
	void			SetType( TYPE type);

	MATRIX *		getMatrix(void)				{ return &m_Matrix; }
	void			setMatrix( MATRIX * pMtx)	{ i3Matrix::Copy( &m_Matrix, pMtx); }

	virtual void	Update( i3RenderContext * pCtx) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
