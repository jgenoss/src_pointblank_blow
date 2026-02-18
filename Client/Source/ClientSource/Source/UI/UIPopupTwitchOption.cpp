#include "pch.h"
#include "UIPopupTwitchOption.h"

#include "UIMainFrame.h"
#include "UIBattleFrame.h"
#include "UIUtil.h"

#include "TwitchTVContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/compare/generic_compare.h"

I3_CLASS_INSTANCE( UIPopupTwitchOption );

namespace
{
	UIPopupTwitchOption* g_this = 0;
	enum { START, ACCEPT, CANCEL };
	enum { FRAMECB, VIDEOCB };
}

extern "C" 
{
	int option_click_twich( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked( value );
		return 0;
	}

	int list_click_twich( LuaState * L)
	{
		INT32 value1 = i3Lua::GetIntegerArg( L, 1);
		INT32 value2 = i3Lua::GetIntegerArg( L, 2);
		g_this->clickedlist( value1, value2 );
		return 0;
	}

	int volume_click_twich( LuaState * L)
	{
		INT32 value1 = i3Lua::GetIntegerArg( L, 1);
		INT32 value2 = i3Lua::GetIntegerArg( L, 2);
		g_this->clickedvolume( value1, value2 );
		return 0;
	}
}

LuaFuncReg option_twich_glue[] =
{
	{ "Click",				option_click_twich	},
	{ "ClickList",			list_click_twich	},
	{ "ClickVolume",		volume_click_twich	},
	{ nullptr,							nullptr}
};

void UIPopupTwitchOption::clicked(INT32 idx)
{
	switch(idx)
	{
	case START:
		Apply_Game();
		TwitchTVBroadCasteProcess();
		break;
	case ACCEPT:
		Apply_Game();
		break;
	case CANCEL:
		break;
	}

	if( (idx != START) )
	{
		if(!g_pFramework->IsBattleStage())
		{
			GetMainFrame()->ClosePopup(UPW_TWITCHOPTION);
		}
		else
		{
			UIBattleFrame::i()->ClosePopup(UBP_TWITCHOPTION);
		}
	}
}

void UIPopupTwitchOption::clickedlist(INT32 cbidx, INT32 idx)
{
	switch(cbidx)
	{
	case FRAMECB:
		m_nFrameIndex = idx;
		break;
	case VIDEOCB:
		m_nVideoIndex = idx;
		break;
	}
	
	Apply_Lua();
}

void UIPopupTwitchOption::clickedvolume(INT32 volidx, INT32 idx)
{
	switch(volidx)
	{
	case MIKE_VOL: case SPEAKER_VOL:
		{
			bool enable = (idx) ? false : true;
			mVolume[volidx].enable = enable;
			mVolume[volidx].edit.Enable( enable );
		}
		break;
	}

	Apply_Lua();
}

UIPopupTwitchOption::UIPopupTwitchOption() : m_nFrameIndex(0), m_nVideoIndex(0), m_bInitTitleBox(false)
{
	g_this = this;

	m_Callback1.SetObj(this);
	mVolume[MIKE_VOL].edit.SetCallback(&m_Callback1);
	mVolume[MIKE_VOL].edit.SetRange(0, 100);
	
	m_Callback2.SetObj(this);
	mVolume[SPEAKER_VOL].edit.SetCallback(&m_Callback2);
	mVolume[SPEAKER_VOL].edit.SetRange(0, 100);

	m_pThread = nullptr;
}

UIPopupTwitchOption::~UIPopupTwitchOption()
{
	if(m_pThread != nullptr)
	{
		m_pThread->Stop();
		m_pThread->Destroy();
		I3_SAFE_RELEASE_NODBG(m_pThread);
	}
}

/**********************//**********************/
/*virtual*/ void UIPopupTwitchOption::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/Popup/PBRe_PopUp_Twitch_TV_Main.i3UIs", option_twich_glue);
}

