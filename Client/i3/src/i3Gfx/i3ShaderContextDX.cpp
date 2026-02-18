#include "i3GfxType.h"
#include "i3ShaderContext.h"
#include "i3RenderContext.h"
#include "i3ShaderGen.h"

//#include "i3Common.hlsl.cpp"
//#include "i3ShadowMap.hlsl.cpp"
//#include "i3DirectionalLight.hlsl.cpp"
//#include "i3LuxMap.hlsl.cpp"
//#include "i3Phong_LightModel.hlsl.cpp"
//#include "i3HSL_LightModel.hlsl.cpp"
//#include "i3GI1_LightModel.hlsl.cpp"
//#include "i3Custom_LightModel.hlsl.cpp"
//#include "i3PointLight.hlsl.cpp"
//#include "i3SpotLight.hlsl.cpp"
//#include "i3Reflection.hlsl.cpp"
//#include "i3VertexShader_PPL.hlsl.cpp"
//#include "i3VertexShader_PVL.hlsl.cpp"
//#include "i3PixelShader_PPL.hlsl.cpp"
//#include "i3PixelShader_PVL.hlsl.cpp"
//#include "i3SuperShader.hlsl.cpp"

#include "i3ShaderSourceCodeMgr.h"

//#if !defined( I3_DEBUG)
//#define I3G_BINARY_SHADER
//#endif

//#define I3G_BINARY_SHADER

#if defined( I3G_DX)
void i3ShaderContext::_ReleaseShaders(void)
{
	INT32 i;

	for( i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		if( m_pShaderBin[i] != nullptr)
		{
			if( i != I3G_SHADER_TYPE_PIXEL)
			{
				IDirect3DVertexShader9 * pVS = (IDirect3DVertexShader9 *) m_pShaderBin[i];

				pVS->Release();
			}
			else
			{
				IDirect3DPixelShader9 * pPS = (IDirect3DPixelShader9 *) m_pShaderBin[i];

				pPS->Release();
			}
		}

		m_pShaderBin[i] = nullptr;
	}
}

bool i3ShaderContext::_SyncParams( I3G_SHADER_TYPE type, DWORD * pShaderData, ID3DXConstantTable * pTable)
{
	i3ShaderParam * pParam;
	UINT32 i, cnt;
	D3DXCONSTANTTABLE_DESC desc;
	D3DXCONSTANT_DESC constDesc;
	D3DXHANDLE hConst;
	i3ShaderParam::SEMANTIC sem;
	bool bRv = true;

	// Constant Table을 검색하여 기본 Param들을 추가한다.
	{
		pTable->GetDesc( &desc);

		for( i = 0; i < desc.Constants; i++)
		{
			hConst = pTable->GetConstant( nullptr, i);

			cnt = 1;
			pTable->GetConstantDesc( hConst, &constDesc, &cnt);

			if( FindParam( type,constDesc.Name) == nullptr)
			{
				sem = i3ShaderParam::GetSemanticByVarName( constDesc.Name);

				pParam = i3ShaderParam::new_object_ref();

				pParam->SetName( constDesc.Name);
				pParam->setSemantic( sem);
				
				
				AddParam( type, pParam);

				if( sem == i3ShaderParam::SEMANTIC_CONSTANT)
				{
					I3TRACE( "SHADER [%s] : %s parameters's semantic is constant.\n", m_pShader->GetName(), constDesc.Name);
				}
			}
		}
	}

	for( i = 0; i < getParamCount( type); i++)
	{
		pParam = getParam( type, i);

		bRv = bRv && pParam->Init( pTable);
	}

	return bRv;
}

static char s_szProfile[6][16];

