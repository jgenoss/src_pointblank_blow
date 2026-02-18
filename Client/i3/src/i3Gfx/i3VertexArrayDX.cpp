#include "i3GFXType.h"
#include "../i3Math/i3Color.h"
#include "i3RenderContext.h"
#include "i3GfxGlobalVariable.h"

#ifdef I3G_DX
#include "i3GfxUtilDX.h"
#include "i3VertexArray.h"
#include "i3VertexArrayDX.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3GfxCaps.h"
#include "i3Input/dxerr.h"

#define VA_REQ_BUF_COUNT	128

enum REQ_TYPE {
	REQ_TYPE_NONE = 0,
	REQ_TYPE_RESERVE,	// 사용을 예약한 상태
	REQ_TYPE_CALCSIZE,
	REQ_TYPE_CREATEBUF,
	REQ_TYPE_LOCK,
	REQ_TYPE_UNLOCK,
};

struct VADXREQINFO {
	UINT32			_idx = 0;
	UINT32			_param1 = 0;
	UINT32			_param2 = 0;
	UINT32			_param3 = 0;
	DWORD			_dparam = 0;

	i3VertexArray *	_pVA = nullptr;
	i3VertexFormat	_format;

	UINT32			_result = 0;
	REQ_TYPE		_type = REQ_TYPE_NONE;
};

static bool		sVAReqInit = false;
static UINT32	s_VAReqRead = 0;
static UINT32	s_VAReqWrite = 0;
VADXREQINFO 	s_VAReq[VA_REQ_BUF_COUNT]; 
VADXREQINFO 	s_VAAck[VA_REQ_BUF_COUNT];

static i3Mutex2	sDXVAMutex;

I3_CLASS_INSTANCE( i3VertexArrayDX);

i3VertexArrayDX::i3VertexArrayDX(void)
{
	if( sVAReqInit == false)
	{
		i3mem::FillZero( s_VAReq, sizeof( s_VAReq));
		i3mem::FillZero( s_VAAck, sizeof( s_VAAck));
		sVAReqInit = true;
	}
}

i3VertexArrayDX::~i3VertexArrayDX(void)
{
	I3MEM_SAFE_FREE( m_pResData);

	NetworkDump_Free( m_pVB, __FILE__, __LINE__);
	NetworkDump_Free( m_pVBDecl, __FILE__, __LINE__);
	I3_SAFE_RELEASE_NODBG( m_pVB);
	I3_SAFE_RELEASE_NODBG( m_pVBDecl);
}

static D3DVERTEXELEMENT9 s_Decl[80];

