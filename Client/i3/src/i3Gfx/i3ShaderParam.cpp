#include "i3GfxType.h"
#include "i3ShaderParam.h"
#include "i3GfxGlobalVariable.h"

#if defined( I3G_DX)
#include "i3TextureDX.h"
#include "i3GfxGlobalVariableDX.h"
#endif

struct _I3_RESERVED_SHADER_PARAM
{
	char						m_szName[64];
	i3ShaderParam::SEMANTIC		m_Semantic;
	UINT32						m_UpdateMask;
} ;

static _I3_RESERVED_SHADER_PARAM s_ReservedShaderParamTable[] =
{
	{ "---",				i3ShaderParam::SEMANTIC_CONSTANT				,	I3G_SHADER_PMASK_ALL											},
	{ "g_mWorld",			i3ShaderParam::SEMANTIC_MATRIX_WORLD			,	I3G_SHADER_PMASK_MATRIX_WORLD									},
	{ "g_mBone",			i3ShaderParam::SEMANTIC_MATRIX_BONE				,	I3G_SHADER_PMASK_MATRIX_BONE									},
	{ "g_mView",			i3ShaderParam::SEMANTIC_MATRIX_VIEW				,	I3G_SHADER_PMASK_MATRIX_VIEW									},
	{ "g_mInvView",			i3ShaderParam::SEMANTIC_MATRIX_INV_VIEW			,	I3G_SHADER_PMASK_MATRIX_VIEW									},
	{ "g_mWorldView",		i3ShaderParam::SEMANTIC_MATRIX_WORLDVIEW		,	I3G_SHADER_PMASK_MATRIX_WORLD | I3G_SHADER_PMASK_MATRIX_VIEW	},
	{ "g_mViewProj",		i3ShaderParam::SEMANTIC_MATRIX_VIEWPROJECT		,	I3G_SHADER_PMASK_MATRIX_VIEW | I3G_SHADER_PMASK_MATRIX_PROJECT	},
	{ "g_mProj",			i3ShaderParam::SEMANTIC_MATRIX_PROJECT			,	I3G_SHADER_PMASK_MATRIX_PROJECT									},
	{ "g_mWVP",				i3ShaderParam::SEMANTIC_MATRIX_FULL				,	I3G_SHADER_PMASK_MATRIX_WORLD | I3G_SHADER_PMASK_MATRIX_VIEW | I3G_SHADER_PMASK_MATRIX_PROJECT},
	{ "g_mTex",				i3ShaderParam::SEMANTIC_MATRIX_TEX				,	I3G_SHADER_PMASK_MATRIX_TEX										},


	{ "g_vAmbient",			i3ShaderParam::SEMANTIC_AMBIENT					,	I3G_SHADER_PMASK_AMBIENT										},
	{ "g_vColor",			i3ShaderParam::SEMANTIC_COLOR					,	I3G_SHADER_PMASK_COLOR											},

	{ "g_vLightDir0",		i3ShaderParam::SEMANTIC_LIGHT_DIRECTION_0		,	I3G_SHADER_PMASK_LIGHT0											},
	{ "g_vLightPos0",		i3ShaderParam::SEMANTIC_LIGHT_POSITION_0		,	I3G_SHADER_PMASK_LIGHT0											},
	{ "g_vLightDiffuse0",	i3ShaderParam::SEMANTIC_LIGHT_DIFFUSE_0			,	I3G_SHADER_PMASK_LIGHT0	| I3G_SHADER_PMASK_MATERIAL				},
	{ "g_vLightSpecular0",	i3ShaderParam::SEMANTIC_LIGHT_SPECULAR_0		,	I3G_SHADER_PMASK_LIGHT0	| I3G_SHADER_PMASK_MATERIAL				},
	{ "g_vLightMap0",		i3ShaderParam::SEMANTIC_LIGHT_MAP_0				,	I3G_SHADER_PMASK_LIGHT0											},
	{ "g_vLightFactor0",	i3ShaderParam::SEMANTIC_LIGHT_FACTOR_0			,	I3G_SHADER_PMASK_LIGHT0											},

	{ "g_mShadowMap0",		i3ShaderParam::SEMANTIC_SHADOWMAP_MATRIX_0		,	I3G_SHADER_PMASK_LIGHT0											},
	{ "g_vShadowMapFactor0",i3ShaderParam::SEMANTIC_SHADOWMAP_FACTOR_0		,	I3G_SHADER_PMASK_LIGHT0											},
	{ "g_texShadow0",		i3ShaderParam::SEMANTIC_SHADOWMAP_0				,	I3G_SHADER_PMASK_LIGHT0											},

	{ "g_vLightDir1",		i3ShaderParam::SEMANTIC_LIGHT_DIRECTION_1		,	I3G_SHADER_PMASK_LIGHT1											},
	{ "g_vLightPos1",		i3ShaderParam::SEMANTIC_LIGHT_POSITION_1		,	I3G_SHADER_PMASK_LIGHT1											},
	{ "g_vLightDiffuse1",	i3ShaderParam::SEMANTIC_LIGHT_DIFFUSE_1			,	I3G_SHADER_PMASK_LIGHT1	| I3G_SHADER_PMASK_MATERIAL				},
	{ "g_vLightSpecular1",	i3ShaderParam::SEMANTIC_LIGHT_SPECULAR_1		,	I3G_SHADER_PMASK_LIGHT1	| I3G_SHADER_PMASK_MATERIAL				},
	{ "g_vLightMap1",		i3ShaderParam::SEMANTIC_LIGHT_MAP_1				,	I3G_SHADER_PMASK_LIGHT1											},
	{ "g_vLightFactor1",	i3ShaderParam::SEMANTIC_LIGHT_FACTOR_1			,	I3G_SHADER_PMASK_LIGHT1											},

	{ "g_mShadowMap1",		i3ShaderParam::SEMANTIC_SHADOWMAP_MATRIX_1		,	I3G_SHADER_PMASK_LIGHT1											},
	{ "g_vShadowMapFactor1",i3ShaderParam::SEMANTIC_SHADOWMAP_FACTOR_1		,	I3G_SHADER_PMASK_LIGHT1											},
	{ "g_texShadow1",		i3ShaderParam::SEMANTIC_SHADOWMAP_1				,	I3G_SHADER_PMASK_LIGHT1											},

	{ "g_vMtlEmissive",		i3ShaderParam::SEMANTIC_MATERIAL_EMISSIVE		,	I3G_SHADER_PMASK_MATERIAL										},

	{ "g_BoneCount",		i3ShaderParam::SEMANTIC_BONE_COUNT				,	I3G_SHADER_PMASK_MATRIX_BONE									},

	{ "g_vCamPos",			i3ShaderParam::SEMANTIC_CAMERA_POSITION			,	I3G_SHADER_PMASK_MATRIX_VIEW									},
	{ "g_vCamDir",			i3ShaderParam::SEMANTIC_CAMERA_DIRECTION		,	I3G_SHADER_PMASK_MATRIX_VIEW									},

	{ "g_texDiffuse",		i3ShaderParam::SEMANTIC_DIFFUSEMAP				,	I3G_SHADER_PMASK_TEX_DIFFUSE									},
	{ "g_texSpecular",		i3ShaderParam::SEMANTIC_SPECULARMAP				,	I3G_SHADER_PMASK_TEX_SPECULAR									},
	{ "g_texNormal",		i3ShaderParam::SEMANTIC_NORMALMAP				,	I3G_SHADER_PMASK_TEX_NORMAL										},
	{ "g_texEmissive",		i3ShaderParam::SEMANTIC_EMISSIVEMAP				,	I3G_SHADER_PMASK_TEX_EMISSIVE									},
	{ "g_texReflect",		i3ShaderParam::SEMANTIC_REFLECTMAP				,	I3G_SHADER_PMASK_TEX_REFLECT									},
	{ "g_texReflectMask",	i3ShaderParam::SEMANTIC_REFLECTMASKMAP			,	I3G_SHADER_PMASK_TEX_REFLECT_MASK								},
	{ "g_texLux",			i3ShaderParam::SEMANTIC_LUXMAP					,	I3G_SHADER_PMASK_TEX_LUX										},

	{ "g_FresnelIOR",		i3ShaderParam::SEMANTIC_FRESNEL_IOR				,	I3G_SHADER_PMASK_FRESNEL										},
	{ "g_FresnelColor",		i3ShaderParam::SEMANTIC_FRESNEL_COLOR			,	I3G_SHADER_PMASK_FRESNEL										},

	{ "g_texLightProbe",	i3ShaderParam::SEMANTIC_LIGHT_PROBE				,	I3G_SHADER_PMASK_ALL											},

	{ "g_SecondaryColor",	i3ShaderParam::SEMANTIC_SECONDARY_COLOR			,	I3G_SHADER_PMASK_SECONDARY_COLOR								},
	{ "g_TetraColor",		i3ShaderParam::SEMANTIC_TETRA_COLOR				,	I3G_SHADER_PMASK_TETRA_COLOR									},
	{ "g_Time",				i3ShaderParam::SEMANTIC_TIME					,	I3G_SHADER_PMASK_ALL											},
	{ "g_mInvWorld",		i3ShaderParam::SEMANTIC_MATRIX_INV_WORLD		,	I3G_SHADER_PMASK_MATRIX_WORLD									},

	{ "g_ScreenSize",		i3ShaderParam::SEMANTIC_SCREENSIZE				,	I3G_SHADER_PMASK_ALL											},
	{ "g_XrayColor",		i3ShaderParam::SEMANTIC_XRAY_COLOR				,	I3G_SHADER_PMASK_XRAYCOLOR										},

	{ "",					i3ShaderParam::SEMANTIC_CONSTANT				,	I3G_SHADER_PMASK_ALL											},
};																				


