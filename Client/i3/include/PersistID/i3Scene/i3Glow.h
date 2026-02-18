#ifndef __I3GLOW_H__
#define __I3GLOW_H__

#if defined (I3G_DX)

#include "i3Node.h"
#include "i3AllAttrs.h"

#define NUM_TONEMAP_TEXTURES	5

class I3_EXPORT_SCENE i3Glow :	public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Glow);

protected:
	BOOL						m_bEnableGlow;
	BOOL						m_bEnableToneMap;
	INT32						m_nRTWidth;
	INT32						m_nRTHeight;	
	BOOL						m_bDebug;

	VEC4D						m_GlowShaderConstant;

	////////////////////////////////////////////////////////////////////
	// Shader Attrs
	//
	// Glow용 Shader Attr
	i3ShaderAttr *				m_pDownscaleShaderAttr;
	i3ShaderAttr *				m_BlurShaderAttr_AxisX;
	i3ShaderAttr *				m_BlurShaderAttr_AxisY;
	i3ShaderAttr *				m_pAddTwoTexShaderAttr;
	i3ShaderAttr *				m_pNullShaderAttr;

	// Tone Map용 Shader Attr
	i3ShaderAttr *				m_Lum2x2ShaderAttr;
	i3ShaderAttr *				m_ToneDownScaleShaderAttr;
	i3ShaderAttr *				m_AdaptationShaderAttr;
	i3ShaderAttr *				m_ToneMapShaderAttr;
	i3ShaderAttr *				m_ToneMapWithGlowShaderAttr;
	
	// Debug용 Shader Attr
	i3ShaderAttr *				m_DrawTexShaderAttr;
	//
	//
	////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////
	// Render Target
	//
	// Glow용 Render Target
	i3RenderTargetAttr *		m_pGlowRTAttr;
	i3RenderTargetAttr *		m_pScaleRTAttr;
	i3RenderTargetAttr *		m_pGaussBlurRTAttr_AxisX;
	i3RenderTargetAttr *		m_pGaussBlurRTAttr_AxisY;
	i3RenderTargetAttr *		m_pRestoreRTAttr;
	i3ClearRenderTargetAttr *	m_pClearRTAttr;

	// Tone Map용 Render Target
	i3RenderTargetAttr *		m_pToneMapRTAttr[NUM_TONEMAP_TEXTURES];
	i3RenderTargetAttr *		m_pAdaptationRTAttr;
	i3RenderTargetAttr *		m_pLuminanceRTAttr;
	i3RenderTargetAttr *		m_pLastLuminanceRTAttr;
    //
	//
	////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////
	// Geometry Attr 
	//
	// 실제 화면을 구성하는 Plan입니다.(화면전체사이즈)
	i3GeometryAttr *			m_pRectGeoAttr;

	// Debug용 Plan입니다.
	i3GeometryAttr *			m_pRectGeoDebugAttr;
	//
	//
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// Shader Constant Param
	//
	//i3ShaderParamVector4Ref *	m_pGlowConstant;	
	i3ShaderParamSamplerStates * m_pCurSamplerStates;
	i3ShaderParamSamplerStates * m_pPrevSamplerStates;
	i3ShaderParamSamplerStates * m_pToneDownSamplerStates;	
	i3ShaderParamSamplerStates * m_pDrawSamplerStates;
	//
	//
	////////////////////////////////////////////////////////////////////

public:
	i3Glow(void);
	virtual ~i3Glow(void);

	void			Create( INT32 w, INT32 h);
	void			Apply( i3SceneTracer * pTracer);

	// Glow용
	bool			CreateDownscaleShader(void);
	bool			CreateGaussBlurShader_AxisX(void);
	bool			CreateGaussBlurShader_AxisY(void);
	bool			CreateAddTwoTexShader(void);	

	// Tone Map용
	bool			CreateLum2x2();
	bool			CreateToneDownScale();
	bool			CreateAdaptation();
	bool			CreateToneMap();
	bool			CreateToneMapWithGlow();
	BOOL			MeasureLuminance( i3SceneTracer * pTracer);

	// Debug용
	bool			CreateDrawTexShader(void);

	//
	void			SetBlurDispersion( REAL32 fVal)			{ i3Vector::SetX( &m_GlowShaderConstant, fVal);		}
	REAL32			GetBlurDispersion()						{ return i3Vector::GetX( &m_GlowShaderConstant);	}

	void			SetToneMapScale( REAL32 fVal)			{ i3Vector::SetY( &m_GlowShaderConstant, fVal);		}
	REAL32			GetToneMapScale()						{ return i3Vector::GetY( &m_GlowShaderConstant);	}

	//
	void			SetDebug(BOOL bFlag)					{ m_bDebug = bFlag;			}	
	void			SetGlowEnable(BOOL bFlag)				{ m_bEnableGlow = bFlag;	}
	BOOL			GetGlowEnable()							{ return m_bEnableGlow;		}
	void			SetToneMapEnable(BOOL bFlag)			{ m_bEnableToneMap = bFlag;	}
	BOOL			GetToneMapEnable()						{ return m_bEnableToneMap;	}

	//
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif

#endif	// __I3GLOW_H__