/*virtual*/ void UIPopupTwitchOption::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	TwitchTVContext* info = TwitchTVContext::i();
	mVolume[MIKE_VOL].enable = info->isMikeOn();
	mVolume[MIKE_VOL].vol	 = info->GetMikeVolume();
	mVolume[SPEAKER_VOL].enable = info->isSpeakerOn();
	mVolume[SPEAKER_VOL].vol	= info->GetSpeakerVolume();

	UIPopupBase* parent = g_this;
	mVolume[MIKE_VOL].edit.SetControl( parent, pScene, "MikeProgressBar", "MikeProgressBut", "MikeEditBox" );
	mVolume[MIKE_VOL].edit.Enable( mVolume[MIKE_VOL].enable);
	mVolume[SPEAKER_VOL].edit.SetControl( parent, pScene, "SpeakerProgressBar", "SpeakerProgressBut","SpeakerEditBox" );
	mVolume[SPEAKER_VOL].edit.Enable( mVolume[SPEAKER_VOL].enable);

	m_nFrameIndex = info->GetFrame();
	m_nVideoIndex = info->GetVideoGrade();

	//제목설정 작업.

	m_pTitleBox = (i3UIEditBox*) pScene->FindChildByName( "BroadCasteEditBox");
	I3ASSERT( m_pTitleBox != nullptr);
	m_pTitleBox->setAlphaNumericOnly(true);
	m_pTitleBox->SetLetterLimit( 50 );
	m_pTitleBox->SetText(info->GetBroadTitle());
	
	//프레임, 비디오 콤보 박스 전투중일때는 선택 불가 모드로 하고 검은 이미지 활성화 시킨다.
	m_pFrameComboBox = (i3UIComboBox *) pScene->FindChildByName( "FrameComboBox");
	m_pVideoComboBox = (i3UIComboBox *) pScene->FindChildByName( "VideoComboBox");
	
	if(info->isBroadCasting())
	{
		m_pFrameComboBox->addState(I3UI_STATE_DEACTIVATED);
		m_pFrameComboBox->setInputDisable(true);
		m_pFrameComboBox->setModifyState();
		m_pVideoComboBox->addState(I3UI_STATE_DEACTIVATED);
		m_pVideoComboBox->setInputDisable(true);
		m_pVideoComboBox->setModifyState();
	}
	LuaState * L = 0;

	L = _CallLuaFunction("ListDisable");
	i3Lua::PushBoolean( L , info->isBroadCasting() );
	UILua::EndLuaFunction( L , 1);

	//방송 시작, 종료 버튼 적용.

	L = _CallLuaFunction("BroadCasteButttonDisable");
	if( g_pFramework->IsBattleStage() )
		i3Lua::PushBoolean( L , !(TwitchTVContext::i()->isBroadCasting()) );
	else
		i3Lua::PushBoolean( L , false );
	UILua::EndLuaFunction( L , 1);
	
	//
	m_pFrameWnd = (i3UIFrameWnd *) pScene->FindChildByName("i3UIFrameWnd0");
	
}

/*virtual*/ void UIPopupTwitchOption::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);
}

/*virtual*/ void UIPopupTwitchOption::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();

	Apply_Lua();

	//GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_CART_OVERLAP_ERROR"), nullptr, MAKE_CALLBACK(cb_close_duplicate_popup), this) ;
}

/*virtual*/ void UIPopupTwitchOption::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();
}

/*virtual*/ void UIPopupTwitchOption::OnUnloadAllScenes()
{
	UIPopupBase::OnUnloadAllScenes();
}

