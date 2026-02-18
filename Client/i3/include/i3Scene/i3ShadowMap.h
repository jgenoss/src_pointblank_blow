#if !defined( __I3_SHADOW_MAP_H)
#define __I3_SHADOW_MAP_H

#include "i3Node.h"
#include "i3Camera.h"
#include "i3AllAttrs.h"
#include "i3PhysixDefine.h"
#include "i3SceneUtil.h"
#include "i3SATGenerator.h"
#include "i3VSM_BlurFilter.h"

#define		I3SG_SHADOWMAP_STATE_CASTER_RENDERED	0x00000001

#define		I3SG_SHADOWMAP_STYLE_AUTOFIT			0x00000001
#define		I3SG_SHADOWMAP_STYLE_RENDERCASTER		0x00000002

typedef i3TBandBuffer<VEC3D>		i3VEC3DArray;

class I3_EXPORT_SCENE i3ShadowMap : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3ShadowMap, i3Node);
public:
	enum METHOD
	{
		NORMAL,
		CASCADE,
		PERSPECTIVE,
		LIGHTSPACE_PERSPECTIVE,
		TRAPEZOIDAL
	};

protected:
	UINT32						m_Style = I3SG_SHADOWMAP_STYLE_AUTOFIT; // | I3SG_SHADOWMAP_STYLE_RENDERCASTER;;
	METHOD						m_Method = LIGHTSPACE_PERSPECTIVE;
	i3Node *					m_pSg = nullptr;

	i3Light *					m_pLight = nullptr;
	MATRIX						m_mLightTx;
	COLORREAL					m_ShadowColor = { 0.2f, 0.2f, 0.2f, 0.7f };
	
	UINT32						m_State = 0;

	i3Shader *					m_pCasterShader = nullptr;

	i3RenderAttrListAttr *		m_pDisplayList = nullptr;

	i3ViewMatrixAttr *			m_pLightViewMatrixAttr = nullptr;
	i3ProjectMatrixAttr *		m_pLightProjectMatrixAttr = nullptr;
	i3RenderTargetAttr *		m_pRenderTargetAttr = nullptr;
	i3ClearRenderTargetAttr *	m_pClearRenderTargetAttr = nullptr;
	i3ShaderAttr *				m_pCasterShaderAttr = nullptr;

	i3TextureBindAttr *			m_pTexBindAttr = nullptr;
	i3TextureEnableAttr *		m_pTexEnableAttr = nullptr;
	i3ZWriteEnableAttr *		m_pZWriteEnableAttr = nullptr;
	i3FaceCullModeAttr *		m_pFaceCullModeAttr = nullptr;
	i3GeometryAttr *			m_pRectGeoAttr = nullptr;

#if defined( I3_DEBUG)
	i3Shader *					m_pDebugViewShader = nullptr;
	i3ShaderAttr *				m_pDebugViewShaderAttr = nullptr;

	i3GeometryAttr *			m_pRectGeoAttr2 = nullptr;
	i3TextureBindAttr *			m_pTexBindAttr2 = nullptr;
	i3ViewMatrixAttr *			m_pViewMatrixAttr2 = nullptr;

	i3ProjectMatrixAttr *		m_pProjectMatrixAttr = nullptr;
	i3ViewMatrixAttr *			m_pViewMatrixAttr = nullptr;
#endif

	REAL32						m_zNear = 0.0f, m_zFar = 0.0f, m_fLSPSM_Nopt = 0.0f, m_fCosGamma = 0.0f;
	MATRIX						m_LitSpaceBasis;
	i3BoundBox					m_LitSpaceBound;
	REAL32						m_fLSPSM_NoptWeight = 1.0f;
	bool						m_bUnitCubeClip = false;

	//
	i3PolygonList				m_BodyPList;
	i3Vector3List				m_B;
	i3Vector3List				m_LVS;					// View Frustum * Scene Bound의 교집합
	i3BoundBox					m_ReceiverBound;
	VEC3D						m_vCamDirWS;
	VEC3D						m_vCamPosWS;
	VEC3D						m_vLightDirWS;
	VEC3D						m_NearestP_WS;

	i3PolygonList				intersectionEdges;
	i3PolygonList				objIn;
	
	REAL32						m_fCutOff = 0.0f;

	i3BoundBox					m_BBox;
	REAL32						m_AutoFitCamDist = 25.0f;

	i3MatrixStack				m_MStack;

	i3SATGenerator *			m_pSAT = nullptr;
	i3VSM_BlurFilter *			m_pBlur = nullptr;