UINT32 i3VertexArrayDX::CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count)
{
	i3Monitor2 mon( &sDXVAMutex);

	INT32 idx = 0;

	// Position (X, Y, Z) Component
	{
		m_OffsetPos = 0;

		if( pFormat->GetHasPosition())
		{
			m_Stride += sizeof(REAL32) * 3;

			s_Decl[idx].Stream = 0;
			s_Decl[idx].Offset = (WORD) m_OffsetPos;
			s_Decl[idx].Type = D3DDECLTYPE_FLOAT3;
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_POSITION;
			s_Decl[idx].UsageIndex = 0;
			idx++;
		}
	}

	// RHW(Reciprocal Homogeneous W) Component
	{
		m_OffsetRHW = m_Stride;

		if( pFormat->IsTransformed() && !pFormat->GetHasNormal())
		{
			m_Stride += sizeof(REAL32);

			s_Decl[idx - 1].Type = D3DDECLTYPE_FLOAT4;
			s_Decl[idx - 1].Usage = D3DDECLUSAGE_POSITIONT;
		}
	}

	// Blend Weight Component
	{
		m_OffsetWeight = m_Stride;

		if( pFormat->GetBlendWeightCount() > 0)
		{
			#if 0
				for( i = 0; i < pFormat->GetBlendWeightCount(); i++)
				{
					m_Stride += sizeof(REAL32);

					s_Decl[idx].Stream = 0;
					s_Decl[idx].Offset = (WORD) (m_OffsetWeight + (i * sizeof(REAL32)));
					s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT1);
					s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
					s_Decl[idx].Usage = D3DDECLUSAGE_BLENDWEIGHT;
					s_Decl[idx].UsageIndex = 0;
					idx++;
				}
			#else
				m_Stride += sizeof(REAL32) * pFormat->GetBlendWeightCount();

				s_Decl[idx].Stream = 0;
				s_Decl[idx].Offset = (WORD) m_OffsetWeight;
				s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT1 + pFormat->GetBlendWeightCount() - 1);
				s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
				s_Decl[idx].Usage = D3DDECLUSAGE_BLENDWEIGHT;
				s_Decl[idx].UsageIndex = 0;
				idx++;
			#endif 
		}
	}

	// Blend Matrix Index Component
	{
		i3GfxCaps * pCaps = i3RenderContext::GetCaps();

		m_OffsetBlendIndex = m_Stride;

		if( pFormat->GetBlendIndexCount() > 0)
		{
			if( pCaps->isSupported_UBYTE4())
			{
				m_Stride += sizeof(UINT32);

				s_Decl[idx].Stream = 0;
				s_Decl[idx].Offset = (WORD) m_OffsetBlendIndex;
				s_Decl[idx].Type = D3DDECLTYPE_UBYTE4;
				s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
				s_Decl[idx].Usage = D3DDECLUSAGE_BLENDINDICES;
				s_Decl[idx].UsageIndex = 0;
				idx++;
			}
			else
			{
				m_Stride += sizeof(REAL32) * pFormat->GetBlendIndexCount();
				
				s_Decl[idx].Stream = 0;
				s_Decl[idx].Offset = (WORD) m_OffsetBlendIndex;
				s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT1 + pFormat->GetBlendIndexCount() - 1);
				s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
				s_Decl[idx].Usage = D3DDECLUSAGE_BLENDINDICES;
				s_Decl[idx].UsageIndex = 0;
				idx++;
			}
		}
	}

	// Normal Component
	{
		m_OffsetNormal = m_Stride;

		if( pFormat->GetHasNormal())
		{
			m_Stride += sizeof(REAL32) * 3;

			s_Decl[idx].Stream = 0;
			s_Decl[idx].Offset = (WORD) m_OffsetNormal;
			s_Decl[idx].Type = D3DDECLTYPE_FLOAT3;
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_NORMAL;
			s_Decl[idx].UsageIndex = 0;
			idx++;
		}
	}

	// Vertex Diffuse Color Component
	{
		m_OffsetColor = m_Stride;

		if( pFormat->GetHasColor())
		{
			m_Stride += sizeof(UINT32);

			s_Decl[idx].Stream = 0;
			s_Decl[idx].Offset = (WORD) m_OffsetColor;
			s_Decl[idx].Type = D3DDECLTYPE_D3DCOLOR;
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_COLOR;
			s_Decl[idx].UsageIndex = 0;
			idx++;
		}
	}

	// Texture Coord. Set Components
	{
		UINT32 i;

		for( i = 0; i < (UINT32)pFormat->GetTextureCoordSetCount(); i++)
		{
			m_OffsetTexCoord[i] = m_Stride;

			m_Stride += sizeof(REAL32) * pFormat->GetTextureCoordCount();

			s_Decl[idx].Stream = 0;
			s_Decl[idx].Offset = (WORD) m_OffsetTexCoord[i];
			s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT1 + (pFormat->GetTextureCoordCount() - 1));
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_TEXCOORD;
			s_Decl[idx].UsageIndex = (BYTE) i;
			idx++;
		}
	}

	// Tangent
	{
		m_OffsetTangent = m_Stride;

		if( pFormat->GetHasTangent())
		{
			m_Stride += sizeof(REAL32) * 3;

			s_Decl[idx].Stream = 0;
			s_Decl[idx].Offset = (WORD) m_OffsetTangent;
			s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT3);
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_TANGENT;
			s_Decl[idx].UsageIndex = 0;
			idx++;
		}
	}

	// Binormal
	{
		m_OffsetBinormal = m_Stride;

		if( pFormat->GetHasBinormal())
		{
			m_Stride += sizeof(REAL32) * 3;

			s_Decl[idx].Stream = 0;
			s_Decl[idx].Offset = (WORD) m_OffsetBinormal;
			s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT3);
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_BINORMAL;
			s_Decl[idx].UsageIndex = 0;
			idx++;
		}
	}

	s_Decl[idx].Stream = 0xFF;
	s_Decl[idx].Offset = 0;
	s_Decl[idx].Type = D3DDECLTYPE_UNUSED;
	s_Decl[idx].Method = 0;
	s_Decl[idx].Usage = 0;
	s_Decl[idx].UsageIndex = 0;

	{
		HRESULT hRv;

		// FVF를 사용해서 정점선언값을 자동으로 채워넣는다
		//D3DXDeclaratorFromFVF(GetVertexFormatCode(), s_Decl );

		hRv = g_pD3DDevice->CreateVertexDeclaration( s_Decl, &m_pVBDecl);
		CHECKRESULT(hRv);
		NetworkDump_Malloc( m_pVBDecl, sizeof(D3DVERTEXELEMENT9) * idx, __FILE__, __LINE__);
		//if( FAILED( hRv))
		//{
		//	//DXTRACE_ERR( "", hRv);
		//	I3PRINTLOG(I3LOG_FATAL,  "Could not create vertex declaration data");
		//}

		#if 0
		{
			D3DVERTEXELEMENT9	decl[80];
			UINT32 cnt, i;
			static char s_DeclName[][32] = 
			{
				"Position",
				"Weight",
				"Index",
				"Normal",
				"Psize",
				"Texcoord",
				"Tangent",
				"Binormal",
				"TessFactor",
				"PositionT",
				"Color",
				"Fog",
				"Depth",
				"Sample",
			};

			hRv = m_pVBDecl->GetDeclaration( decl, &cnt);
			CHECKRESULT( hRv);

			I3TRACE( "---------------------\n");
			for( i = 0; i < cnt; i++)
			{
				I3TRACE( "DECL - OFF:%d USAGE:%s\n", decl[i].Offset, s_DeclName[ decl[i].Usage]);
			}
		}
		#endif
	}

	return m_Stride * Count;
}

