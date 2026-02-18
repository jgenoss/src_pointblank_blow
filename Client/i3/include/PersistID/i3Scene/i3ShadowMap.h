#if !defined( __I3_SHADOW_MAP_H)
#define __I3_SHADOW_MAP_H

#include "i3Node.h"
#include "i3Camera.h"
#include "i3AllAttrs.h"
#include "i3PhysixDefine.h"
#include "i3SceneUtil.h"
#include "i3SATGenerator.h"

#define		I3SG_SHADOWMAP_STATE_CASTER_RENDERED	0x00000001

#define		I3SG_SHADOWMAP_STYLE_AUTOFIT			0x00000001
#define		I3SG_SHADOWMAP_STYLE_RENDERCASTER		0x00000002

typedef i3TBandBuffer<VEC3D>		i3VEC3DArray;

class I3_EXPORT_SCENE i3ShadowMap : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3ShadowMap);
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
	UINT32						m_Style;
	METHOD						m_Method;
	i3Node *					m_pSg;

	i3Light *					m_pLight;
	MATRIX						m_mLightTx;
	COLORREAL					m_ShadowColor;
	
	UINT32						m_State;

	i3Shader *					m_pCasterShader;

	i3RenderAttrListAttr *		m_pDisplayList;

	i3ViewMatrixAttr *			m_pLightViewMatrixAttr;
	i3ProjectMatrixAttr *		m_pLightProjectMatrixAttr;
	i3RenderTargetAttr *		m_pRenderTargetAttr;
	i3ClearRenderTargetAttr *	m_pClearRenderTargetAttr;
	i3ShaderAttr *				m_pCasterShaderAttr;

	i3TextureBindAttr *			m_pTexBindAttr;
	i3TextureEnableAttr *		m_pTexEnableAttr;
	i3ZWriteEnableAttr *		m_pZWriteEnableAttr;
	i3FaceCullModeAttr *		m_pFaceCullModeAttr;
	i3GeometryAttr *			m_pRectGeoAttr;

#if defined( I3_DEBUG)
	i3Shader *					m_pDebugViewShader;
	i3ShaderAttr *				m_pDebugViewShaderAttr;

	i3GeometryAttr *			m_pRectGeoAttr2;
	i3TextureBindAttr *			m_pTexBindAttr2;
	i3ViewMatrixAttr *			m_pViewMatrixAttr2;

	i3ProjectMatrixAttr *		m_pProjectMatrixAttr;
	i3ViewMatrixAttr *			m_pViewMatrixAttr;
#endif

	REAL32						m_fAspect, m_zNear, m_zFar, m_ppNear, m_ppFar, m_fSlideBack, m_fLSPSM_Nopt, m_fCosGamma;
	MATRIX						m_LitSpaceBasis;
	i3BoundBox					m_LitSpaceBound;
	REAL32						m_fLSPSM_NoptWeight;
	bool						m_bUnitCubeClip;
	bool						m_bFirstBound;

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
	
	REAL32						m_fCutOff;

	i3BoundBox					m_BBox;
	REAL32						m_AutoFitCamDist;

	i3MatrixStack				m_MStack;

	i3SATGenerator *			m_pSAT;

protected:
	i3AttrSet *					m_pDebugNode;
	i3GeometryAttr *			m_pDebugGeo;
	i3VertexArray *				m_pDebugVA;
	INT32						m_DebugLineCount;

	void						_ResetDebugLines(void);
	void						_AddDebugLine( VEC3D * pStart, VEC3D * pEnd, COLOR * pColor);
	void						_AddDebugDir( VEC3D * pDir, REAL32 len, COLOR * pColor);

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

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

};

#endif