protected:
	bool		CreateCasterShader(void);

	void		CalcShadowMatrix( i3SceneTracer * pTracer);

protected:
	void		_CalcVirtualCamParam( i3SceneTracer * pTracer);
	static I3SG_TRAVERSAL_RESULT	_CheckBoundProc( i3Node * pNode, void * pData, i3MatrixStack * pStack);

	void		CalcLSPSMShadowMatrix( i3SceneTracer * pTracer);
	void		CalcLSPSMShadowMatrix2( i3SceneTracer * pTracer);

	void		_CalcB( i3SceneTracer * pTracer);
	void		_BuildLiSP_Method1( i3SceneTracer * pTracer);
	void		_BuildLiSP_Method2( i3SceneTracer * pTracer, VEC3D * pLightDir, VEC3D * pRight, VEC3D * pUp, VEC3D * pAt);
	
	void		calcStandardLightSpace( MATRIX * lightView, MATRIX * lightProj, i3SceneTracer * pTracer);
	void		calcPoints( i3Vector3List * B, i3PolygonList * obj);
	void		calcLVS( i3Vector3List * LVS, i3SceneTracer * pTracer);
	void		getNearCameraPointE( VEC3D * pOut, i3SceneTracer * pTracer);
	void		getProjViewDir_ls( VEC3D * pOut, MATRIX * lightSpace, i3SceneTracer * pTracer);
	void		getZ0_ls( VEC3D * pOut, MATRIX * lightSpace, VEC3D * e, REAL32 b_lsZmax);
	REAL32		calcNoptGeneral( MATRIX * lightSpace, i3BoundBox * B_ls, i3SceneTracer * pTracer);
	void		getLispSmMtx( MATRIX * pOut, MATRIX * lightSpace, i3Vector3List * B, i3SceneTracer * pTracer);
	void		clipObjectByAABox( i3PolygonList * pPolyList, i3BoundBox * pBox);
	void		clipObjectByPositiveHalfSpaceOfPlane( i3PolygonList * obj, VEC4D * A, i3PolygonList * objOut);

public:
	i3ShadowMap(void);
	virtual ~i3ShadowMap(void);

	void		Create(void);

	UINT32		getState(void)						{ return m_State; }
	void		setState( UINT32 state)				{ m_State = state; }
	void		addState( UINT32 state)				{ m_State |= state; }
	void		removeState( UINT32 state)			{ m_State &= ~state; }

	UINT32		getStyle(void)						{ return m_Style; }
	void		setStyle( UINT32 style)				{ m_Style = style; }
	void		addStyle( UINT32 style)				{ m_Style |= style; }
	void		removeStyle( UINT32 style)			{ m_Style &= ~style; }

	i3Light *	getLight(void)						{ return m_pLight; }
	void		setLight( i3Light * pLight);

	MATRIX *	getLightTransform(void)				{ return &m_mLightTx; }
	void		SetLightTransform( MATRIX * pMtx);

	COLORREAL *	getShadowColor(void)				{ return &m_ShadowColor; }
	void		setShadowColor( COLORREAL * pCol)	{ i3Color::Set( &m_ShadowColor, pCol); }
	void		setShadowColor( REAL32 r, REAL32 g, REAL32 b, REAL32 a)
	{
		i3Color::Set( &m_ShadowColor, r, g, b, a);
	}

	void		SetBoundBox( VEC3D * pMin, VEC3D * pMax)	{ m_BBox.SetMinMax( pMin, pMax); }

	REAL32		getAutoFitCamDist(void)				{ return m_AutoFitCamDist; }
	void		setAutoFitCamDist( REAL32 fVal)		{ m_AutoFitCamDist = fVal; }

	void		Apply( i3Light * pLight, i3SceneTracer * pTracer, i3Node * pSg, i3BoundBox * pBBox);
	void		Reset(void);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual void	OnLostDevice( bool bLostDevice) override;
	virtual void	OnRevive( i3RenderContext * pCtx) override;
};

#endif