INT32 i3ShaderContext::_MakeVSProfile( I3G_SHADER_TYPE type, INT32 * pStartIndex)
{
	INT32 idx = 0;
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	*pStartIndex = 0;


	// Shader Model 1
	{
		if( m_pShader->getRequestedVSVersion() == I3G_SHADER_VERSION_1)
			*pStartIndex = 0;

		sprintf( s_szProfile[idx], "vs_1_1");
		idx++;
	}

	// Shader Model 2
	if( pCaps->getVertexShaderVersionMajor() >= 2)
	{
		if( m_pShader->getRequestedVSVersion() == I3G_SHADER_VERSION_2)
			*pStartIndex = idx;

		if( m_pShader->getTargetGPU() == I3G_GPU_VENDOR_NA)
		{
			m_pShader->QueryTargetGPU();
		}

		switch( m_pShader->getTargetGPU())
		{
			case I3G_GPU_VENDOR_NVIDIA :		sprintf( s_szProfile[idx], "vs_2_a");	break;
			default :							sprintf( s_szProfile[idx], "vs_2_0");	break;
		}
		idx++;
	}

	// Shader Model 3
	if( pCaps->getVertexShaderVersionMajor() >= 3)
	{
		if( m_pShader->getRequestedVSVersion() == I3G_SHADER_VERSION_3)
			*pStartIndex = idx;

		sprintf( s_szProfile[idx], "vs_3_0");
		idx++;
	}

	return idx;
}

INT32 i3ShaderContext::_MakePSProfile( INT32 * pStartIndex)
{
	INT32 idx = 0, start;
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	*pStartIndex = 0;

	// Shader Model 1
	
	{
		if( m_pShader->getRequestedPSVersion() == I3G_SHADER_VERSION_1)
			*pStartIndex = idx;

		if( pCaps->getPixelShaderVersionMajor() == 1)
			start = pCaps->getPixelShaderVersionMinor();
		else
			start = 4;

		INT32 i;
		for( i = 1; i <= start; ++i)
		{
			sprintf( s_szProfile[idx], "ps_1_%d", i);
			idx++;
		}
	}
	

	// Shader Model 2
	if( pCaps->getPixelShaderVersionMajor() >= 2)
	{
		if( m_pShader->getRequestedPSVersion() == I3G_SHADER_VERSION_2)
			*pStartIndex = idx;

		if( m_pShader->getTargetGPU() == I3G_GPU_VENDOR_NA)
		{
			m_pShader->QueryTargetGPU();
		}

		switch( m_pShader->getTargetGPU())
		{
			case I3G_GPU_VENDOR_NVIDIA :		sprintf( s_szProfile[idx], "ps_2_a");	break;
			case I3G_GPU_VENDOR_ATI :			sprintf( s_szProfile[idx], "ps_2_b");	break;
			default :							sprintf( s_szProfile[idx], "ps_2_0");	break;
		}
		idx++;
	}

	// Shader Model 3
	if( pCaps->getPixelShaderVersionMajor() >= 3)
	{
		if( m_pShader->getRequestedPSVersion() == I3G_SHADER_VERSION_3)
			*pStartIndex = idx;

		sprintf( s_szProfile[idx], "ps_3_0");
		idx++;
	}

	return idx;
}

// D3DXSHADER_OPTIMIZATION_LEVEL3
//#define			SHADER_COMPILE_FLAG		(D3DXSHADER_PACKMATRIX_COLUMNMAJOR | D3DXSHADER_USE_LEGACY_D3DX9_31_DLL)
#define			SHADER_COMPILE_FLAG		(D3DXSHADER_PACKMATRIX_COLUMNMAJOR)

