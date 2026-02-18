#include "i3GuiPCH.h"

#include "i3GuiMovie.h"

// ============================================================================
//
// i3GuiMovie : 버튼
//
// ============================================================================

I3_CLASS_INSTANCE( i3GuiMovie);

i3GuiMovie::i3GuiMovie()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);

}

i3GuiMovie::~i3GuiMovie()
{
	I3TRACE("Movie Release\n\n");
}

void i3GuiMovie::SetMovie( char* szFileName )
{
	/*
	// 우선은 GuiControl 을 이용하지 않고 직접 세팅 하겠습니다.
	// 통상적인 데이타가 아니라 불필요한 작업이 많아서..
	I3_SAFE_RELEASE( m_pMovie );

	m_pMovie = i3GfxMovie::new_object();
	m_pMovie->InitDShowTextureRenderer( szFileName );

	if(m_pSprite == nullptr) {
		m_pSprite = i3Sprite2D::new_object();
		m_pSprite->Create( 1, true, true );
	}
	m_pSprite->SetTexture( m_pMovie->GetTexture() );
	m_pSprite->SetEnable( 0, true );
	m_pSprite->SetRect( 0, 0, 0, (REAL32)m_pMovie->GetVidWidth(), (REAL32)m_pMovie->GetVidHeight() );
	m_pSprite->SetColor( 0, 255, 255, 255, 255 );
	//	동영상이 넘어올때는 상하가 바뀌어 오기때문에 (0, y, x, 0) 으로 세팅
	m_pSprite->SetTextureCoord( 0, 0, (REAL32)m_pMovie->GetVidHeight(), (REAL32)m_pMovie->GetVidWidth(), 0 );

	GetNode()->AddChild((i3Node*)m_pSprite);
	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	*/
}

void i3GuiMovie::SetMovieRect( REAL32 x, REAL32 y, REAL32 width, REAL32 height )
{
	m_pSprite->SetRect( 0, x, y, width, height );
}

void i3GuiMovie::SetMovieColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a )
{
	m_pSprite->SetColor( 0, r, g, b, a );
}

void i3GuiMovie::SetMovieEnable( bool bFlag )
{
	m_pSprite->SetEnable( 0, bFlag );
}

void i3GuiMovie::SetMovieMute( bool bFlag )
{
	//m_pMovie->SetMute( bFlag );
}

void	i3GuiMovie::SetMovieRate( double dRateAdjust )
{
	//m_pMovie->SetRate( dRateAdjust );
}

void i3GuiMovie::OnUpdate( REAL32 rDeltaSeconds )
{
	//if(m_pMovie) m_pMovie->CheckMovieStatus();

	i3GuiControl::OnUpdate( rDeltaSeconds );
}

void i3GuiMovie::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{	
}

void i3GuiMovie::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct GUIMOVIE
	{
		INT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC)
#if defined(PACKING_RIGHT)
	#pragma pack(pop)						// [test required] 중복 push입니다. pop으로 수정. 2017.02.09
#else
	#pragma pack(push, 4)
#endif
#endif

UINT32 i3GuiMovie::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	i3Stream * pStream = pResFile->GetStream();
	
	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	pack::GUIMOVIE data;
	data.m_State = getControlState();

	Rc = pStream->Write(&data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiMovie::OnSave()", "Could not save button");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiMovie::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIMOVIE data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiMovie::OnLoad()", "Could not read Button");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}
