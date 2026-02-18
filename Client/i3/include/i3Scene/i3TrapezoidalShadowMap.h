#if !defined( __I3_TRAPEZOIDAL_SHADOW_MAP_H)
#define __I3_TRAPEZOIDAL_SHADOW_MAP_H

#include "i3Node.h"
#include "i3Camera.h"
#include "i3AllAttrs.h"

#define		I3SG_SHADOWMAP_STATE_CASTER_RENDERED	0x00000001

class I3_EXPORT_SCENE i3TrapezoidalShadowMap : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3TrapezoidalShadowMap);
protected:
	UINT32						m_Style;
	i3Node *					m_pReceiver;
	i3Node *					m_pCaster;

	i3Light *					m_pLight;
	MATRIX						m_mLightTx;
	MATRIX						m_mInvLightTx;
	MATRIX						m_mProject;
	COLORREAL					m_ShadowColor;
	
	UINT32						m_State;
	i3RenderTargetAttr *		m_pShadowRTAttr;
	i3ClearRenderTargetAttr *	m_pClearRTAttr;

	i3RenderAttrListAttr *		m_pCasterRenderListAttr;

	i3ViewMatrixAttr *			m_pLightViewMatrixAttr;
	i3ProjectMatrixAttr *		m_pLightProjectMatrixAttr;

	i3ShaderAttr *				m_pCasterShaderAttr;

	i3ShadowMapEnableAttr *		m_pShadowMapEnableAttr;

	i3ZTestEnableAttr *			m_pZTestEnableAttr;
	i3FaceCullModeAttr *		m_pFaceCullModeAttr;
	
	REAL32						m_fCutOff;
	REAL32						m_fNear;
	REAL32						m_fFar;
	REAL32						m_fCamNear;
	REAL32						m_fCamFar;

	REAL32						m_fFocusDist;

protected:
	bool		CreateCasterShader(void);

	REAL32		_GetDotWithAt( VEC3D * pV1, VEC3D * pV2, VEC3D * pV3);
	INT32		_GetConvexHull( VEC3D * pvFrusta, VEC3D * pvE);
	void		_GetTPlaneVertex( VEC3D * vFrusta, VEC3D * vE, INT32 vE_Count, VEC3D * vT);
	
	
	void		CalcPostPerspectiveMatrix( i3SceneTracer * pTracer);

public:
	i3TrapezoidalShadowMap(void);
	virtual ~i3TrapezoidalShadowMap(void);

	void	Create( INT32 w, INT32 h);

	UINT32		getState(void)						{ return m_State; }
	void		setState( UINT32 state)				{ m_State = state; }
	void		addState( UINT32 state)				{ m_State |= state; }
	void		removeState( UINT32 state)			{ m_State &= ~state; }

	UINT32		getStyle(void)						{ return m_Style; }
	void		setStyle( UINT32 style)				{ m_Style = style; }
	void		addStyle( UINT32 style)				{ m_Style |= style; }
	void		removeStyle( UINT32 style)			{ m_Style &= ~style; }

	i3Node *	getReceiver(void)					{ return m_pReceiver; }
	void		setReceiver( i3Node * pNode);

	i3Node *	getCaster(void)						{ return m_pCaster; }
	void		setCaster( i3Node * pNode);

	i3Light *	getLight(void)						{ return m_pLight; }
	void		setLight( i3Light * pLight, REAL32 fCutoff, REAL32 fNear, REAL32 fFar);

	MATRIX *	getLightTransform(void)				{ return &m_mLightTx; }
	void		SetLightTransform( MATRIX * pMtx);

	i3Texture *	getShadowMap(void)					{ return m_pShadowRTAttr->GetColorRenderTarget(); }

	COLORREAL *	getShadowColor(void)				{ return &m_ShadowColor; }
	void		setShadowColor( COLORREAL * pCol)	{ i3Color::Set( &m_ShadowColor, pCol); }
	void		setShadowColor( REAL32 r, REAL32 g, REAL32 b, REAL32 a)
	{
		i3Color::Set( &m_ShadowColor, r, g, b, a);
	}

	bool		getShadowEnable(void)				{ return (bool) (m_pShadowMapEnableAttr->Get() == TRUE); }
	void		setShadowEnable( bool bFlag)		{ m_pShadowMapEnableAttr->Set( bFlag); }

	void		setCameraNearFar( REAL32 n, REAL32 f)
	{
		m_fCamNear = n;
		m_fCamFar = f;
	}

	void	Apply( i3SceneTracer * pTracer);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

};

#endif