UINT32 i3VertexArrayDX::CalcBufferSizeForThread( i3VertexFormat * pFormat, UINT32 Count)
{
	i3Monitor2 mon( &sDXVAMutex);

	while( s_VAReqWrite - s_VAReqRead >= VA_REQ_BUF_COUNT)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_VAReqWrite;	//sFindEmptyDXVAReqInfo();

	while( s_VAAck[ idx % VA_REQ_BUF_COUNT]._type != REQ_TYPE_NONE)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	VADXREQINFO * pReq = &s_VAReq[ idx % VA_REQ_BUF_COUNT];
	
	pReq->_pVA = this;
	I3_MUST_ADDREF( pReq->_pVA);
	I3ASSERT( pReq->_pVA->GetRefCount() > 1);
	pReq->_format = *pFormat;
	pReq->_param1 = Count;
	pReq->_result = 0;
	pReq->_idx = idx;
	pReq->_type = REQ_TYPE_CALCSIZE;

	//s_DXVAReqQue.push( pReq);
	s_VAReqWrite++;
	
	return idx;
}

UINT32 i3VertexArrayDX::CreateDXBuffer( UINT32 BuffSz, UINT32 Usage, D3DPOOL Pool)
{
	i3Monitor2 mon( &sDXVAMutex);

	while( s_VAReqWrite - s_VAReqRead >= VA_REQ_BUF_COUNT)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_VAReqWrite;	//sFindEmptyDXVAReqInfo();

	while( s_VAAck[ idx % VA_REQ_BUF_COUNT]._type != REQ_TYPE_NONE)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	VADXREQINFO * pReq = &s_VAReq[ idx % VA_REQ_BUF_COUNT];

	pReq->_pVA = this;
	I3_MUST_ADDREF( pReq->_pVA);
	I3ASSERT( pReq->_pVA->GetRefCount() > 1);
	pReq->_param1 = BuffSz;
	pReq->_param2 = Usage;
	pReq->_param3 = Pool;
	pReq->_idx = idx;
	pReq->_type = REQ_TYPE_CREATEBUF;

	//s_DXVAReqQue.push( pReq);
	s_VAReqWrite++;
	
	return idx;
}

UINT32 i3VertexArrayDX::LockDXBuffer( UINT32 StartOff, UINT32 Size, I3G_LOCKMETHOD lock)
{
	i3Monitor2 mon( &sDXVAMutex);

	while( s_VAReqWrite - s_VAReqRead >= VA_REQ_BUF_COUNT)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_VAReqWrite;	//sFindEmptyDXVAReqInfo();

	while( s_VAAck[ idx % VA_REQ_BUF_COUNT]._type != REQ_TYPE_NONE)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	VADXREQINFO * pReq = &s_VAReq[ idx % VA_REQ_BUF_COUNT];

	pReq->_pVA = this;
	I3_MUST_ADDREF( pReq->_pVA);
	I3ASSERT( pReq->_pVA->GetRefCount() > 1);
	pReq->_param1 = StartOff;
	pReq->_param2 = Size;
	pReq->_param3 = lock;
	pReq->_idx = idx;
	pReq->_type = REQ_TYPE_LOCK;

	//s_DXVAReqQue.push( pReq);
	s_VAReqWrite++;
	
	return idx;
}

