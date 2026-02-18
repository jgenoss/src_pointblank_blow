#if !defined( __I3G_SHADER_PARAM_MATRIX_H)
#define __I3G_SHADER_PARAM_MATRIX_H

#include "i3ShaderParam.h"

class I3_EXPORT_GFX i3ShaderParamMatrix : public i3ShaderParam
{
	I3_CLASS_DEFINE( i3ShaderParamMatrix);
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
	TYPE		m_Type;
	MATRIX		m_Matrix;

public:
	i3ShaderParamMatrix(void);
	virtual ~i3ShaderParamMatrix(void);

	TYPE			getType(void)				{ return m_Type; }
	void			SetType( TYPE type);

	MATRIX *		getMatrix(void)				{ return &m_Matrix; }
	void			setMatrix( MATRIX * pMtx)	{ i3Matrix::Copy( &m_Matrix, pMtx); }

	virtual void	Update( i3RenderContext * pCtx);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