bool i3ShaderContext::_Compile( I3G_SHADER_TYPE type, const char * pszWorkPath, const char * pszFileName, INT32 numBones)
{
	LPD3DXBUFFER pInst = nullptr, pErrorMsg = nullptr;
	ID3DXConstantTable * pConstTable = nullptr;
	HRESULT hRv = S_OK;
	UINT32 flags, i;
	INT32 cnt, idxStart;
	char szPath[ MAX_PATH];
	D3DXMACRO Macro[3], * pDef = nullptr;
	char szProfile[32], szNumBones[64];
	UINT16	major = 0, minor = 0;

	::memset(szProfile, 0, sizeof(szProfile));

	// Profile String을 생성한다.
	// 이는 다양한 그래픽 카드의 제조사 및 Shader Model에 맞추기 위함.

	switch( type)
	{
		case I3G_SHADER_TYPE_VERTEX :				
			cnt = _MakeVSProfile( type, &idxStart);

			idxStart = m_pShader->getRequestedVSVersion() - I3G_SHADER_VERSION_1;
			break;

		case I3G_SHADER_TYPE_PIXEL :
			cnt = _MakePSProfile( &idxStart);
			break;

		//case I3G_SHADER_TYPE_BLENDED_VERTEX :
		default:
			cnt = _MakeVSProfile( type, &idxStart);

			Macro[0].Name = "I3L_VERTEX_BLEND";
			Macro[0].Definition = nullptr;

			sprintf( szNumBones, "%d",	numBones);
			Macro[1].Name = "NUM_BONES";
			Macro[1].Definition = szNumBones;

			Macro[2].Name = nullptr;
			Macro[2].Definition = nullptr;

			pDef = Macro;
			break;
	}

	flags = D3DXSHADER_PACKMATRIX_COLUMNMAJOR;

	#if defined( I3_DEBUG_SHADER)
		flags |= D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
	#endif

	sprintf( szPath, "%s\\%s", pszWorkPath, pszFileName);

	for( i = idxStart; i < (UINT32) cnt; i++)
	{
		pConstTable = nullptr;
		pErrorMsg = nullptr;
		pInst = nullptr;

		if( s_szProfile[i][3] == '1')
			flags |= D3DXSHADER_USE_LEGACY_D3DX9_31_DLL;
		else
			flags &= ~D3DXSHADER_USE_LEGACY_D3DX9_31_DLL;

		switch( type)
		{
			default :
				hRv = ::D3DXCompileShaderFromFile( (LPCTSTR) szPath, pDef, NULL, "VS_Def",
									(LPCSTR) s_szProfile[ i], flags,
									&pInst, &pErrorMsg, &pConstTable);

				if( !FAILED( hRv))
					i3::safe_string_copy( szProfile, s_szProfile[i], 32);
				break;

			case I3G_SHADER_TYPE_PIXEL :
				hRv = ::D3DXCompileShaderFromFile( (LPCTSTR) szPath, NULL, NULL, "PS_Def",
									(LPCSTR) s_szProfile[ i], flags,
									&pInst, &pErrorMsg, &pConstTable);

				if( !FAILED( hRv))
					i3::safe_string_copy( szProfile, s_szProfile[i], 32);
				break;
		}
		/*I3PRINTLOG(I3LOG_WARN,  "-ShaderFromFile - SHADER_NAME : %s \n" , szPath);
		I3PRINTLOG(I3LOG_WARN,  "SHADER_TYPE : %d \n" , type);
		I3PRINTLOG(I3LOG_WARN,  "SHADER_TARGET : %s \n" , s_szProfile[ i]);*/

		if( FAILED( hRv))
		{
			if( pErrorMsg != nullptr)
			{
				SetErrorMsg( (char *) pErrorMsg->GetBufferPointer(), pErrorMsg->GetBufferSize());
				pErrorMsg->Release();
			}
		}
		else
		{
			break;
		}
	}

	if( FAILED( hRv))
	{
		ReportErrorMsg();
		addState( I3G_SHADER_CTX_STATE_ERROR);
		return false;
	}

	I3TRACE( "%s ", szProfile);

	major = szProfile[3];
	minor = szProfile[5];

	m_pShader->SetShaderData( type, (char *) pInst->GetBufferPointer(), pInst->GetBufferSize(), MAKEWORD( minor, major));

	pInst->Release();
	pConstTable->Release();

	return true;
}