UINT32 i3VertexArrayDX::UnlockDXBuffer( void)
{
	i3Monitor2 mon( &sDXVAMutex);

	while( s_VAReqWrite - s_VAReqRead >= VA_REQ_BUF_COUNT)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_VAReqWrite;	//sFindEmptyDXVAReqInfo();

	while( s_VAAck[ idx % VA_REQ_BUF_COUNT]._type != REQ_TYPE_NONE)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	VADXREQINFO * pReq = &s_VAReq[ idx % VA_REQ_BUF_COUNT];

	pReq->_pVA = this;
	I3_MUST_ADDREF( pReq->_pVA);
	I3ASSERT( pReq->_pVA->GetRefCount() > 1);
	pReq->_idx = idx;
	pReq->_type = REQ_TYPE_UNLOCK;

	//s_DXVAReqQue.push( pReq);
	s_VAReqWrite++;

	return idx;
}

static i3Mutex2 sVertexArrayUpdateMutex;

void i3VertexArrayDX::UpdateForThread( void)
{
	i3Monitor2 mon( &sVertexArrayUpdateMutex);

	UINT32 cnt = s_VAReqWrite - s_VAReqRead;	//s_DXVAReqQue.size();

	for( UINT32 i = 0; i < cnt; i++)
	{
		UINT32 idx = s_VAReqRead % VA_REQ_BUF_COUNT;
		VADXREQINFO * pReq = &s_VAReq[ idx];	//s_DXVAReqQue.front();

		I3ASSERT( pReq->_pVA != nullptr);
		I3ASSERT( pReq->_type > REQ_TYPE_RESERVE);

		{
			switch( pReq->_type)
			{
			case REQ_TYPE_CALCSIZE :	pReq->_result = pReq->_pVA->CalcBufferSize( &pReq->_format, pReq->_param1);	break;
			case REQ_TYPE_CREATEBUF :
				{
					pReq->_result = (UINT32) g_pD3DDevice->CreateVertexBuffer( pReq->_param1, pReq->_param2,
						pReq->_pVA->GetVertexFormatCode(), (D3DPOOL)pReq->_param3, &((i3VertexArrayDX*)pReq->_pVA)->m_pVB, NULL);
					CHECKRESULT(pReq->_result);
					NetworkDump_Malloc( ((i3VertexArrayDX*)pReq->_pVA)->m_pVB, pReq->_param1, __FILE__, __LINE__);
				}
				break;

			case REQ_TYPE_LOCK :
				{
					pReq->_result = (UINT32) ((i3VertexArrayDX*)pReq->_pVA)->getDXInstance()->Lock( pReq->_param1, pReq->_param2,
						(void**) &pReq->_pVA->m_pLockedBuf, (I3G_LOCKMETHOD) pReq->_param3);
					CHECKRESULT(pReq->_result);
				}
				break;

			case REQ_TYPE_UNLOCK :
				pReq->_result = (UINT32) ((i3VertexArrayDX*)pReq->_pVA)->getDXInstance()->Unlock();
				CHECKRESULT(pReq->_result);
				break;

			default :
				I3PRINTLOG(I3LOG_FATAL,  "아직 등록하지 않은 타입!!!!");
				break;
			}

			/*UINT32 write = sFindEmptyDXVAAckInfo();
			I3ASSERT( write < VA_REQ_BUF_COUNT);*/
			s_VAAck[idx] = *pReq;

			// 초기화
			I3_SAFE_RELEASE( pReq->_pVA);
			pReq->_type = REQ_TYPE_NONE;
		}
		
		//s_DXVAReqQue.pop();
		s_VAReqRead++;
	}
}

void i3VertexArrayDX::LostDeviceForThread( void)
{
	//i3Monitor2 mon( &sDXVAMutex);

	//for( UINT32 i = 0; i < VA_REQ_BUF_COUNT; i++)
	//{
	//	VADXREQINFO * pReq = &s_VAReq[i];
	//	I3_SAFE_RELEASE( pReq->_pVA);	// 
	//	pReq->_type = REQ_TYPE_NONE;
	//}

	//s_VAReqWrite = 0;
	//s_VAAckWrite = 0;
}