/*virtual*/ void UIPopupTwitchOption::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);

	if(isVisible() == false) return;

	if( m_pLoadIcon && m_pThread )
	{
		if( m_pThread->m_iState == THREAD_PROCESS_FINISH)
		{
			GameUI::DisableLoadingIcon( m_pLoadIcon );

			bool result = false;
			if( m_pThread->m_bStart)
			{
				result = m_pThread->m_bStartResult;
				if( result )
					GameUI::MsgBox(MSG_TYPE_GAME_OK,GAME_STRING("STBL_POPUP_TWITCH_BROADCAST_START_GUIDE"));
				else
					GameUI::MsgBox(MSG_TYPE_GAME_OK,GAME_STRING("STBL_POPUP_TWITCH_BROADCAST_START_FAIL"));
				m_pThread->m_bStart = false;
			}
			if( m_pThread->m_bStop)
			{
				result = m_pThread->m_bStopResult;
				if( result )
					GameUI::MsgBox(MSG_TYPE_GAME_OK,GAME_STRING("STBL_POPUP_TWITCH_BROADCAST_END_GUIDE"));
				else
					GameUI::MsgBox(MSG_TYPE_GAME_OK,GAME_STRING("STBL_POPUP_TWITCH_BROADCAST_END_FAIL"));
				m_pThread->m_bStop = false;
			}

			m_pThread->Stop();
			m_pThread->Destroy();
			I3_SAFE_RELEASE_NODBG(m_pThread);

			if( result )
			{
				if(!g_pFramework->IsBattleStage())
				{
					GetMainFrame()->ClosePopup(UPW_TWITCHOPTION);
				}
				else
				{
					UIBattleFrame::i()->ClosePopup(UBP_TWITCHOPTION);
				}
			}
		}
	}
	else
	{
		for(int i=0; i<MAX_VOL;++i)
			mVolume[i].edit.OnUpdate(rDeltaSeconds);

		if( m_pTitleBox->isFocused() && !m_bInitTitleBox )
		{
			m_pTitleBox->SetText(L"");
			m_pTitleBox->SetIMEText(L"");
			TwitchTVContext::i()->SetBroadTitle(L"");
			m_bInitTitleBox = true;
		}
	}
}

/*virtual*/ bool UIPopupTwitchOption::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	return UIPopupBase::OnEvent( event, pObj, param2, code);
}

/*virtual*/ bool UIPopupTwitchOption::OnKey_Enter( void)
{
	return false;
}
/**********************//**********************/

void UIPopupTwitchOption::Apply_Game()
{
	//입력된 정보를 TwitchTVContext에 갱신하는 함수.
	TwitchTVContext* info = TwitchTVContext::i();

	const i3::wstring& Title = m_pTitleBox->getText();

	bool bChangeFrameOrVideo = false;
	bool bChangeSound		 = false;
	bool bChangeTitle		 = false;

	if( m_nFrameIndex != info->GetFrame())							bChangeFrameOrVideo = true;
	if( m_nVideoIndex != info->GetVideoGrade())						bChangeFrameOrVideo = true;
	//if(  != info->GetFrame()) bChangeFrameOrVideo = true; 해상도 추가애야됨.
	if( mVolume[MIKE_VOL].vol != info->GetMikeVolume() )			bChangeSound = true;
	if( mVolume[SPEAKER_VOL].vol != info->GetSpeakerVolume() )		bChangeSound = true;
	if( mVolume[MIKE_VOL].enable != info->isMikeOn() )				bChangeSound = true;
	if( mVolume[SPEAKER_VOL].enable != info->isSpeakerOn() )		bChangeSound = true;
	if( i3::generic_compare(info->GetBroadTitle(),Title) )			bChangeTitle = true;

	info->SetFrame(m_nFrameIndex);
	info->SetVideoGrade(m_nVideoIndex);
	info->SetMikeEnable(mVolume[MIKE_VOL].enable);
	info->SetMikeVolume(mVolume[MIKE_VOL].vol);
	info->SetSpeakerEnable(mVolume[SPEAKER_VOL].enable);
	info->SetSpeakerVolume(mVolume[SPEAKER_VOL].vol);
	info->SetBroadTitle(Title);

	if( info->isBroadCasting() )
	{
		if( bChangeSound || bChangeFrameOrVideo || bChangeTitle )
		{
			info->TTVOptionChange(bChangeFrameOrVideo);
		}
	}
}