bool i3ShaderContext::_CompileMem( I3G_SHADER_TYPE type, char * pszHLSL, UINT32 sz, INT32 numBones)
{
	LPD3DXBUFFER pInst = nullptr, pErrorMsg = nullptr;
	ID3DXConstantTable * pConstTable = nullptr;
	HRESULT hRv = S_OK;
	UINT32 flags, i, cnt;
	INT32 idxStart;
	D3DXMACRO Macro[4], *pDef = nullptr;
	char szProfile[32], szNumBones[64];

	switch( type)
	{
	case I3G_SHADER_TYPE_VERTEX:
		cnt = _MakeVSProfile(type, &idxStart);

		/*Macro[0].Name = "I3G_BINARY_SHADER";
		Macro[0].Definition = nullptr;

		Macro[1].Name = nullptr;
		Macro[1].Definition = nullptr;*/

		Macro[0].Name = nullptr;
		Macro[0].Definition = nullptr;
		break;

	case I3G_SHADER_TYPE_PIXEL:
		cnt = _MakePSProfile(&idxStart);

		/*Macro[0].Name = "I3G_BINARY_SHADER";
		Macro[0].Definition = nullptr;

		Macro[1].Name = nullptr;
		Macro[1].Definition = nullptr;*/

		Macro[0].Name = nullptr;
		Macro[0].Definition = nullptr;
		break;

		//case I3G_SHADER_TYPE_BLENDED_VERTEX :
	default:
		cnt = _MakeVSProfile(type, &idxStart);

		/*Macro[0].Name = "I3G_BINARY_SHADER";
		Macro[0].Definition = nullptr;*/

		Macro[0].Name = "I3L_VERTEX_BLEND";
		Macro[0].Definition = nullptr;

		sprintf(szNumBones, "%d", numBones);
		Macro[1].Name = "NUM_BONES";
		Macro[1].Definition = szNumBones;

		Macro[2].Name = nullptr;
		Macro[2].Definition = nullptr;
		break;
	}

	pDef = Macro;

	flags = SHADER_COMPILE_FLAG;

	#if defined( I3_DEBUG_SHADER)
		flags |= D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
	#endif

	for( i = idxStart; i < cnt; i++)
	{
		pConstTable = nullptr;
		pErrorMsg = nullptr;
		pInst = nullptr;

		switch( type)
		{
			default :
				/*hRv = ::D3DXCompileShader( pszHLSL, sz, pDef, NULL, "VS_Def",
				(LPCSTR) s_szProfile[ i], flags,
				&pInst, &pErrorMsg, &pConstTable);*/

				hRv = D3DXCompileShader(pszHLSL, sz, pDef, GetShaderSourceCodeMgr()->GetD3DXInclude(), "VS_Def",
					(LPCSTR)s_szProfile[i], flags,
					&pInst, &pErrorMsg, &pConstTable);
				break;

			case I3G_SHADER_TYPE_PIXEL :
				/*hRv = ::D3DXCompileShader( pszHLSL, sz, pDef, NULL, "PS_Def",
				(LPCSTR) s_szProfile[ i], flags,
				&pInst, &pErrorMsg, &pConstTable);*/

				hRv = D3DXCompileShader(pszHLSL, sz, pDef, GetShaderSourceCodeMgr()->GetD3DXInclude(), "PS_Def",
					(LPCSTR)s_szProfile[i], flags,
					&pInst, &pErrorMsg, &pConstTable);

				if (!FAILED(hRv))
					i3::safe_string_copy(szProfile, s_szProfile[i], 32);
				break;
		}

		if( ! FAILED( hRv))
		{
			break;
		}

		if( pErrorMsg != nullptr)
		{
			SetErrorMsg( (char *) pErrorMsg->GetBufferPointer(), pErrorMsg->GetBufferSize());
			pErrorMsg->Release();
		}
	}

	if( FAILED( hRv))
	{
		I3TRACE( "--------------------------------------------\n");
		I3TRACE( "%s\n", pszHLSL);
		I3TRACE( "--------------------------------------------\n");
		ReportErrorMsg();
		addState( I3G_SHADER_CTX_STATE_ERROR);
		return false;
	}

	I3TRACE( "%s ", szProfile);
	UINT16 major, minor;

	major = szProfile[3];
	minor = szProfile[5];

	m_pShader->SetShaderData( type, (char *) pInst->GetBufferPointer(), pInst->GetBufferSize(), MAKEWORD( minor, major));

	pInst->Release();
	pConstTable->Release();

	return true;
}