UINT32 i3VertexArrayDX::GetReqResult( UINT32 req)
{
	UINT32 result = 0;

	bool bWait = true;

	while( bWait)
	{
		for( UINT32 i = 0; i < VA_REQ_BUF_COUNT; i++)
		{
			VADXREQINFO * pAck = &s_VAAck[i];

			if( pAck->_idx == req &&
				pAck->_type > REQ_TYPE_RESERVE)
			{
				result = pAck->_result;
				pAck->_pVA = nullptr;
				pAck->_type = REQ_TYPE_NONE;

				bWait = false;
				break;				
			}
		}

		if( bWait)
		{
			i3Timer::Sleep( 1);
		}
	}

	return result;
}

bool i3VertexArrayDX::Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE usage)
{
	HRESULT Rv;
	UINT32 BuffSz, Usage;
	D3DPOOL Pool;

	I3ASSERT( g_pD3DDevice);
	I3ASSERT( count > 0);

	// 만약 Software Processing을 해야하는 상황이라면
	// Usage Flag에 I3G_USAGE_SOFTWAREPROCESSING을 추가한다.
	usage = i3DXUT_DetermineRuntimeUsage( pFormat, usage);

	i3VertexArray::Create( pFormat, count, usage);

	if( g_pRenderContext->getGfxOption()->getNormalMapEnable() == false)
	{
		m_Format.SetHasTangent(false);
		m_Format.SetHasBinormal(false);
	}

	// DirectX는 REAL32 타입만 사용할 수 있기 때문에...
	m_Format.SetPositionType( I3VF_TYPE_REAL32);
	m_Format.SetNormalType( I3VF_TYPE_REAL32);
	m_Format.SetColorType( I3VF_TYPE_COLOR32_RGBA_8888);
	m_Format.SetTangentType( I3VF_TYPE_REAL32);
	m_Format.SetTextureCoordType( I3VF_TYPE_REAL32);
	m_Format.SetBlendWeightType( I3VF_TYPE_REAL32);
	m_Format.SetTangentType( I3VF_TYPE_REAL32);
	m_Format.SetBinormalType( I3VF_TYPE_REAL32);

	m_VertexFormatCode = i3DXUT_GetNativeVertexFormat( &m_Format);

	BuffSz = CalcBufferSize( &m_Format, count);

	Usage = i3DXUT_GetNativeUsage( usage);

	if( usage & I3G_USAGE_SOFTWAREPROCESSING)
	{
		Pool = D3DPOOL_SYSTEMMEM;
	}
	else if( usage & (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE))
	{
		Pool = D3DPOOL_DEFAULT;
	}
	else
	{
		Pool = D3DPOOL_MANAGED;
	}

	if( getThreadMode())
	{
		UINT32 que = CreateDXBuffer( BuffSz, Usage, Pool);

		Rv = (HRESULT) GetReqResult( que);	// blocked
	}
	else
	{
		Rv = g_pD3DDevice->CreateVertexBuffer( BuffSz, Usage, m_VertexFormatCode, Pool, &m_pVB, NULL);
		NetworkDump_Malloc( m_pVB, BuffSz, __FILE__, __LINE__);
	}

	if( Rv == D3DERR_OUTOFVIDEOMEMORY)
	{
		I3PRINTLOG(I3LOG_NOTICE, "CreateDepthStencilSurface D3DERR_OUTOFVIDEOMEMORY");
		// Video Memory가 고갈되었다면, 기존에 Loading된 VertexBuffer 등을 VRAM에서 내린 후
		// 다시 시도한다.
		g_pD3DDevice->EvictManagedResources();

		Rv = g_pD3DDevice->CreateVertexBuffer( BuffSz, Usage, m_VertexFormatCode, Pool, &m_pVB, NULL);
		NetworkDump_Malloc( m_pVB, BuffSz, __FILE__, __LINE__);
	}

	if( FAILED( Rv))
	{
		switch( Rv)
		{
		case D3DERR_OUTOFVIDEOMEMORY :	
			if( m_CallBackExternalOutOfMemory)	m_CallBackExternalOutOfMemory();
			I3ERRLOG( i3Error::_OUTOFVIDEOMEMORY);
			break;
		default :
			I3ERRLOG( i3Error::_UNSUPPORTED_GPU);	break;
		}

		CHECKRESULT(Rv);
		return false;
	}

	m_pBuffer = (void *) m_pVB;

	if( m_Format.IsTransformed())
	{
		UINT32 i;
		REAL32 * pW;
		//UINT8 cnt = 0;

		if( Lock() == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "VA lock failed!!!!!");
		}

		pW = (REAL32 *) m_pLockedBuf;
		pW += 3;		// Position 부분을 건너 뛴다.

		for( i = 0; i < count; i++)
		{
			*pW = 1.0f;			// DirectX에서 Transformed Vertex인 경우, RHW Field에 항상 1.0이 들어가 있어야 한다.

			pW = (REAL32 *)((INT8 *)pW + m_Stride);
		}
		
		Unlock();
	}
	
	return true;
};


bool i3VertexArrayDX::Lock( I3G_LOCKMETHOD lock, UINT32 StartIdx, UINT32 Count)
{
	HRESULT Rv;
	UINT StartOff, Size;

	if( m_bLocked)
		return false;

	I3ASSERT( m_pBuffer);

	StartOff = StartIdx * m_Stride;

	m_LockStartIdx = StartIdx;

	if( Count == 0)
	{
		Count = m_Count;
	}

	m_LockCount = Count;

	Size = Count * m_Stride;

	Rv = m_pVB->Lock( StartOff, Size, (VOID **) & m_pLockedBuf, lock);
	if( FAILED( Rv))
	{
		I3PRINTLOG(I3LOG_WARN,  "%s", DXGetErrorString( Rv));
		return false;
	}

	m_bLocked = true;

	return true;
}

void i3VertexArrayDX::Unlock(void)
{
	HRESULT Rv;

	if( ! IsLocked())
	{
		return;
	}

	I3ASSERT( m_pBuffer);
	I3ASSERT( m_pLockedBuf);

	/*if( getThreadMode())
	{
		UINT32 req = UnlockDXBuffer();
		Rv = (HRESULT) GetReqResult( req);
	}
	else*/
		Rv = m_pVB->Unlock();
	if( FAILED( Rv))
	{
		I3PRINTLOG(I3LOG_WARN,  "%s", DXGetErrorString( Rv));
	}

	m_pLockedBuf = nullptr;
	m_bLocked = false;
}

bool i3VertexArrayDX::SafeLock( I3G_LOCKMETHOD lock/* = I3G_LOCK_NONE*/, UINT32 StartIdx /*= 0*/, UINT32 Count /*= 0*/)
{
	m_Mutex.Enter();
	return Lock( lock, StartIdx, Count);
}

bool i3VertexArrayDX::SafeUnlock(void)
{
	Unlock();

	m_Mutex.Leave();

	return true;
}


void i3VertexArrayDX::MakeRestoreData(void)
{
	I3MEM_SAFE_FREE( m_pResData);

	m_ResDataSize = GetStride() * GetCount();
	m_pResData = (char *) i3MemAlloc( m_ResDataSize);

	bool bRv = Lock( I3G_LOCK_READONLY);
	I3ASSERT( bRv == true);

	memcpy( m_pResData, GetLockedBuffer(), m_ResDataSize);

	Unlock();
}

bool	i3VertexArrayDX::Destroy( bool bLostDevice)
{
	if( m_pVB == nullptr) return true;

	bool bDestroy = false;

	if( GetUsageFlag() & (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE))
	{
		bDestroy = true;
	}
	else
	{
		if( bLostDevice)
			bDestroy = true;
	}

	if( IsLocked() )
	{
		bool bTemp = getThreadMode();
		setThreadMode( false);
		Unlock();
		setThreadMode( bTemp);
	}

	if (g_pRenderContext->IsRenderFlag( I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR) == false)
	{
		if( getRestoreType() == I3G_RESTORE_MEM)
		{
			MakeRestoreData();
		}
	}

	if( bDestroy)
	{
		NetworkDump_Free( m_pVB, __FILE__, __LINE__);
		NetworkDump_Free( m_pVBDecl, __FILE__, __LINE__);
		I3_SAFE_RELEASE_NODBG( m_pVB);
		I3_SAFE_RELEASE_NODBG( m_pVBDecl);
	}

	return i3VertexArray::Destroy( bLostDevice);
}

bool	i3VertexArrayDX::isDestroyed( void)
{
	if( m_pVB != nullptr || m_pVBDecl != nullptr) return false;
	
	return true;
}

bool	i3VertexArrayDX::Revive( i3RenderContext * pCtx)
{
	i3::pack::VERTEX_OFFSET Info;

	if( m_pVB != nullptr)
		return true;

	bool bTemp = getThreadMode();
	setThreadMode( false);

	if( GetCount() > 0  )
	{
		switch( getRestoreType())
		{
			case I3G_RESTORE_NONE :
				{
					// 데이터는 저장할 필요가 없는 것이라 판단.
					// 그러나, 새롭게 생성은 해주어야 한다.
					Create( GetFormat(), GetCount(), GetUsageFlag());
				}
				break;

			case I3G_RESTORE_MEM :
				{
					if( m_pResData != nullptr)
					{
						Create( GetFormat(), GetCount(), GetUsageFlag());

						bool bRv = Lock( 0);
						I3ASSERT( bRv == true);

						memcpy( GetLockedBuffer(), m_pResData, m_ResDataSize);

						Unlock();

						I3MEM_SAFE_FREE( m_pResData);

						m_ResDataSize = 0;
					}
				}
				break;

			case I3G_RESTORE_FILE :
				{
					setThreadMode( bTemp);
					return i3VertexArray::Revive( pCtx);
				}
				break;

		}
	}

	setThreadMode( bTemp);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 i3VertexArrayDX::BuildDecl( INT32 * pidx, i3VertexFormat * pFormat, WORD Stream, i3VBUsageIndex * pUsage)
{
	INT32	idx		= *pidx;
	UINT32	Offset	= 0;

	// Position (X, Y, Z) Component
	{
		if( pFormat->GetHasPosition())
		{
			s_Decl[idx].Stream = Stream;
			s_Decl[idx].Offset = (WORD) Offset;
			s_Decl[idx].Type = D3DDECLTYPE_FLOAT3;
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_POSITION;
			s_Decl[idx].UsageIndex = pUsage->Pos;
			idx++;

			Offset	+= sizeof(REAL32) * 3;
			pUsage->Pos++;
		}
	}

	// RHW(Reciprocal Homogeneous W) Component
	{
		if( pFormat->IsTransformed() && !pFormat->GetHasNormal())
		{
			Offset += sizeof(REAL32);

			s_Decl[idx - 1].Type = D3DDECLTYPE_FLOAT4;
			s_Decl[idx - 1].Usage = D3DDECLUSAGE_POSITIONT;
		}
	}

	// Blend Weight Component
	{
		if( pFormat->GetBlendWeightCount() > 0)
		{
			s_Decl[idx].Stream = Stream;
			s_Decl[idx].Offset = (WORD) Offset;
			s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT1 + pFormat->GetBlendWeightCount() - 1);
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_BLENDWEIGHT;
			s_Decl[idx].UsageIndex = pUsage->Weight;

			Offset += sizeof(REAL32) * pFormat->GetBlendWeightCount();
			idx++;
			pUsage->Weight++;
		}
	}

	// Blend Matrix Index Component
	{
		i3GfxCaps * pCaps = i3RenderContext::GetCaps();

		if( pFormat->GetBlendIndexCount() > 0)
		{
			if( pCaps->isSupported_UBYTE4())
			{
				s_Decl[idx].Stream = Stream;
				s_Decl[idx].Offset = (WORD) Offset;
				s_Decl[idx].Type = D3DDECLTYPE_UBYTE4;
				s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
				s_Decl[idx].Usage = D3DDECLUSAGE_BLENDINDICES;
				s_Decl[idx].UsageIndex = pUsage->BlendIdx;

				Offset += sizeof(UINT32);
			}
			else
			{
				s_Decl[idx].Stream = Stream;
				s_Decl[idx].Offset = (WORD) Offset;
				s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT1 + pFormat->GetBlendIndexCount() - 1);
				s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
				s_Decl[idx].Usage = D3DDECLUSAGE_BLENDINDICES;
				s_Decl[idx].UsageIndex = pUsage->BlendIdx;

				Offset += sizeof(REAL32) * pFormat->GetBlendIndexCount();
			}

			pUsage->BlendIdx++;
			idx++;
		}
	}

	// Normal Component
	{
		if( pFormat->GetHasNormal())
		{
			s_Decl[idx].Stream = Stream;
			s_Decl[idx].Offset = (WORD) Offset;
			s_Decl[idx].Type = D3DDECLTYPE_FLOAT3;
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_NORMAL;
			s_Decl[idx].UsageIndex = pUsage->Normal;

			Offset += sizeof(REAL32) * 3; 

			pUsage->Normal++;
			idx++;
		}
	}

	// Vertex Diffuse Color Component
	{
		if( pFormat->GetHasColor())
		{
			s_Decl[idx].Stream = Stream;
			s_Decl[idx].Offset = (WORD) Offset;
			s_Decl[idx].Type = D3DDECLTYPE_D3DCOLOR;
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_COLOR;
			s_Decl[idx].UsageIndex = pUsage->Color;

			Offset += sizeof(UINT32);

			pUsage->Color++;
			idx++;
		}
	}

	// Texture Coord. Set Components
	{
		UINT32 i;

		for( i = 0; i < (UINT32)pFormat->GetTextureCoordSetCount(); i++)
		{
			s_Decl[idx].Stream = Stream;
			s_Decl[idx].Offset = (WORD) Offset;
			s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT1 + (pFormat->GetTextureCoordCount() - 1));
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_TEXCOORD;
			s_Decl[idx].UsageIndex = pUsage->TexCoord;

			Offset += sizeof(REAL32) * pFormat->GetTextureCoordCount();

			pUsage->TexCoord++;
			idx++;
		}
	}

	// Tangent
	{
		if( pFormat->GetHasTangent())
		{
			s_Decl[idx].Stream = Stream;
			s_Decl[idx].Offset = (WORD) Offset;
			s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT3);
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_TANGENT;
			s_Decl[idx].UsageIndex = pUsage->Tangent;

			Offset += sizeof(REAL32) * 3;

			pUsage->Tangent++;
			idx++;
		}
	}

	// Binormal
	{
		if( pFormat->GetHasBinormal())
		{
			s_Decl[idx].Stream = Stream;
			s_Decl[idx].Offset = (WORD) Offset;
			s_Decl[idx].Type = (BYTE) (D3DDECLTYPE_FLOAT3);
			s_Decl[idx].Method = D3DDECLMETHOD_DEFAULT;
			s_Decl[idx].Usage = D3DDECLUSAGE_BINORMAL;
			s_Decl[idx].UsageIndex = pUsage->Binormal;

			Offset += sizeof(REAL32) * 3;

			pUsage->Binormal++;
			idx++;
		}
	}

	*pidx = idx;

	return Offset;
}

