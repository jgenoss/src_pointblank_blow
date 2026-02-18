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
	I3SG_NODE_CLASS_DEFINE( i3ProjectiveShadowShader, i3Node);

protected:
	i3Node *						m_pCaster = nullptr;
	i3ProjectiveTextureShader *		m_pShader;		// [initialize at constructor]
	bool							m_bSoftEdge = true;

	REAL32							m_Range = 1.0f;
	REAL32							m_CutoffAngle = I3_DEG2RAD(45.0f);

	// Volatile members
	i3MatrixObject *				m_pViewMatrixObj;		// [initialize at constructor]
	i3MatrixObject *				m_pInvViewMatrixObj;	// [initialize at constructor]
	i3MatrixObject *				m_pProjectMatrixObj;	// [initialize at constructor]

	i3RenderTargetAttr *			m_pRenderTargetAttr;	// [initialize at constructor]
	i3ClearRenderTargetAttr *		m_pClearRenderTargetAttr;	// [initialize at constructor]
	i3ColorAttr *					m_pColorAttr;			// [initialize at constructor]
	i3BlendModeAttr *				m_pBlendModeAttr;		// [initialize at constructor]
	i3BlendEnableAttr *				m_pBlendEnableAttr;		// [initialize at constructor]
	i3TextureEnableAttr *			m_pTextureEnableAttr;	// [initialize at constructor]
	i3LightingEnableAttr *			m_pLightingEnableAttr;	// [initialize at constructor]
	i3ProjectMatrixAttr *			m_pProjectMatrixAttr;	// [initialize at constructor]
	i3ViewMatrixAttr *				m_pViewMatrixAttr;		// [initialize at constructor]
	i3ZWriteEnableAttr *			m_pZWriteEnableAttr;	// [initialize at constructor]
	i3ZTestEnableAttr *				m_pZTestEnableAttr;		// [initialize at constructor]
	i3RenderAttrListAttr *			m_pDispList;			// [initialize at constructor]

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

	MATRIX *	GetLightTransform( void)						{ return m_pViewMatrixAttr->GetMatrix(); }
	void		SetLightTransform( MATRIX * pMatrix, MATRIX * pInvMatrix = nullptr);

	REAL32		GetLightRange(void)								{ return m_Range; }
	void		SetLightRange( REAL32 val);

	REAL32		GetCutoffAngle(void)							{ return m_CutoffAngle; }
	void		SetCutoffAngle( REAL32 val);

	COLORREAL*	GetShadowColor(void)							{ return m_pColorAttr->Get(); }
	void		SetShadowColor( COLORREAL * pCol)					{ m_pColorAttr->Set( pCol); }

	void	Apply( i3SceneTracer * pTracer);

	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif

