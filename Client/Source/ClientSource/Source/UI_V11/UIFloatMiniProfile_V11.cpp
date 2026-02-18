#include "pch.h"
#include "UIFloatMiniProfile_V11.h"
#include "UI/UIUtil.h"

#include "UI/UIMainFrame_V11.h"
#include "UITopMenu_V11.h"

#include "../GameGUI.h"
#include "../Record.h"
#include "UserInfoContext.h"
#include "GameContextUtil.h"
#include "GameLoginContext.h"

#include "AttendanceContext.h"
#include "LatinAmericaNationInfo.h"

I3_CLASS_INSTANCE( UIFloatMiniProfile_V11);

UIFloatMiniProfile_V11::UIFloatMiniProfile_V11()
{
	m_rUpdateTime = 0.f;

	m_AlignedPos = UIFLOATING_POS_LEFTBOTTOM;

#if defined( I3_DEBUG)
	SetName( "MiniProfile");
#endif

	m_ui8NationMarkShapeIdx = 0;
}

UIFloatMiniProfile_V11::~UIFloatMiniProfile_V11()
{
}

void UIFloatMiniProfile_V11::_SetClanMark(UINT32 ClanMark)
{
	GameUI::SetClanTexture(m_pImgBox, CLAN_MARK_SMALL, ClanMark);
}

void UIFloatMiniProfile_V11::UpdateUserInfo( void)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	i3::rc_wstring		wstrText;
	//char szTemp[ MAX_STRING_COUNT];
	//char szExpText[ MAX_STRING_COUNT];

	LuaState * L;

	if( IsReadyToUpdate() == false)return;

	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	const USER_INFO_RECORD* pRecord = UserInfoContext::i()->GetMyUserInfoRecord();	
	
	USER_INFO_INVITEM_DATA MyUserInfoInvItemData;
	UserInfoContext::i()->GetMyUserInfoInvItemData( &MyUserInfoInvItemData );

	// 닉네임 컬러
	const COLOR * pColor = GameGUI::GetColor(GCT_DEFAULT);
	if( MyUserInfoInvItemData._ui8NickColor != 0)
	{
		pColor = GetColorCashItem( MyUserInfoInvItemData._ui8NickColor );
	}

	L = _CallLuaFunction("SetCouponButton");
	if (L != nullptr)
	{
		i3Lua::PushBoolean(L, LocaleValue::Enable("EnableCoupon"));

		_EndLuaFunction(L, 1);
	}

	UINT8 nationCode = UserInfoContext::i()->GetNationalCode();
	m_ui8NationMarkShapeIdx = LatinAmericaNationInfo::i()->FindNationShapeIdx(nationCode);

	L = _CallLuaFunction( "SetInfoDefault");
	if( L != nullptr)
	{
		if (MyUserInfoInvItemData._wAprilFoolRank != RANK_NONE)
			i3Lua::PushInteger( L, MyUserInfoInvItemData._wAprilFoolRank);
		else if (MyUserInfoInvItemData._wRank != RANK_NONE)
			i3Lua::PushInteger( L, MyUserInfoInvItemData._wRank);
		else
			i3Lua::PushInteger( L, MyUserInfoBasic.m_ui32Rank);

		if(MyUserInfoBasic.m_ui32Rank < RANK_GAMEMASTER)
			i3Lua::PushInteger( L, MyUserInfoBasic.m_ui32Rank + 1);	// 경험치바 rank
		else
			i3Lua::PushInteger( L, MyUserInfoBasic.m_ui32Rank);	// 경험치바 rank

		i3Lua::PushStringUTF16To8( L, MyUserInfoBasic.m_strNick);

		if( i3::generic_string_size( MyUserInfoBasic.m_strClanName ) > 0)
			i3Lua::PushStringUTF16To8( L, MyUserInfoBasic.m_strClanName );
		else
			i3Lua::PushStringUTF16To8( L, GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"));	// clan

		i3Lua::PushColor( L, pColor);

		if( MyUserInfoBasic.m_ui8ESportID == ESPORT_ID_LEVEL_ZERO )
			i3Lua::PushBoolean( L, false);
		else
			i3Lua::PushInteger( L, true);

		// 국기 마크
		if (LocaleValue::Enable("EnableNationMark") == TRUE)
		{
			i3Lua::PushInteger(L, m_ui8NationMarkShapeIdx);
		}
		else
		{
			i3Lua::PushInteger(L, -1);
		}
		_EndLuaFunction( L, 10);
	}

	_SetClanMark( MyUserInfoBasic.m_ui32ClanMark );
	_SetClanMarkPosition();
	// 시즌전적
	L = _CallLuaFunction( "SetScore");
	if( L != nullptr)
	{
		#ifdef USE_MODE_RECORD
			i3Lua::PushInteger( L, pRecord->_season_record._match);
			i3Lua::PushInteger( L, pRecord->_season_record._win);
			i3Lua::PushInteger( L, pRecord->_season_record._lose);
		#else
			i3Lua::PushInteger( L, pRecord->_smatch);
			i3Lua::PushInteger( L, pRecord->_swin);
			i3Lua::PushInteger( L, pRecord->_slose);
		#endif
		_EndLuaFunction( L, 3);
	}

	// 시즌 킬/데스
	{	
#ifdef USE_MODE_RECORD
		INT32 killRate = g_pGameContext->getKillRate(pRecord->_season_record._killcount, pRecord->_season_record._death);
		
		L = _CallLuaFunction( "SetKillDeath");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, killRate);
			i3Lua::PushInteger( L, pRecord->_season_record._killcount);
			i3Lua::PushInteger( L, pRecord->_season_record._AssistCount);
			i3Lua::PushInteger( L, pRecord->_season_record._death);
			_EndLuaFunction( L, 4);
		}
#else
		INT32 killRate = GameContextUtil::getKillRate( pRecord->_skillcount, pRecord->_sdeath );

		L = _CallLuaFunction( "SetKillDeath");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, killRate);
			i3Lua::PushInteger( L, pRecord->_skillcount);
			i3Lua::PushInteger( L, pRecord->_sAssistCount);
			i3Lua::PushInteger( L, pRecord->_sdeath);
			_EndLuaFunction( L, 4);
		}
#endif	
		
	}

	// HeadShot
	{
		INT32 nHeadshot = SeasonHeadShotPercent(pRecord);

		//sprintf_s( szRate, GAME_STRING("STR_TBL_PROFILE_HEADSHOT_RATE_MODE1"), nHeadshot);
		i3::sprintf( wstrText, L"%d%%", nHeadshot );

		L = _CallLuaFunction( "SetHeadrate" );
		if( L != nullptr)
		{
			i3Lua::PushStringUTF16To8( L, wstrText );
			_EndLuaFunction( L, 1);
		}
	}

	/*
	switch( MyUserInfoBasic._eSportID)
	{
	case ESPORT_ID_LEVEL_ZERO:	// 유저의 E-Sport 태그
		{
			m_pESport->SetEnable( true);
			m_pESport->SetShapeEnable(0, true);
		}
		break;
	default:
		{
			m_pESport->SetEnable( false);
		}
		break;
	}
	*/

	/*if (MyUserInfoBasic._eSportID == ESPORT_ID_LEVEL_ZERO)
	{
		m_pESport->SetEnable( false);
	}
	else
	{
		m_pESport->SetEnable( true);
		m_pESport->SetShapeEnable(0, true);
	}*/

	{	
		UINT32 wRank = UserInfoContext::i()->GetRealRank();

		INT32 nCurrentExp = MyUserInfoBasic.m_ui32Exp - __GetExpMax(wRank);
		nCurrentExp = MAX(nCurrentExp, 0);	// 음수값은 0
		INT32 nExpRate = 100;

		// 상대평가라도 목표 경험치 추가
		if(GameUI::is_hero_rank(wRank, true) == true)
		{
			i3::sprintf(wstrText, L"%d (100%%)   ", MyUserInfoBasic.m_ui32Exp);
		}
		else
		{
			INT32 nMaxExp = GameUI::GetMaxExp(wRank + 1) - GameUI::GetMaxExp(wRank);
			//if( nMaxExp != 0) nExpRate = (nCurrentExp * 100) / nMaxExp;

			//nExpRate = MINMAX(0, nExpRate, 100);	// 100%가 넘을 경우 100%으로 고정 출력
			nExpRate = GameUI::GetExpRate(wRank, nCurrentExp);

			i3::sprintf( wstrText, L"%d/%d (%d%%)  ", nCurrentExp, nMaxExp, nExpRate);
		}

		L = _CallLuaFunction("SetExpGauge");
		if (L != nullptr)
		{
			i3Lua::PushNumber(L, nExpRate * 0.01f);
			_EndLuaFunction(L, 1);
		}

		L = _CallLuaFunction("SetExpText");
		if (L != nullptr)
		{
			i3Lua::PushStringUTF16To8(L, wstrText);
			_EndLuaFunction(L, 1);
		}
	
		L = _CallLuaFunction( "SetCash");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, MyUserInfoBasic.m_ui32Point );
	#if defined (LOCALE_KOREA) && 0
			//N-Coin & 해피코인는 한국 NC만 사용
			//sprintf_s( strBuf, "%d\n %d", MyUserInfoBasic._NCCoin, MyUserInfoBasic._HappyCoin);
			i3Lua::PushInteger( L, MyUserInfoBasic._NCCoin);
			i3Lua::PushInteger( L, MyUserInfoBasic._HappyCoin);
	#else
			i3Lua::PushInteger( L, MyUserInfoBasic.m_ui32Cash);
			i3Lua::PushInteger( L, 0);
	#endif
			_EndLuaFunction( L, 3);
		}
	}
}

INT32 UIFloatMiniProfile_V11::__GetExpMax(INT32 nRank)
{
	I3_BOUNDCHK( nRank, RANK_MAX);

	if (nRank >= 0 && nRank < RANK_51+1)
	{
		return g_ExpValueV1[nRank];
	}
	else
	{
		return 0;
	}
}

void UIFloatMiniProfile_V11::_SetClanMarkPosition()
{
	i3UIText * t_Text = m_pClanName->getUIText();
	INT32 cLength = t_Text->getExtentWidth();


	REAL32 rTextRight = m_pClanName->getPos()->x + m_pClanName->getWidth();
	REAL32 rClanMarkWidth = m_pImgBox->getWidth();

	// 기본 ImgBox의 X 위치값
	REAL32 rImgBoxPosX = rTextRight - rClanMarkWidth - cLength - 5;

	m_pImgBox->setPos( rImgBoxPosX , getY( m_pImgBox->getPos()));//보정값 10
}

extern "C" 
{
	int cClick( LuaState * L)
	{
		// 추후 수정 부탁 드립니다. 20150518 김현우

		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

		if( i3UI::getDisableUIInput() == true)
			return 1;

		if( pFrame->IsEnteredSubPhase(USP_PROFILE) )
			pFrame->ExclusedEnterSubPhase( USP_NONE);
		else
		{
			pFrame->ExclusedEnterSubPhase( USP_PROFILE );			
		}

		return 1;
	}

	int UIFloatMiniProfile_ClickInputCoupon(LuaState * L)
	{

		// 쿠폰 입력 팝업 리팩토링 필요
		/*UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

		if (pFrame != nullptr)
		{
		pFrame->TogglePopup(UPW_COUPON_INPUT);
		}*/

		return 0;
	}

	int UIFloatMiniProfile_AttendanceClick(LuaState * L)
	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

		if( pFrame != nullptr)
		{
			INT32 EvtType = i3Lua::GetIntegerArg(L, 1); 
			UIFloatMiniProfile_V11 * pThis = (UIFloatMiniProfile_V11 *)pFrame->GetFloating(UFW_MINIPROFILE);
			if(pThis != nullptr)
			{
				pThis->AttendanceOpenPopup(EvtType);
			}
		}

		return 0;
	}
}

LuaFuncReg UIFloatMiniProfile_Glue[] =
{
	{"ClickInfo",			cClick},
	{"AttendanceClick",		UIFloatMiniProfile_AttendanceClick },
	{"ClickInputCoupon",	UIFloatMiniProfile_ClickInputCoupon },
	{nullptr,				nullptr}
};

/*virtual*/ void UIFloatMiniProfile_V11::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Popup/PointBlankRe_MyInfo.i3UIs", UIFloatMiniProfile_Glue);
}

/*virtual*/ void UIFloatMiniProfile_V11::OnUpdate( REAL32 rDeltaSeconds)
{
	I3ASSERT( isEnable() );

	UIFloatingBase::OnUpdate( rDeltaSeconds);

	m_rUpdateTime -= rDeltaSeconds;

	if( m_rUpdateTime <= 0.f)
	{
		m_rUpdateTime = 1.f;
	}
}

/*virtual*/ bool UIFloatMiniProfile_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;


	OnSlideIn();

	if ((BOOL)LocaleValue::Enable("EnableNationMark") == TRUE)
	{
		m_pNationMark->SetVisible(TRUE);
	}

	UpdateUserInfo();
	
	return true;
}