struct SG_TABLE
{
	I3G_SHADER_TYPE		m_Type;
	INT32				m_BoneCount;
};

static SG_TABLE s_ShaderGenTable[I3G_SHADER_TYPE_MAX] =
{
	{ I3G_SHADER_TYPE_VERTEX,				0},
	{ I3G_SHADER_TYPE_BLENDED_VERTEX_1,		1},
	{ I3G_SHADER_TYPE_BLENDED_VERTEX_2,		2},
	{ I3G_SHADER_TYPE_BLENDED_VERTEX_3,		3},
	{ I3G_SHADER_TYPE_BLENDED_VERTEX_4,		4},
	{ I3G_SHADER_TYPE_PIXEL,				0}
};

//typedef struct _ShaderBinTable
//{
//	UINT8 *				m_pData;
//	UINT32				m_Size;
//} SBIN_TABLE;
//
//static SBIN_TABLE s_ShaderBinTable[] =
//{
//	{	g_i3Common,				sizeof(g_i3Common)},
//	{	g_i3Reflection,			sizeof(g_i3Reflection) },
//	{	g_i3ShadowMap,			sizeof(g_i3ShadowMap) },
//	{	g_i3DirectionalLight,	sizeof(g_i3DirectionalLight)},
//	{	g_i3LuxMap,				sizeof(g_i3LuxMap)},
//	{	g_i3Phong_LightModel,	sizeof(g_i3Phong_LightModel)},
//	{	g_i3HSL_LightModel,		sizeof(g_i3HSL_LightModel) },
//	{	g_i3GI1_LightModel,		sizeof(g_i3GI1_LightModel) },
//	{	g_i3Custom_LightModel,	sizeof(g_i3Custom_LightModel) },
//	{	g_i3PointLight,			sizeof(g_i3PointLight)},
//	{	g_i3SpotLight,			sizeof(g_i3SpotLight)},
//	{	g_i3VertexShader_PPL,	sizeof(g_i3VertexShader_PPL) },
//	{	g_i3VertexShader_PVL,	sizeof(g_i3VertexShader_PVL) },
//	{	g_i3PixelShader_PPL,	sizeof(g_i3PixelShader_PPL) },
//	{	g_i3PixelShader_PVL,	sizeof(g_i3PixelShader_PVL) },
//	{	g_i3SuperShader,		sizeof(g_i3SuperShader)},
//
//	{	nullptr,					0	}
//};