I3_CLASS_INSTANCE( i3ShaderParam);

i3ShaderParam::i3ShaderParam(void) : m_UpdateMask(0)
{
	for( INT32 i = 0; i < i3ShaderParam::SEMANTIC_NUMS; i++)
	{
		if( s_ReservedShaderParamTable[i].m_Semantic != (i3ShaderParam::SEMANTIC) i)
		{
			I3TRACE( "Invalid Shader parameter table : %s\n", s_ReservedShaderParamTable[i].m_szName);
			I3ASSERT_0;
		}
	}

	#if defined( I3_DEBUG)
	i3ElementBase::setMemTag( -1);
	#endif

#if defined( I3G_DX)
	i3mem::FillZero( &m_Desc, sizeof( m_Desc));
#endif
}

i3ShaderParam::~i3ShaderParam(void)
{
}


void i3ShaderParam::setSemantic( SEMANTIC sem)
{
	m_Semantic = sem;

	m_UpdateMask = s_ReservedShaderParamTable[ sem].m_UpdateMask;
}

#if defined( I3G_DX)
__forceinline 
void i3ShaderParam::_UpdateFloat( REAL32 * pVal, INT32 RegIdx, INT32 RegCnt)
{
	HRESULT hRv;

	if( getTarget() == TARGET_VERTEXSHADER)
	{
		hRv = g_pD3DDevice->SetVertexShaderConstantF( RegIdx, pVal, RegCnt);
	}
	else
	{
		hRv = g_pD3DDevice->SetPixelShaderConstantF( RegIdx, pVal, RegCnt);
	}

	#if defined( I3_DEBUG)
		if( FAILED( hRv))
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not update shader parameter.");
			return;
		}
	#endif
}