void UIPopupTwitchOption::Apply_Lua()
{
	//UI에 입력한 정보들 갱신하는 함수.

	LuaState * L = 0;

	TwitchTVContext* info = TwitchTVContext::i();


	//프레임 콤보 박스 적용.
	i3::wstring frame = info->GetFrameInfoString();

	I3_BOUNDCHK( m_nFrameIndex, MAX_FRAME_INDEX);

	L = _CallLuaFunction("SetFrameList");
	i3Lua::PushStringUTF16To8(L, frame.c_str(), frame.size() );
	i3Lua::PushInteger(L, m_nFrameIndex);
	UILua::EndLuaFunction(L , 2);

	//비디오 등급 콤보 박스 적용.
	i3::wstring video = info->GetVideoInfoString();

	I3_BOUNDCHK( m_nVideoIndex, MAX_VIDEO_INDEX);

	L = _CallLuaFunction("SetVideoList");
	i3Lua::PushStringUTF16To8(L, video.c_str(), video.size() );
	i3Lua::PushInteger(L, m_nVideoIndex);
	UILua::EndLuaFunction(L , 2);


	//마이크,스피커 볼륨 적용
	for(int i=0; i<MAX_VOL;++i) 
		mVolume[i].edit.SetNumber(mVolume[i].vol);

	//마이크,스피커 mute 적용
	L = _CallLuaFunction( "SetMute");
	for(int i=0; i<MAX_VOL;++i)
		i3Lua::PushBoolean( L, mVolume[i].enable );
	UILua::EndLuaFunction(L , MAX_VOL);

	//방송 시작, 종료 버튼 적용.
	L = _CallLuaFunction("SetStartEnd");
	i3Lua::PushBoolean( L , info->isBroadCasting());
	UILua::EndLuaFunction( L , 1);
	
}

void UIPopupTwitchOption::cb_edit_change1( INT32 value )
{
	mVolume[MIKE_VOL].vol = value;
	Apply_Lua();
}

void UIPopupTwitchOption::cb_edit_change2( INT32 value )
{
	mVolume[SPEAKER_VOL].vol = value;
	Apply_Lua();
}

void UIPopupTwitchOption::TwitchTVBroadCasteProcess()
{
	//방송시작 함수
	TwitchTVContext* info = TwitchTVContext::i();
	
	m_pThread = TwitchLoadingThread::new_object();
	m_pThread->Create("THREAD_TWITCH");

	if(info->isTwitchLogin() && !info->isBroadCasting() )
	{
		m_pThread->m_bStart = true;
		m_pThread->m_iState = THREAD_PROCESS_START;
		m_pThread->Wakeup();
		m_pLoadIcon = GameUI::EnableLoadingIcon( this, m_pFrameWnd);
	}
	else if( info->isBroadCasting() )
	{
		m_pThread->m_bStop = true;
		m_pThread->m_iState = THREAD_PROCESS_START;
		m_pThread->Wakeup();
		m_pLoadIcon = GameUI::EnableLoadingIcon( this, m_pFrameWnd);
	}
}

I3_CLASS_INSTANCE( TwitchLoadingThread );

TwitchLoadingThread::TwitchLoadingThread()
{
	_setSignalTime(1000);
	m_iState	   = THREAD_PROCESS_FINISH;
	m_bStartResult = false;
	m_bStopResult  = false;
	m_bStart	   = false;
	m_bStop		   = false;
}

TwitchLoadingThread::~TwitchLoadingThread()
{
}

void TwitchLoadingThread::Run(void *pData)
{
	if(m_iState == THREAD_PROCESS_START && m_bStart)
	{
		TwitchTVContext* info = TwitchTVContext::i();
		m_bStartResult = info->TTVStreamStart();
		m_iState = THREAD_PROCESS_FINISH;
	}

	if(m_iState == THREAD_PROCESS_START && m_bStop)
	{
		TwitchTVContext* info = TwitchTVContext::i();
		m_bStopResult = info->TTVStreamStop();
		m_iState = THREAD_PROCESS_FINISH;
	}
}