#if !defined( __I3_PROJECTIVE_TEXTURE_SHADER_H)
#define __I3_PROJECTIVE_TEXTURE_SHADER_H

#include "i3Gfx.h"
#include "i3Node.h"
#include "i3TextureBindAttr.h"
#include "i3BlendEnableAttr.h"
#include "i3BlendModeAttr.h"
#include "i3TextureWrapAttr.h"
#include "i3TextureFunctionAttr.h"
#include "i3TextureMatrixAttr.h"
#include "i3TextureEnableAttr.h"
#include "i3TextureCoordSourceAttr.h"
#include "i3LightingEnableAttr.h"
#include "i3ZFuncAttr.h"
#include "i3SceneTracer.h"

//
// *. Diffuse Texture와 Protected Texture가 필요하다.
// 
// 1-Pass
//		* Diffuse Texture를 설정한다.
//		* 일반적인 설정으로 해당 Scene Graph를 Rendering 한다.
//
// 2-Pass
//		* Texture를 Projected Texture로 설정한다.
//		* Texture Coord Source를 CAMERA_SPACE_POSITION으로 설정한다.
//		* Texture Matrix를 아래와 같이 설정한다.
//				  [Inverse View Matrix]
//				* [Projector-Transform Matrix]
//				* [Projection Matrix]
//				* [Scale & Translate for Texture Coord. System]
//		* Texture Wrapping Mode를 Clamp로 설정.
//		* Blend Enable

class I3_EXPORT_SCENE i3ProjectiveTextureShader : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3ProjectiveTextureShader, i3Node);

protected:
	i3MatrixObject *		m_pTransformMatrixObj;		// [initialize at constructor]
	i3MatrixObject *		m_pInvTransformMatrixObj;	// [initialize at constructor]
	i3MatrixObject *		m_pProjectionMatrixObj;		// [initialize at constructor]
	i3Texture *				m_pProjectTexture = nullptr;

	I3G_BLEND				m_SrcBlend;					// [initialize at constructor]
	I3G_BLEND				m_DestBlend;				// [initialize at constructor]

	i3Node *				m_pScene = nullptr;

	// Volatile members
	i3RenderAttrListAttr *		m_pDispList;			// [initialize at constructor]
	i3TextureBindAttr *			m_pTexBind;				// [initialize at constructor]
	i3BlendEnableAttr *			m_pBlendEnableAttr;		// [initialize at constructor]
	i3BlendModeAttr *			m_pBlendModeAttr;		// [initialize at constructor]
	i3TextureWrapAttr *			m_pTexWrap;				// [initialize at constructor]
	i3TextureFunctionAttr *		m_pTexFunc;				// [initialize at constructor]
	i3TextureCoordSourceAttr *	m_pTexCoordSourceAttr;	// [initialize at constructor]
	i3TextureMatrixAttr *		m_pTextureMatrixAttr;	// [initialize at constructor]
	i3TextureEnableAttr *		m_pTextureEnableAttr;	// [initialize at constructor]
	i3LightingEnableAttr *		m_pLightingEnableAttr;	// [initialize at constructor]
	i3ZFuncAttr *				m_pZFuncAttr;			// [initialize at constructor]

public:
	i3ProjectiveTextureShader(void);
	virtual ~i3ProjectiveTextureShader(void);

	void		SetSceneGraph( i3Node * pNode);
	i3Node *	GetSceneGraph(void)								{ return m_pScene; }

	void		SetProjectiveTexture( i3Texture * pTex);
	i3Texture *	GetProjectiveTexture(void)						{ return m_pProjectTexture; }

	void		SetTransformMatrix( MATRIX * pMatrix, MATRIX * pInvMatrix);
	MATRIX *	GetTransformMatrix(void)						{ return m_pTransformMatrixObj->GetMatrix(); }
	
	i3MatrixObject *	GetTransformMatrixObject(void)				{ return m_pTransformMatrixObj; }
	i3MatrixObject *	GetInverseTransformMatrixObject(void)		{ return m_pInvTransformMatrixObj; }
	void				SetTransformMatrixObject( i3MatrixObject * pMatObj, i3MatrixObject * pInvMatObj);

	void		SetProjectionMatrix( MATRIX * pMatrix);
	MATRIX *	GetProjectionMatrix(void)						{ return m_pProjectionMatrixObj->GetMatrix(); }

	i3MatrixObject *	GetProjectionMatrixObject(void)				{ return m_pProjectionMatrixObj; }
	void				SetProjectionMatrixObject( i3MatrixObject * pMatObj);

	void		SetSrcBlend( I3G_BLEND blend);
	I3G_BLEND	GetSrcBlend(void)								{ return m_SrcBlend; }

	void		SetDestBlend( I3G_BLEND blend);
	I3G_BLEND	GetDestBlend(void)								{ return m_DestBlend; }

	void		Apply( i3SceneTracer * pTracer);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