bool i3ShaderContext::Compile( const char * pszWorkPath, const char * pszFileName)
{
	INT32 i;
	bool bResult = false;
	//bool bCustomShader = false;
	char * pszi3DeclCode = nullptr;
	UINT32 size, idx = 0;
	i3::string strHLSLCode;
	bool isUberShader = false;

	I3TRACE("SHADER GEN : %s - ", m_pShader->GetName());

	if (pszFileName == nullptr)
	{
		pszFileName = "i3SuperShader.hlsl";
		isUberShader = true;
	}
	/*else
	{
	bCustomShader = true;
	}*/

	bool bBuildFile = true;

#ifdef I3_DEBUG
	bBuildFile = true;	// HLSL 파일로 컴파일.
#else
	bBuildFile = false;	// 셰이더 메모리 코드로 컴파일(릴리즈 모드에서는 HLSL 파일 배포하지 않기 때문).
#endif

						//#if defined( I3G_BINARY_SHADER)
						//bBuildFile = bCustomShader;
						//#endif

	if (m_pShader->getTargetGPU() == I3G_GPU_VENDOR_NA)
	{
		// 만약 GPU에 대한 구체적인 지정이 없다면 현재 GPU의 Profile에 맞는
		// Shader를 생성하도록 한다.
		i3GfxCaps * pCaps = i3RenderContext::GetCaps();

		m_pShader->setTargetGPU(pCaps->getGPUVendor());
	}

	if (bBuildFile == false)
	{
		// HLSL Code를 생성하기 위한 Memory 준비

		if (isUberShader)
		{
			// 크기 계산
			{
				size = 4096;

				/*for( i = 0; s_ShaderBinTable[i].m_pData != nullptr; i++)
				{
				size += s_ShaderBinTable[i].m_Size;
				}*/
			}

			// Memory Allocation
			{
				pszi3DeclCode = (char *)i3MemAlloc(size + 1);
				I3ASSERT(pszi3DeclCode != nullptr);
			}

			// Shader Generate
			{
				i3MemStream strm;
				i3ShaderGen gen;

				strm.Open(pszi3DeclCode, STREAM_WRITE, 1024);

				if (gen.Generate(m_pShader, &strm) == false)
				{
					I3PRINTLOG(I3LOG_FATAL, "Could not generate shader code");
					goto ExitPart;
				}

				I3ASSERT(strm.GetPosition() <= 1024);
				idx = strm.GetPosition();
			}
		}

		// Decriptions
		{
			i3::string code = GetShaderSourceCodeMgr()->GetSourceCode(pszFileName);
			if (code.empty())
			{
				if (isUberShader && pszi3DeclCode != nullptr)
					i3MemFree(pszi3DeclCode);

				return false;
			}

			if (isUberShader)
				strHLSLCode.assign(pszi3DeclCode, pszi3DeclCode + idx); // i3Decl.hlsl 코드 삽입. Uber shader defines.

			strHLSLCode += code;

			//for( i = 0; s_ShaderBinTable[i].m_pData != nullptr; i++)
			//{
			//	UINT32 len = s_ShaderBinTable[i].m_Size;

			//	// Encript되어 있는 Binary Shader Code를 복사한다.
			//	i3mem::Copy( &pszHLSL[idx], s_ShaderBinTable[i].m_pData, len);

			//	BitRotateDecript( (UINT8 *) &pszHLSL[idx], len, 3);

			//	idx += len;
			//	I3ASSERT( idx < size);
			//}

			//pszHLSL[idx] = 0;		// Null-Terminated String

#if 0
			{
				i3FileStream file;

				file.Create("D:\\shader.hlsl", STREAM_WRITE);
				file.Write(pszHLSL, idx);
				file.Close();
			}
#endif
			}
		}
	else
	{
		// 파일에서 Shader Compile
		i3ShaderGen gen;

		gen.setWorkDir(pszWorkPath);
		if (gen.Generate(m_pShader) == false)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could not generate shader code\n");
			goto ExitPart;
		}
	}

	m_pShader->addState(I3G_SHADER_STATE_GENERATED);

	for (i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		if (bBuildFile == false)
		{
			//if( _CompileMem( s_ShaderGenTable[i].m_Type, pszHLSL, idx, s_ShaderGenTable[i].m_BoneCount) == false)
			if (_CompileMem(s_ShaderGenTable[i].m_Type, const_cast<char*>(strHLSLCode.c_str()), strHLSLCode.size(), s_ShaderGenTable[i].m_BoneCount) == false)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not compile %s shader\n", m_pShader->GetName());
				goto ExitPart;
			}
	}
		else
		{
			if (_Compile(s_ShaderGenTable[i].m_Type, pszWorkPath, pszFileName, s_ShaderGenTable[i].m_BoneCount) == false)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not compile %s shader\n", m_pShader->GetName());
				goto ExitPart;
			}
		}
}

	bResult = true;