__forceinline
void i3ShaderParam::_UpdateInt( INT32 * pVal, INT32 RegIdx, INT32 RegCnt)
{
	HRESULT hRv;

	if( getTarget() == TARGET_VERTEXSHADER)
	{
		hRv = g_pD3DDevice->SetVertexShaderConstantI( RegIdx, pVal, RegCnt);

	}
	else
	{
		hRv = g_pD3DDevice->SetPixelShaderConstantI( RegIdx, pVal, RegCnt);
	}

	if( FAILED( hRv))
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not update shader parameter.");
		return;
	}
}

__forceinline
void i3ShaderParam::_UpdateBOOL( BOOL * pVal, INT32 RegIdx, INT32 RegCnt)
{
	HRESULT hRv;

	if( getTarget() == TARGET_VERTEXSHADER)
	{
		hRv = g_pD3DDevice->SetVertexShaderConstantB( RegIdx, pVal, RegCnt);

	}
	else
	{
		hRv = g_pD3DDevice->SetPixelShaderConstantB( RegIdx, pVal, RegCnt);
	}

	if( FAILED( hRv))
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not update shader parameter.");
		return;
	}
}

bool i3ShaderParam::Init( ID3DXConstantTable * pTable)
{
	D3DXHANDLE hParam;
	HRESULT hRv;
	UINT cnt = 1;

	hParam = pTable->GetConstantByName( nullptr, GetName());

	if( hParam == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "Could not found %s shader parameter.", GetName());
		return false;
	}

	setHandle( (void *) hParam);

	hRv = pTable->GetConstantDesc( hParam, &m_Desc, &cnt);
	if( FAILED( hRv))
	{
		DXTRACE_ERR(L"Could not retreive description information about the constant.", hRv);
		I3PRINTLOG(I3LOG_FATAL,  "Could not retreive description information about the constant.");
		return false;
	}

	if( m_Desc.RegisterSet == D3DXRS_SAMPLER)
	{
		m_Index = pTable->GetSamplerIndex( hParam);
	}

	return true;
}

#endif

void i3ShaderParam::Update( MATRIX * pMatrix)
{
#if defined( I3G_DX)
	MATRIX mtx;

	i3Matrix::Transpose( &mtx, pMatrix);

	_UpdateFloat( (REAL32 *) &mtx, m_Desc.RegisterIndex, m_Desc.RegisterCount);
#endif
}

