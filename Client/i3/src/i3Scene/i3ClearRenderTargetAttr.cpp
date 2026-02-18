#include "i3SceneDef.h"
#include "i3ClearRenderTargetAttr.h"

I3_CLASS_INSTANCE( i3ClearRenderTargetAttr);

i3ClearRenderTargetAttr::i3ClearRenderTargetAttr(void)
{
	SetID( I3_ATTRID_CLEARRENDERTARGET);
}

#if defined (I3G_DX)
void i3ClearRenderTargetAttr::SetClearRect( D3DRECT* pRect)
{
	i3mem::FillZero( &m_ClearRectArray, sizeof(D3DRECT)*8);
	
	m_nClearRectCount = 1;
	i3mem::Copy( &m_ClearRectArray[0], pRect, sizeof( D3DRECT));
}

void i3ClearRenderTargetAttr::AddClearRect( D3DRECT* pRect)
{
	m_nClearRectCount++;
	i3mem::Copy( &m_ClearRectArray[m_nClearRectCount], pRect, sizeof( D3DRECT));
}
#endif

void i3ClearRenderTargetAttr::Apply( i3RenderContext * pContext)
{
	COLOR oldColor;
	REAL32	oldZ;
	UINT8	oldStencil;

#if defined (I3G_DX)
//	INT32	nOlRectCount;
//	D3DRECT*	pOldRectArray = nullptr;
#endif

	i3Color::Set( &oldColor, pContext->GetClearColor());
	oldZ = pContext->GetClearZValue();
	oldStencil = pContext->GetClearStencilValue();
	
	pContext->SetClearColor( &m_ClearColor);
	pContext->SetClearZValue( m_ClearZ);
	pContext->SetClearStencilValue( (UINT8) m_ClearStencil);
#if defined (I3G_DX)
	pContext->SetClearRect( m_ClearRectArray, m_nClearRectCount);
#endif

	pContext->Clear( m_AddMask, m_RemoveMask);

	pContext->SetClearColor( &oldColor);
	pContext->SetClearZValue( oldZ);
	pContext->SetClearStencilValue( oldStencil);

	// ! praptor - OldClearRect를 받아내야하나, 현재 구조가 이상하여 보류중입니다.
	// i3RenderContext의 m_pClearRectArray의 해제 책임이 분명치 않습니다.
#if defined (I3G_DX)
	// pContext->SetClearRect( pOldRectArray, nOldRectCount);	
#endif
}

void i3ClearRenderTargetAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	if( i3::same_of<i3ClearRenderTargetAttr*>(pObj))
	{
		i3ClearRenderTargetAttr * pDest = (i3ClearRenderTargetAttr *) pObj;

		pDest->SetClearColor( GetClearColor());
		pDest->SetClearZ( GetClearZ());
		pDest->SetClearStencil( GetClearStencil());
		pDest->SetAddClearMask( GetAddClearMask());
		pDest->SetRemoveClearMask( GetRemoveClearMask());
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct PACKED CLEARRENDERTARGET
	{
		UINT8				m_ID[4] = { 'C', 'R', 'T', '1' };
		UINT32				m_Color = 0;
		REAL32				m_ClearZ = 0.0f;
		UINT32				m_ClearStencil = 0;
		UINT16				m_AddMask = 0;
		UINT16				m_RemoveMask = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3ClearRenderTargetAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::CLEARRENDERTARGET Data;

	Result = i3RenderAttr::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Color
	Data.m_Color = i3Color::ConvertRGBA32( &m_ClearColor);

	// Z
	Data.m_ClearZ = m_ClearZ;

	// Stencil
	Data.m_ClearStencil = m_ClearStencil;

	// Add Mask
	Data.m_AddMask = (UINT16) m_AddMask;

	// Remove Mask
	Data.m_RemoveMask = (UINT16) m_RemoveMask;

	{
		Rc = pStream->Write( &Data, sizeof(Data));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ClearRenderTargetAttr::OnSave()", "Could not write data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3ClearRenderTargetAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3ClearRenderTargetAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::CLEARRENDERTARGET Data;

	Result = i3RenderAttr::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	{
		Rc = pStream->Read( &Data, sizeof(Data));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ClearRenderTargetAttr::OnLoad()", "Could not read data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Color
	i3Color::ConvertRGBA32( Data.m_Color, &m_ClearColor);

	// Z
	m_ClearZ = Data.m_ClearZ;

	// Stencil
	m_ClearStencil = Data.m_ClearStencil;

	// Add Mask
	m_AddMask = Data.m_AddMask;

	// Remove Mask
	m_RemoveMask = Data.m_RemoveMask;

	return Result;
}
