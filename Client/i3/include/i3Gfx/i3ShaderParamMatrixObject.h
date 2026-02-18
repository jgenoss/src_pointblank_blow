#if !defined( __I3G_SHADER_PARAM_MATRIX_OBJECT_H)
#define __I3G_SHADER_PARAM_MATRIX_OBJECT_H

#include "i3ShaderParam.h"

class I3_EXPORT_GFX i3ShaderParamMatrixObject : public i3ShaderParam
{
	I3_EXPORT_CLASS_DEFINE( i3ShaderParamMatrixObject, i3ShaderParam);
protected:
	i3MatrixObject	*	m_pMatrixObj = nullptr;

public:
	i3ShaderParamMatrixObject(void);
	virtual ~i3ShaderParamMatrixObject(void);

	MATRIX *		getMatrix(void)				{ return m_pMatrixObj->GetMatrix(); }
	void			setMatrix( MATRIX * pMtx)	{ m_pMatrixObj->SetMatrix( pMtx); }

	i3MatrixObject *	getMatrixObject(void)	{ return m_pMatrixObj; }
	void				SetMatrixObject( i3MatrixObject * pObj);

	virtual void	Update( i3RenderContext * pCtx) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