static	VEC4D	s_Temp[256 * 4];

void i3ShaderParam::Update( MATRIX * pMatrix, INT32 count)
{
#if defined( I3G_DX)
	INT32 i, idx, idxTemp = 0;

	idx = m_Desc.RegisterIndex;

	for( i = 0; i < count; i++, pMatrix++)
	{
		i3Matrix::Transpose( (MATRIX *) &s_Temp[idxTemp], pMatrix);

		idxTemp += m_Desc.Columns;
	}

	{
		_UpdateFloat( (REAL32 *) s_Temp, idx, idxTemp);
	}

#endif
}

void i3ShaderParam::Update( REAL32 val)
{
#if defined( I3G_DX)
	_UpdateFloat( (REAL32 *) &val, m_Desc.RegisterIndex, m_Desc.RegisterCount);
#endif
}

void i3ShaderParam::Update( INT32 val)
{
#if defined( I3G_DX)
	_UpdateInt( (INT32 *) &val, m_Desc.RegisterIndex, m_Desc.RegisterCount);
#endif
}

void i3ShaderParam::Update( bool val)
{
#if defined( I3G_DX)
	BOOL	boolean = (val == true);

	_UpdateBOOL( (BOOL *) &boolean, m_Desc.RegisterIndex, m_Desc.RegisterCount);
#endif
}

void i3ShaderParam::Update( VEC2D * pVec)
{
#if defined( I3G_DX)
	// VEC2D의 Array를 이 Interface로 전달해선 안된다.
	// 그것은 Shader Constant Register들이 모두 VEC4D 형식으로 되어 있으며
	// 만약 Array를 전달하려는 경우라면, 실제로 VEC4D의 Array이어야
	// 올바른 값이 전달될 수 있기 때문이다.
	I3ASSERT( m_Desc.RegisterCount == 1);

	_UpdateFloat( (REAL32 *) pVec, m_Desc.RegisterIndex, m_Desc.RegisterCount);
#endif
}

void i3ShaderParam::Update( VEC3D * pVec)
{
#if defined( I3G_DX)
	// VEC2D의 Array를 이 Interface로 전달해선 안된다.
	// 그것은 Shader Constant Register들이 모두 VEC4D 형식으로 되어 있으며
	// 만약 Array를 전달하려는 경우라면, 실제로 VEC4D의 Array이어야
	// 올바른 값이 전달될 수 있기 때문이다.
	I3ASSERT( m_Desc.RegisterCount == 1);

	_UpdateFloat( (REAL32 *) pVec, m_Desc.RegisterIndex, m_Desc.RegisterCount);
#endif
}

void i3ShaderParam::Update( VEC4D * pVec)
{
#if defined( I3G_DX)
	_UpdateFloat( (REAL32 *) pVec, m_Desc.RegisterIndex, m_Desc.RegisterCount);
#endif
}

void i3ShaderParam::Update( COLORREAL * pColor)
{
#if defined( I3G_DX)
	_UpdateFloat( (REAL32 *) pColor, m_Desc.RegisterIndex, m_Desc.RegisterCount);
#endif
}

