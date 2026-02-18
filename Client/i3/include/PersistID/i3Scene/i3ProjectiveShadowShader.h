#if !defined( __I3_PROJECTIVE_SHADOW_SHADER_H)
#define __I3_PROJECTIVE_SHADOW_SHADER_H

#include "i3Node.h"
#include "i3RenderTargetAttr.h"
#include "i3ClearRenderTargetAttr.h"
#include "i3ProjectiveTextureShader.h"
#include "i3ColorAttr.h"
#include "i3ZWriteEnableAttr.h"
#include "i3ZTestEnableAttr.h"

class I3_EXPORT_SCENE i3ProjectiveShadowShader : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3ProjectiveShadowShader);

protected:
	i3Node *						m_pCaster;
	i3ProjectiveTextureShader *		m_pShader;
	BOOL							m_bSoftEdge;

	REAL32							m_Range;
	REAL32							m_CutoffAngle;

	// Volatile members
	i3MatrixObject *				m_pViewMatrixObj;
	i3MatrixObject *				m_pInvViewMatrixObj;
	i3MatrixObject *				m_pProjectMatrixObj;

	i3RenderTargetAttr *			m_pRenderTargetAttr;
	i3ClearRenderTargetAttr *		m_pClearRenderTargetAttr;
	i3ColorAttr *					m_pColorAttr;
	i3BlendModeAttr *				m_pBlendModeAttr;
	i3BlendEnableAttr *				m_pBlendEnableAttr;
	i3TextureEnableAttr *			m_pTextureEnableAttr;
	i3LightingEnableAttr *			m_pLightingEnableAttr;
	i3ProjectMatrixAttr *			m_pProjectMatrixAttr;
	i3ViewMatrixAttr *				m_pViewMatrixAttr;
	i3ZWriteEnableAttr *			m_pZWriteEnableAttr;
	i3ZTestEnableAttr *				m_pZTestEnableAttr;
	i3RenderAttrListAttr *			m_pDispList;

protected:
	void							UpdateProjectMatrix(void);

public:
	i3ProjectiveShadowShader(void);
	virtual ~i3ProjectiveShadowShader(void);

	INT32	GetShadowTextureRes(void)				{ return m_pRenderTargetAttr->GetWidth(); }
	void	SetShadowTextureWidth( INT32 w)	
	{ 
		m_pRenderTargetAttr->SetWidth( w);
		m_pRenderTargetAttr->SetHeight( w);
	}

	i3Node *	GetCaster(void)									{ return m_pCaster; }
	void		SetCaster( i3Node * pScene);

	i3Node *	GetReceiver(void)								{ return m_pShader->GetSceneGraph(); }
	void		SetReceiver( i3Node * pScene)					{ m_pShader->SetSceneGraph( pScene); }

	RT_MATRIX *	GetLightTransform( void)						{ return m_pViewMatrixAttr->GetMatrix(); }
	void		SetLightTransform( RT_MATRIX * pMatrix, RT_MATRIX * pInvMatrix = NULL);

	REAL32		GetLightRange(void)								{ return m_Range; }
	void		SetLightRange( REAL32 val);

	REAL32		GetCutoffAngle(void)							{ return m_CutoffAngle; }
	void		SetCutoffAngle( REAL32 val);

	COLOR	*	GetShadowColor(void)							{ return m_pColorAttr->Get(); }
	void		SetShadowColor( COLOR * pCol)					{ m_pColorAttr->Set( pCol); }

	void	Apply( i3SceneTracer * pTracer);

	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif

