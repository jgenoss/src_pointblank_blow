#include "i3SceneDef.h"
#include "i3Gfx.h"
#include "i3PointSpriteAttr.h"

I3_CLASS_INSTANCE( i3PointSpriteAttr);

i3PointSpriteAttr::~i3PointSpriteAttr(void)
{
	I3_SAFE_RELEASE( m_pVA );
	I3MEM_SAFE_FREE_POINTER( m_pPosBuf ); 
	I3MEM_SAFE_FREE_POINTER( m_pColorBuf ); 
	I3MEM_SAFE_FREE_POINTER( m_pSizeBuf ); 
}

bool i3PointSpriteAttr::Create( UINT32 PrimCount, I3SG_POINTSPRITE_UPDATE_MODE UpdateMode, bool bColor, bool bSize, bool bUV)
{
	i3VertexArray * pVA;
	INT32 SizeCount;

	if( bSize)
		SizeCount = 2;
	else
		SizeCount = 0;

	pVA = i3VertexArray::CreatePointSprite( PrimCount, SizeCount, bColor, bUV);
	I3ASSERT( pVA != nullptr);

	SetVertexArray( pVA, UpdateMode, PrimCount, 0);
	 
//	m_PosBufIdx = i3MemAlloc( sizeof(VEC3D) * m_PrimCount);
	m_pPosBuf = (VEC3D *) i3MemAlloc( sizeof(VEC3D) * m_PrimCount);
	I3ASSERT( m_pPosBuf != nullptr);

	if( bColor )
	{
		m_pColorBuf		= (COLOR *) i3MemAlloc( sizeof(COLOR) * m_PrimCount);
		I3ASSERT( m_pColorBuf != nullptr);
	}

	if( bSize )
	{
		m_pSizeBuf		= (VEC2D *) i3MemAlloc( sizeof(VEC2D) * m_PrimCount);
		I3ASSERT( m_pSizeBuf != nullptr);
	}

	m_bEditted = false;

	return true;
}

void i3PointSpriteAttr::SetVertexArray( i3VertexArray * pVA, I3SG_POINTSPRITE_UPDATE_MODE UpdateMode, UINT32 PrimCount, UINT32 StartIdx)
{
	m_PrimCount = PrimCount;
	m_UpdateMode = UpdateMode;
	m_StartIdx = StartIdx;

	I3_REF_CHANGE( m_pVA, pVA);
}

void i3PointSpriteAttr::UpdateVertexArray(void)
{
	m_pVA->Lock();

	m_pVA->SetPositions( 0, m_PrimCount, m_pPosBuf);

	if( m_pVA->HasColor())
	{
		m_pVA->SetColors( 0, m_PrimCount, m_pColorBuf);
	}
	
	m_pVA->Unlock();

	m_bEditted = false;
}

#if defined( I3G_DX) || defined( I3G_XBOX)
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }
#endif

void i3PointSpriteAttr::Apply( i3RenderContext * pContext)
{
	if( m_bEditted == true)
	{
		UpdateVertexArray();
	}

	#if defined( I3G_DX) || defined( I3G_XBOX)
	g_pD3DDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE);
    g_pD3DDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
    g_pD3DDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
    g_pD3DDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
    g_pD3DDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
    g_pD3DDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );
	#endif

	pContext->SetVertexArray( m_pVA);

	pContext->DrawPointSprite( m_StartIdx, m_PrimCount);
}

void i3PointSpriteAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	if( method == I3_COPY_REF)
	{
		i3PointSpriteAttr * pDest = (i3PointSpriteAttr *) pObj;

		pDest->SetVertexArray( m_pVA, m_UpdateMode, m_PrimCount, m_StartIdx);

		// 사실상, Point Sprite을 복사하는 경우는 발생해서는 안된다.
		// 동일한 Vertex Array에 대해, 서로 다른 여러 개의 PointSprite들이 자신의
		// Position, Color 등의 값을 설정하려고 하기 때문이다.
	}
	else
	{
		i3Log( "i3PointSpriteAttr::CopyTo()", "Invalid Operation : Could not copy i3PointSpriteAttr to another instance.");
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED POINTSPRITEINFO
	{
		UINT8			m_bColor = 0;
		UINT8			m_bSize = 0;
		UINT8			m_UpdateMode = 0;
		UINT8			m_bConstantSize = 0;
		UINT32			m_PrimCount = 0;
		UINT32			m_StartIdx = 0;
		REAL32			m_cx = 0.0f;
		REAL32			m_cy = 0.0f;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3PointSpriteAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3RenderAttr::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Information
	{
		pack::POINTSPRITEINFO	Info;

		if( m_pVA->GetFormat()->GetHasColor())
			Info.m_bColor = 1;
		else
			Info.m_bColor = 0;

		/*
		if( m_pVA->GetFormat()->GetPointSpriteSizeCount() > 0)
			Info.m_bSize = 1;
		else
			Info.m_bSize = 0;
			*/

		Info.m_UpdateMode = (UINT8) m_UpdateMode;

		if( m_bConstantSize)
		{
			Info.m_bConstantSize = 1;

			Info.m_cx = i3Vector::GetU( &m_ConstantSize);
			Info.m_cy = i3Vector::GetV( &m_ConstantSize);
		}
		else
			Info.m_bConstantSize = 0;

		Info.m_PrimCount = m_PrimCount;
		Info.m_StartIdx = m_StartIdx;

		Rc = pStream->Write( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3PointSpriteAttr::OnSave()", "Could not write point sprite information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3PointSpriteAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3PointSpriteAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3RenderAttr::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Information
	{
		pack::POINTSPRITEINFO	Info;

		Rc = pStream->Read( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3PointSpriteAttr::OnLoad()", "Could not read point sprite information.");
			return STREAM_ERR;
		}
		Result += Rc;

		Create( Info.m_PrimCount, (I3SG_POINTSPRITE_UPDATE_MODE) Info.m_UpdateMode, Info.m_bColor != 0, Info.m_bSize != 0);
	}

	return Result;
}