ExitPart:
	/*#if defined( I3G_BINARY_SHADER)
	if( pszHLSL != nullptr)
	{
	i3MemFree( pszHLSL);
	}
	#endif*/

	if (isUberShader && pszi3DeclCode != nullptr)
		i3MemFree(pszi3DeclCode);

	I3TRACE("\n");

	return bResult;
}

IDirect3DVertexShader9 *	i3ShaderContext::_CreateVertexShader( I3G_SHADER_TYPE type, i3RenderContext * pContext)
{
	IDirect3DVertexShader9 * pShader;
	ID3DXConstantTable * pConstTable;
	IDirect3DDevice9 * pD3DDevice;
	HRESULT hRv;
	DWORD * pData;

	pD3DDevice = (IDirect3DDevice9 *) pContext->GetNativeDevice();

	pData = (DWORD *) m_pShader->getShaderData( type);

	hRv = pD3DDevice->CreateVertexShader( (const DWORD *) pData, &pShader);

	if( hRv == D3DERR_OUTOFVIDEOMEMORY)
	{
		I3PRINTLOG(I3LOG_NOTICE, "CreateVertexShader D3DERR_OUTOFVIDEOMEMORY ");
		// Video Memory가 고갈되었다면, 기존에 Loading된 Vertex Shader 등을 VRAM에서 내린 후
		// 다시 시도한다.
		pD3DDevice->EvictManagedResources();

		hRv = pD3DDevice->CreateVertexShader( (const DWORD *) pData, &pShader);
		CHECKRESULT(hRv);
	}

	if( FAILED( hRv))
	{
		I3TRACE( "Could not create vertex shader(%s)\n", m_pShader->GetName());
		addState( I3G_SHADER_CTX_STATE_ERROR);
		
		return nullptr;
	}

	hRv = D3DXGetShaderConstantTable( pData, &pConstTable);
	if( FAILED( hRv))
	{
		I3TRACE( "Could not retreive vertex shader parameters(%s)\n", m_pShader->GetName());
		addState( I3G_SHADER_CTX_STATE_ERROR);
		
		return nullptr;
	}

	if( _SyncParams( type, (DWORD *) pData, pConstTable) == false)
	{
		I3TRACE( "Could not initialize shader parameter(s) (%s)\n", m_pShader->GetName());
		addState( I3G_SHADER_CTX_STATE_ERROR);

		return nullptr;
	}

	pConstTable->Release();

	return pShader;
}

IDirect3DPixelShader9 *		i3ShaderContext::_CreatePixelShader( i3RenderContext * pContext)
{
	IDirect3DPixelShader9 * pShader;
	ID3DXConstantTable * pConstTable;
	IDirect3DDevice9 * pD3DDevice;
	HRESULT hRv;
	DWORD * pData = (DWORD *) m_pShader->getShaderData( I3G_SHADER_TYPE_PIXEL);

	pD3DDevice = (IDirect3DDevice9 *) pContext->GetNativeDevice();

	hRv = pD3DDevice->CreatePixelShader( (const DWORD *) pData, &pShader);

	if( hRv == D3DERR_OUTOFVIDEOMEMORY)
	{
		I3PRINTLOG(I3LOG_NOTICE, "CreatePixelShader D3DERR_OUTOFVIDEOMEMORY");
		// Video Memory가 고갈되었다면, 기존에 Loading된 Pixel Shader 등을 VRAM에서 내린 후
		// 다시 시도한다.
		pD3DDevice->EvictManagedResources();

		hRv = pD3DDevice->CreatePixelShader( (const DWORD *) pData, &pShader);
	}

	if( FAILED( hRv))
	{
		// ATI Radeon 9600계열에서의 Pixel Shader Texture갯수의 제한으로 인한 Shader생성 오류로써,
		// 예외적인 Code를 사용! 단! 이 Code는 반드시 고쳐져야 함! -송명일
		//I3PRINTLOG(I3LOG_WARN,  "Could not create pixel shader(%s)\n", getPixelShaderName());
		I3TRACE( "Could not create pixel shader(%s)", m_pShader->GetName());
		addState( I3G_SHADER_CTX_STATE_ERROR);

		return nullptr;
	}

	hRv = D3DXGetShaderConstantTable( pData, &pConstTable);
	if( FAILED( hRv))
	{
		I3TRACE( "Could not retreive vertex shader parameters(%s)", m_pShader->GetName());
		addState( I3G_SHADER_CTX_STATE_ERROR);
		
		return nullptr;
	}

	_SyncParams( I3G_SHADER_TYPE_PIXEL, (DWORD *) pData, pConstTable);

	pConstTable->Release();


	return pShader;
}