/*virtual*/ void UIFloatMiniProfile_V11::_InitializeAtLoad(i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad(pScene);
	m_pClanName = static_cast<i3UIStaticText*>(pScene->FindChildByName("i3UIStaticText11"));
	I3ASSERT(m_pClanName);

	m_pImgBox	= static_cast<i3UIImageBoxEx*>( pScene->FindChildByName("i3UIClanMark") );
	I3ASSERT(m_pImgBox);

	m_pNationMark = static_cast<i3UIButtonImageSet*>(pScene->FindChildByName("i3UIButtonImageSet0"));
	I3ASSERT(m_pNationMark);

	m_pNationMark->SetVisible(FALSE);
}

void	UIFloatMiniProfile_V11::OnLoadAllScenes()
{
	UIFloatingBase::OnLoadAllScenes();

	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::L_TO_R);
}

/*virtual*/ bool UIFloatMiniProfile_V11::OnExitStart( void)
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	OnSlideOut();

	return true;
}

void UIFloatMiniProfile_V11::ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data & UserData)
{
	switch (evt)
	{
		// 클랜 생성, 가입 허용, 강제 제명, 탈퇴, 해체시 개인정보 갱신
	case	EVENT_CLAN_CREATE:
	case	EVENT_CLAN_SECESSION:
	case	EVENT_M_CLAN_MEMBER_INFO:
	case	EVENT_CLAN_DISSOLUTION:
		// Cash가 Refresh가 될 때 마다 갱신 해줘야 함. 어휴
	case	EVENT_REFRESH_CASH:
		UpdateUserInfo();
		break;

	case	EVENT_CREATE_NICK_NAME: // 첫 닉네임 설정시 정보 갱신
	{
		if (EV_SUCCESSED(arg))
		{

			UserInfoContext::i()->SetMyUserInfoRecord()->Reset();
			UserInfoContext::i()->SetMyNickName(GameLoginContext::i()->getUserNick());

			UpdateUserInfo();
		}
	}
	break;

	case	EVENT_M_CLAN_DEPROTATION:
	{
		USER_INFO_BASIC		MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
		MyUserInfoBasic.m_ui32Clanidx = 0;
		MyUserInfoBasic.m_ui32ClanState = CLAN_MEMBER_LEVEL_UNKNOWN;
		MyUserInfoBasic.m_ui32ClanMark = CLAN_MARK_DEFAULT;
		i3mem::FillZero(MyUserInfoBasic.m_strClanName, sizeof(MyUserInfoBasic.m_strClanName));
		UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
		UpdateUserInfo();
	}
	break;

	default:
		break;
	}
}

void UIFloatMiniProfile_V11::AttendanceOpenPopup(INT32 EvtType)
{
	if (AttendanceContext::i()->IsAttendanceEvent((ATTENDANCE_TYPE)EvtType) &&
		AttendanceContext::i()->IsEventTarget((ATTENDANCE_TYPE)EvtType))
	{
		switch (EvtType)
		{
		case ATTENDANCE_TYPE_DORMANT:
			GetMainFrame()->OpenPopup(UPW_DORMANT_EVENT);
			break;
		case ATTENDANCE_TYPE_NORMAL:
			GetMainFrame()->OpenPopup(UPW_MONTH_ATTENDANCE);
			break;
		default:
			I3PRINTLOG(I3LOG_NOTICE, "Unregist Attendance Event. %d", EvtType);
			break;
		}
	}
	else
	{
		i3::rc_wstring wstrString;

		// 이벤트가 없는 경우와 이벤트 대상이 아닌 경우. 이벤트 존재 유무룰 먼저 검사해야 합니다 
		if (!AttendanceContext::i()->IsAttendanceEvent((ATTENDANCE_TYPE)EvtType))
		{
			switch (EvtType)
			{
			case ATTENDANCE_TYPE_DORMANT:
				wstrString = GAME_RCSTRING("STR_TBL_UI_DORMANT_NOT_ATTENDANCE");
				break;
			case ATTENDANCE_TYPE_NORMAL:
				wstrString = GAME_STRING("STR_TBL_UI_ATTENDANCE_NOT_ATTENDANCE");
				break;
			default:
				I3PRINTLOG(I3LOG_NOTICE, "Unregist Attendance Event. %d", EvtType);
				break;
			}
		}
		else if (!AttendanceContext::i()->IsEventTarget((ATTENDANCE_TYPE)EvtType))
		{
			switch (EvtType)
			{
			case ATTENDANCE_TYPE_DORMANT:
				wstrString = GAME_RCSTRING("STR_TBL_UI_DORMANT_NOTUSER_ATTENDANCE");
				break;
			default:
				I3PRINTLOG(I3LOG_NOTICE, "Unregist Attendance Event. %d", EvtType);
				break;
			}
		}

		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrString, &GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_TEXT"));
	}
}