IDirect3DVertexDeclaration9 *  i3VertexArrayDX::GetCombinedVBDecl( i3VertexFormat * pStream0, i3VertexFormat * pStream1)
{
	//usage index will be accumulated
	i3VBUsageIndex Usage;
	i3mem::FillZero( &Usage, sizeof(Usage));

	INT32 idx = 0;
	UINT32 Stride1, Stride2;

	Stride1 = BuildDecl( &idx, pStream0, 0, &Usage);
	if( Usage.TexCoord == 0)
		Usage.TexCoord = 1;

	Stride2 = BuildDecl( &idx, pStream1, 1, &Usage);

	s_Decl[idx].Stream = 0xFF;
	s_Decl[idx].Offset = 0;
	s_Decl[idx].Type = D3DDECLTYPE_UNUSED;
	s_Decl[idx].Method = 0;
	s_Decl[idx].Usage = 0;
	s_Decl[idx].UsageIndex = 0;

	HRESULT hRv;
	IDirect3DVertexDeclaration9 * pVBDecl = nullptr;

	hRv = g_pD3DDevice->CreateVertexDeclaration( s_Decl, &pVBDecl);
	if( FAILED( hRv))
	{
		DXTRACE_ERR(L"", hRv);
		I3PRINTLOG(I3LOG_FATAL,  "Could not reate vertex declaration data");
	}

	return pVBDecl;
}

#if defined( I3_DEBUG)
void	i3VertexArrayDX::OnQueryStat( I3G_RES_STAT * pInfo)
{
	pInfo->m_VBCount++;

	if( m_pVB != nullptr)
	{
		D3DVERTEXBUFFER_DESC desc;

		m_pVB->GetDesc( &desc);

		pInfo->m_VBSize += desc.Size;
		//pInfo->m_VBSize += CalcBufferSize( GetFormat(), GetCount());
	}
}

#endif

#endif