void i3ShaderParam::Update( i3Texture * pTex, bool bProjective)
{
#if defined( I3G_DX)
	HRESULT hRv;
	i3TextureDX * pTexDX = (i3TextureDX *) pTex;

	if( getHandle() == nullptr)
		return;

	hRv = g_pD3DDevice->SetTexture( getIndex(), pTexDX->GetDXImage());
	if( FAILED( hRv))
	{
		DXTRACE_ERR(L"SetTexture() failed.", hRv);
		I3PRINTLOG(I3LOG_WARN,  "Could not set sampler shader param. %d", getIndex());
	}

	g_pD3DDevice->SetSamplerState( getIndex(), D3DSAMP_BORDERCOLOR, 0xFF000000);
	g_pD3DDevice->SetSamplerState( getIndex(), D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	g_pD3DDevice->SetSamplerState( getIndex(), D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);

	/*
	if( bProjective)
	{
		g_pD3DDevice->SetTextureStageState( getIndex(), D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 | D3DTTFF_PROJECTED);
	}
	else
	{
		g_pD3DDevice->SetTextureStageState( getIndex(), D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	}
	*/
#endif
}

#if defined( I3G_DX)

//#define I3L_VSM
//#define I3L_ESM

void i3ShaderParam::Update( I3G_TEXTURE_BIND_CONTEXT * pTexCtx)
{
	HRESULT hRv;
	i3TextureDX * pTexDX = (i3TextureDX *) pTexCtx->m_pTexture;

	if( getHandle() == nullptr)
		return;

	// 동일한 Sampler Stage에 이전에 다른 Shader가 어떻게 값을 설정했는지를
	// Tracking하는 시스템이 없기 때문에, 그 값이 유지되고 있다고 가정하고 있는
	// m_ModiftFlag이 올바르게 동작할 수 없다.
	// 이런 이유로 아래의 Modify Flag 검사 코드를 제거한다. (이진균)
	//if( pTexCtx->m_ModifyFlag & I3G_TEXTURE_MODIFY_TEXTURE)
	{
		if( pTexDX != nullptr)
		{
			hRv = g_pD3DDevice->SetTexture( getIndex(), pTexDX->GetDXImage());
		}
		else
		{
			hRv = g_pD3DDevice->SetTexture( getIndex(), NULL);
		}

		if( FAILED( hRv))
		{
			DXTRACE_ERR(L"SetTexture() failed.", hRv);
			I3PRINTLOG(I3LOG_WARN,  "Could not set sampler shader param. %d", getIndex());
		}
	}

	 //if( pTexCtx->m_ModifyFlag & I3G_TEXTURE_MODIFY_WRAP)
	{
		g_pRenderContext->_SetTextureWrap( getIndex(), pTexCtx->m_HWrap, pTexCtx->m_VWrap);
	}

	//if( pTexCtx->m_ModifyFlag & I3G_TEXTURE_MODIFY_BORDER_COLOR)
	{
		g_pRenderContext->_SetTextureBorderColor( getIndex(), pTexCtx->m_BorderColor);
	}

	//if( pTexCtx->m_ModifyFlag & I3G_TEXTURE_MODIFY_FILTER)
	{
		g_pRenderContext->_SetTextureFilter( getIndex(), pTexCtx->m_MagFilter, pTexCtx->m_MinFilter);
	}

	if( pTexCtx->m_BindType == I3G_TEXTURE_BIND_SHADOW)
	{
		#if defined( I3L_VSM) || defined( I3L_ESM)
			g_pRenderContext->_SetTextureFilter( getIndex(), I3G_TEXTURE_FILTER_LINEAR, I3G_TEXTURE_FILTER_ANISOTROPIC);
			//g_pRenderContext->_SetTextureFilter( getIndex(), I3G_TEXTURE_FILTER_POINT, I3G_TEXTURE_FILTER_POINT);
		#else
			g_pRenderContext->_SetTextureFilter( getIndex(), I3G_TEXTURE_FILTER_POINT, I3G_TEXTURE_FILTER_POINT);
		#endif
	}

	pTexCtx->m_ModifyFlag &= ~(I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER | I3G_TEXTURE_MODIFY_BORDER_COLOR);
}
#endif

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)				// [test required] 패킹 사이즈 누락으로 추가합니다. 2017.02.23. soon9
#endif

namespace pack
{
	struct ALIGN8 SHADER_PARAM
	{
		UINT8			m_Semantic = 0;
		UINT8			m_Target = 0;
		UINT8			m_Index = 0;
		UINT8			pad[3] = { 0 };
	};
}

#if defined(I3_COMPILER_VC) && defined(PACHING_RIGHT)
#pragma pack(pop)
#endif

UINT32 i3ShaderParam::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SHADER_PARAM data;

	Rc = i3NamedElement::OnSave( pResFile);
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "");
		return STREAM_ERR;
	}
	Result += Rc;

	data.m_Semantic = (UINT8)	getSemantic();
	data.m_Target = (UINT8)		getTarget();
	data.m_Index = (UINT8)		getIndex();

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write shader parameter data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3ShaderParam::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SHADER_PARAM data;

	Rc = i3NamedElement::OnLoad( pResFile);
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "");
		return STREAM_ERR;
	}
	Result += Rc;

	Rc = pStream->Read( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not read shader parameter data");
		return STREAM_ERR;
	}
	Result += Rc;

	setSemantic( (SEMANTIC) data.m_Semantic);
	setTarget( (TARGET) data.m_Target);
	setIndex( (INT32) data.m_Index);

	return Result;
}


i3ShaderParam::SEMANTIC i3ShaderParam::GetSemanticByVarName( const char * pszName)
{
	INT32 i = 0;

	while( s_ReservedShaderParamTable[i].m_szName[0] != 0)
	{
		if( i3::generic_is_iequal( s_ReservedShaderParamTable[i].m_szName, pszName) )
			return s_ReservedShaderParamTable[i].m_Semantic;

		i++;
	}

	return SEMANTIC_CONSTANT;
}