bool i3ShaderContext::BuildNativeShader( i3RenderContext * pContext)
{
	INT32 i;

	static UINT32 s_validMask[ I3G_SHADER_TYPE_MAX] =
	{
		I3G_SHADER_STATE_VS_0_VALIDATED,
		I3G_SHADER_STATE_VS_1_VALIDATED,
		I3G_SHADER_STATE_VS_2_VALIDATED,
		I3G_SHADER_STATE_VS_3_VALIDATED,
		I3G_SHADER_STATE_VS_4_VALIDATED,
		I3G_SHADER_STATE_PS_VALIDATED
	};

	for( i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		if( m_pShader->isCompiled( (I3G_SHADER_TYPE) i) && !m_pShader->isValidated( (I3G_SHADER_TYPE) i))
		{
			if( i != I3G_SHADER_TYPE_PIXEL)
			{
				IDirect3DVertexShader9 * pShader = (IDirect3DVertexShader9*) m_pShaderBin[i];
				I3_SAFE_RELEASE_NODBG( pShader );
				m_pShaderBin[i] = _CreateVertexShader( (I3G_SHADER_TYPE) i, pContext);
			}
			else
			{
				IDirect3DPixelShader9 * pShader = (IDirect3DPixelShader9*) m_pShaderBin[i];
				I3_SAFE_RELEASE_NODBG( pShader);
				m_pShaderBin[i] = _CreatePixelShader( pContext);
			}

			if( m_pShaderBin[i] == nullptr)
			{
				addState( I3G_SHADER_CTX_STATE_ERROR);
				return false;
			}

			m_pShader->addState( s_validMask[i]);
		}
	}

	addState( I3G_SHADER_CTX_STATE_COMPILED);

	return true;
}

bool i3ShaderContext::Destroy( bool bLostDevice)
{
	if( bLostDevice == false)
		return true;

	RemoveAllParams();
	_ReleaseShaders();

	m_pShader->removeState( I3G_SHADER_STATE_ALL_VALIDATED);

	addState( I3G_SHADER_CTX_STATE_LOSTDEVICE);

	return true;
}

bool i3ShaderContext::Revive( i3RenderContext * pCtx)
{
	if( isState( I3G_SHADER_CTX_STATE_LOSTDEVICE))
	{
		removeState( I3G_SHADER_CTX_STATE_LOSTDEVICE);

		if( BuildNativeShader( pCtx) == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not revive shader context (%s)", m_pShader->GetName());
			return false;
		}
	}

	return i3GfxResource::Revive( pCtx);
}

#if defined( I3_DEBUG)
void i3ShaderContext::OnQueryStat( I3G_RES_STAT * pInfo)
{
	INT32 i;

	for( i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		if( m_pShaderBin[i] != nullptr)
		{
			UINT sz = 0;

			pInfo->m_ShaderCount++;

			sz += m_pShader->getShaderDataSize( (I3G_SHADER_TYPE) i);

			pInfo->m_ShaderSize += sz;
		}
	}	
}
#endif

#endif
