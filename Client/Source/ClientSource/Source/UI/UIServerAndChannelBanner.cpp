#include "pch.h"
#include "UIServerAndChannelBanner.h"

#include "UIMainFrame.h"
#include "UIUtil.h"

#include "../WebControl.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/remove_all_blank.h"
#include "i3Base/string/compare/generic_compare.h"
#include "SteamContext.h"

#include "GameLoginContext.h"
//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIServerAndChannelBanner_ClickNumber( LuaState * L)
	{
		INT32 slot = i3Lua::GetIntegerArg( L, 1);

		// 해당 번호의 공지를 보여준다.
		UIServerAndChannelBanner * pBanner = static_cast<UIServerAndChannelBanner*>(GameUI::GetFloating( UFW_SCBANNER));
		if( pBanner != nullptr)
		{
			pBanner->AnnounceButton( slot);
		}

		return 0;
	}

	int UIServerAndChannelBanner_ClickBanner( LuaState * L)
	{

		UIServerAndChannelBanner * pBanner = static_cast<UIServerAndChannelBanner*>(GameUI::GetFloating( UFW_SCBANNER));

		if(pBanner != nullptr)
		{
			SHELLEXECUTEINFO sh;
			memset(&sh, 0L, sizeof(SHELLEXECUTEINFO));

			sh.cbSize = sizeof(SHELLEXECUTEINFO);
			sh.lpVerb = "open";

			char * url = pBanner->GetCurrentBannerURL();

		// 북미/Steam 대회신청 팝업 - 배너 url에 SteamLoginWebkey를 GET방식으로 넣어 전달합니다.

			if (i3::generic_string_size(GameLoginContext::i()->getWebLoginKey()) > 0)
			{
				char strURL[1024] = { 0, };

				i3::generic_string_copy(strURL, url);
				if (i3::generic_is_iequal(strURL, "http://us.fps-pb.com/ccn/steam/gw.do"))		// Live용 url 
				{
					char strParam[1024] = { 0, };
					i3::snprintf(strParam, 1024, "?s=%s&l=tournament", GameLoginContext::i()->getWebLoginKey());
					i3::generic_string_cat(strURL, strParam);

					// SteamAPI 사용시 별도의 웹 브라우저를 띄우기 때문에 이 부분에서 return 해줍니다.
					if (SteamContext::i()->IsSteamUser())
					{
						if (SteamContext::i()->ActivateWebPage(strURL))
							return 0;
					}
				}

			}
			
			if (SteamContext::i()->IsSteamUser())
			{
				if (SteamContext::i()->ActivateWebPage(url))
					return 0;
			}

			if(url == nullptr)
				return 0;

			i3::remove_all_blank(url);

			if(i3::generic_compare(url, "") != 0)
				sh.lpFile = url;
			else
				return 0;

			sh.nShow = SW_NORMAL;

			if(!ShellExecuteEx(&sh))
			{
				DWORD dw = GetLastError();

				char errorbuffer[256];

				::FormatMessage(
					FORMAT_MESSAGE_FROM_SYSTEM,NULL,
					dw,0,
					(LPTSTR)errorbuffer,0,NULL);

				I3PRINTLOG(I3LOG_DEFALUT, errorbuffer);
			}

		}


		return 0;
	}


	int UIServerAndChannelBanner_ClickEsportsBanner(LuaState * L)
	{
		UIServerAndChannelBanner * pBanner = static_cast<UIServerAndChannelBanner*>(GameUI::GetFloating(UFW_SCBANNER));

		if (pBanner != NULL)
		{
			SHELLEXECUTEINFO sh;
			memset(&sh, 0L, sizeof(SHELLEXECUTEINFO));

			sh.cbSize = sizeof(SHELLEXECUTEINFO);
			sh.lpVerb = "open";

			char * url = const_cast<char*>(pBanner->GetEsportsBannerURL());

			// 북미/Steam 팝업
			if (SteamContext::i()->IsSteamUser())
			{
				if (SteamContext::i()->ActivateWebPage(url))
					return 0;
			}

			if (url == NULL)
				return 0;

			i3::remove_all_blank(url);

			if (i3::generic_compare(url, "") != 0)
				sh.lpFile = url;
			else
				return 0;

			sh.nShow = SW_NORMAL;

			if (!ShellExecuteEx(&sh))
			{
				DWORD dw = GetLastError();

				char errorbuffer[256];

				FormatMessage(
					FORMAT_MESSAGE_FROM_SYSTEM, NULL,
					dw, 0,
					(LPTSTR)errorbuffer, 0, NULL);

				I3PRINTLOG(I3LOG_DEFALUT, errorbuffer);
			}

		}


		return 0;
	}
}

LuaFuncReg UIServerAndChannelBanner_Glue[] = {
	{"ClickButton",				UIServerAndChannelBanner_ClickNumber},
	{"BannerImageClick",		UIServerAndChannelBanner_ClickBanner},
	{"EsportsBannerClick",		UIServerAndChannelBanner_ClickEsportsBanner},
	{nullptr,						nullptr}
};

//-----------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIServerAndChannelBanner);//, UIFloatingBase);

UIServerAndChannelBanner::UIServerAndChannelBanner() : m_bNoImage(false), m_pImageBox(nullptr)
	, m_nNoticeIndex(-1), m_fLocalNoticeTime(0.f), m_bEnableNoticeText(true), m_bClickedNoticeCaption(false)
	, m_fAnnounceFilpElapsedTime(0.f), m_fAnnounceFlipTime(0.f), m_nCurrentSelectedIndex(0)
	, m_hWebAnnounce(nullptr), m_pVersionText(nullptr), m_bWebError(false), m_pImgWebError(nullptr)
{
	m_AlignedPos = UIFLOATING_POS_LEFTBOTTOM;
	
	::memset(m_pAnnounceTexture, 0, sizeof(i3Texture*) * MAX_ANNOUNCE);

#if defined( I3_DEBUG)
	SetName( "ServerAndChannelBanner");
#endif
}

UIServerAndChannelBanner::~UIServerAndChannelBanner()
{
	INT32 i;

	for( i = 0; i < MAX_ANNOUNCE; i++)
	{
		I3_SAFE_RELEASE( m_pAnnounceTexture[i]);
	}

	if (m_hWebAnnounce)
	{
		::DestroyWindow(m_hWebAnnounce);
		m_hWebAnnounce = nullptr;
		UninitWebCtrl();
	}
}

void UIServerAndChannelBanner::__Init( void)
{
	INT32 i;

	I3ASSERT( m_pFrameWnd != nullptr);

	m_pImageBox = (i3UIImageBoxEx*) m_pFrameWnd->FindChildByName( "i3UIImageBoxEx");
	I3ASSERT( m_pImageBox);

	VEC2D vOriginSize;
	
	i3Vector::Copy(& vOriginSize , m_pImageBox->getSize());

	// 이미지 공지 읽어오기
	m_ButtonCount = g_pFramework->GetAnnounceCount();
	I3ASSERT(m_ButtonCount <= MAX_ANNOUNCE);

	if (0 < m_ButtonCount)
	{
		// 버튼 갯수 맞추기
		LuaState * L = _CallLuaFunction(m_strLuaContextName.c_str(), "SetButtonCount");
		if (L != nullptr)
		{
			i3Lua::PushInteger(L, m_ButtonCount);
			_EndLuaFunction(L, 1);
		}

		for (i = 0; i < m_ButtonCount; i++)
		{
			const char* pURL = g_pFramework->GetAnnounceURL(i);
			const char* pPath = g_pFramework->GetAnnouncePath(i);
			bool scroll = g_pFramework->IsAnnounceScroll(i) ? true : false;

			// 공지사항용 인덱스를 저장합니다.
			if (i3::generic_is_equal(pURL, "알림"))
			{
				m_nNoticeIndex = i;
			}

			I3_SAFE_RELEASE(m_pAnnounceTexture[i]);
			m_pAnnounceTexture[i] = g_pFramework->GetResourceManager()->LoadTexture(pPath);

			if (m_pAnnounceTexture[i] != nullptr)
			{
				m_pAnnounceURL[i] = (char*)pURL;
				m_bAnnounceScroll[i] = scroll;
				m_pImageBox->AddTexture(m_pAnnounceTexture[i], 0, 0, 380, 478);
			}
		}

	}
	else
	{
		// 공지설정 없음
		m_bNoImage = true;
	}

	m_pImageBox->CreateBaseSceneObjects();
	m_pImageBox->SetShapeSize(vOriginSize.x , vOriginSize.y);

	if(m_ButtonCount > 0)
		AnnounceButton(0);

	_SetEsportsBanner();
}

/*virtual*/ void UIServerAndChannelBanner::_InitializeAtLoad( i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);

	m_pFrameWnd = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd_SACB");
	I3ASSERT( m_pFrameWnd != nullptr);

	if(m_pVersionText == nullptr)
	{
		m_pVersionText = (i3UIStaticText*)pScene->FindChildByName("Static_VersionText");
	}

	m_pImgWebError = static_cast<i3UIImageBox*>(pScene->FindChildByName("i3UIImageBox0"));
	I3ASSERT(m_pImgWebError != nullptr);
	if(m_pImgWebError != nullptr)
		m_pImgWebError->EnableCtrl(false);
	
	i3UITab * temp = (i3UITab*)pScene->FindChildByName("i3UITab");
	vecTabList.push_back(temp);

	for(INT32 i = 0; i < 5; i++)
	{
		i3::string tabname;
		i3::sprintf(tabname, "i3UITab%d", i);
		temp = (i3UITab*)pScene->FindChildByName(tabname.c_str());
		vecTabList.push_back(temp);
	}

	//m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::L_TO_R);
}

/*virtual*/ void UIServerAndChannelBanner::_ClearAtUnload( i3UIScene * pScene)
{
	m_pImageBox = nullptr;
	m_pVersionText = nullptr;

	UIFloatingBase::_ClearAtUnload( pScene);
	vecTabList.clear();
}

void UIServerAndChannelBanner::_ShowAnnounceLong(i3Texture* longTexture,INT32 height)
{
	// 478 px 이상 10px 마다 스크롤 1칸씩 가능.
	height = MAX(478, height);
//	INT32 scrollCount = (height - 478) / 10;

	// 이미지 교체
	/*GameGUI::Reset(m_pScrollList, scrollCount);
	m_pScrollList->SetCurValue(0.0f);

	m_pScrollFrame->SetEnable(true);
	m_pScrollList->SetEnable(true);
	m_pDummyAnnounceLong->ChangeFirstShape(longTexture, 0, 0, 359, 478);
	m_pDummyAnnounceLong->SetEnable(true);*/

	m_announceImageHeight = height;
}

void UIServerAndChannelBanner::_ShowAnnounceShort(i3Texture* shortTexture)
{
	// 이미지 교체
	/*m_pScrollFrame->SetEnable(false);
	m_pScrollList->SetEnable(false);
	m_pDummyAnnounceLong->SetEnable(false);

	m_pDummyAnnounceShort->ChangeFirstShape(shortTexture, 0, 0, 380, 478);
	m_pDummyAnnounceShort->SetEnable(true);*/
}

/*virtual*/ void UIServerAndChannelBanner::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Main/PointBlankRe_ServerAndChannelBanner.i3UIs", UIServerAndChannelBanner_Glue);
}

/*virtual*/ void UIServerAndChannelBanner::OnUpdate( REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate( rDeltaSeconds);

	if (SteamContext::i()->IsSteamUser())
	{
		SteamContext::i()->SteamAPI_RunCallbacks();
	}

	if (m_hWebAnnounce != nullptr) return;

	if( m_nNoticeIndex != -1) // 공지사항이 존재함
	{
		if( g_pFramework->getNoticeActivation() && !m_bClickedNoticeCaption)
		{
			m_fLocalNoticeTime += rDeltaSeconds;

			if( m_fLocalNoticeTime > 0.5f)
			{
				m_fLocalNoticeTime = 0.0f;

				/*m_pAnnounce[m_nNoticeIndex]->SetCaptionEnable( m_bEnableNoticeText);*/
				m_bEnableNoticeText = !m_bEnableNoticeText;
			}
		}
		else
		{
			/*m_pAnnounce[m_nNoticeIndex]->SetCaptionEnable( true);*/
		}
	}

	// 공지 순환
	if( m_fAnnounceFlipTime > 0.f && g_pFramework->GetAnnounceCount() > 1)
	{
		if( m_fAnnounceFlipTime <= m_fAnnounceFilpElapsedTime)
		{
			m_nCurrentSelectedIndex++;

			if( m_nCurrentSelectedIndex >= g_pFramework->GetAnnounceCount())
				m_nCurrentSelectedIndex = 0;

			ChangeAnnounce( m_nCurrentSelectedIndex);

			m_fAnnounceFilpElapsedTime = 0.f;

			// 유저의 클릭으로 일정시간이 지났을 경우 다시 원래 순환 시간으로 설정
			if( m_fAnnounceFlipTime == g_pConfigEx->GetMisc().AnnounceWaitingTimeAfterClicked)
				m_fAnnounceFlipTime = g_pConfigEx->GetMisc().AnnounceFlipTime;
		}
		else
		{
			m_fAnnounceFilpElapsedTime += rDeltaSeconds;
		}
	}	


}

/*virtual*/ void UIServerAndChannelBanner::SetEnable( bool bTrue /*= true*/, REAL32 tm /*= 0.f*/)
{
	UIFloatingBase::SetEnable( bTrue, tm);

	if( m_hWebAnnounce )
	{
		int cmdShow = (bTrue ? SW_SHOW : SW_HIDE);

		if (m_bWebError == true) cmdShow = SW_HIDE;

		::ShowWindow(m_hWebAnnounce, cmdShow);
	}
}

/*virtual*/ bool UIServerAndChannelBanner::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( isEnable())	// 이미 열려있는 경우는 초기화 하지 않는다.
		return true;

	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	__Init();

	m_nCurrentSelectedIndex = 0;
	m_fAnnounceFilpElapsedTime = 0.f;

	m_fAnnounceFlipTime = g_pConfigEx->GetMisc().AnnounceFlipTime;

	ChangeAnnounce( m_nCurrentSelectedIndex);

	_SetTabText();
	// 등급 표시
	LuaState * L = _CallLuaFunction( "SetGrade");
	if( L != nullptr)
	{
#if defined( LOCALE_KOREA) && !defined( SALES_BUILD)
		i3Lua::PushBoolean( L, true);
#else
		i3Lua::PushBoolean( L, false);
#endif
		_EndLuaFunction( L, 1);
	}

	if( m_bNoImage && m_pFrameWnd != nullptr)
	{
		i3UIFrameWnd* pWnd = (i3UIFrameWnd*)m_pFrameWnd->FindChildByName("i3UIFrameWnd");
		pWnd->EnableCtrl(false);

		if (m_pImgWebError)
		{
			m_pImgWebError->EnableCtrl(true);
			m_pImageBox->EnableCtrl(false);
		}
	}
	
	if(m_pVersionText != nullptr)
	{
		i3::stack_wstring wstrVersion;
		i3::sprintf(wstrVersion, L"Version. %d.%d.%d.%d", VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_COUNT);

		m_pVersionText->SetText(wstrVersion);
	}

	_on_off_image_announce(false);
	return true;
}

/*virtual*/ bool UIServerAndChannelBanner::OnEntrancing(REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnEntrancing(rDeltaSeconds);

	return true;
}

/*virtual*/ void UIServerAndChannelBanner::OnEntranceEnd(void)
{
	UIFloatingBase::OnEntranceEnd();

	i3::vector<BANNER_URL_INFO> url_list;	//복수로 웹툴에서 제공은 할 수 있는데 기획상 해당 부분에서 복수가 되면 안된다. 따라서 0만 쓸께여
	GameLoginContext::i()->GetIngameURLInfo(url_list, URL_TYPE_BANNER);

	if ( url_list.size() != 0 )
	{
		if (m_hWebAnnounce)
		{
			::DestroyWindow(m_hWebAnnounce);
			m_hWebAnnounce = nullptr;
			UninitWebCtrl();
		}

		i3::pack::RECT web_announce_rect;
		_calc_web_announce_rect(web_announce_rect);

		InitWebCtrl();
		if (SteamContext::i()->IsSteamUser())
		{
			INT32 i32Size = i3::generic_string_size(GameLoginContext::i()->getWebLoginKey());
			if (i32Size > 0)
			{
				i3::rc_wstring wstrParam = url_list[0].wsURL;
				i3::rc_wstring wstrParam2; i3::sprintf(wstrParam2, L"?s=%s", GameLoginContext::i()->getWebLoginKey());
				i3::generic_string_cat(wstrParam, wstrParam2);

				m_hWebAnnounce = ::CreateWebControl(wstrParam.c_str(),
					WS_CLIPCHILDREN | WS_CHILD, (INT)web_announce_rect.left, (INT)web_announce_rect.top, (INT)web_announce_rect.Width(), (INT)web_announce_rect.Height(),
					g_pViewer->GetWindowHandle(), NULL);
			}
		}
		else
			m_hWebAnnounce = ::CreateWebControl( url_list[0].wsURL.c_str(),
			WS_CLIPCHILDREN | WS_CHILD, (INT)web_announce_rect.left, (INT)web_announce_rect.top, (INT)web_announce_rect.Width(), (INT)web_announce_rect.Height(),
			g_pViewer->GetWindowHandle(), NULL);

		::ShowWindow(m_hWebAnnounce, SW_HIDE);

		// 배너 끄기
		_OffURLBanner();

		if (SteamContext::i()->IsSteamUser())
		{
			CWebCtrl* pCtl = (CWebCtrl*)GetProp(m_hWebAnnounce, "this");
			if (pCtl != nullptr) 
				pCtl->setSteamWebbrowser(true);			//스팀 팝업으로 띄움. 스팀 dll등의 오류가 날경우는 isCloseNewWebbrowser와 작동이 같음.
		}
		else
		{ 
			//CWebCtrl* ctl = (CWebCtrl*)GetProp(m_hWebAnnounce, "this");
			//if (ctl != nullptr) ctl->isPBWebboxuibrowser(true);		웹배너의 팝업들을 UIPopupWebbox로 띄움.
			//if (ctl != nullptr) ctl->isCloseNewWebbrowser(true);		웹베너의 팝업들을 현재 배너창에 띄움.
			//일단 구현만 하고 막음. 나중에 입맛에 맞게 쓰셈.
		}
	}

}

/*virtual*/ bool UIServerAndChannelBanner::OnExitStart(void)
{
	UIFloatingBase::OnExitStart();
	if (m_hWebAnnounce != nullptr) ::ShowWindow(m_hWebAnnounce, SW_HIDE);
	return true;
}

/*virtual*/ void UIServerAndChannelBanner::OnExitEnd(void)
{
	UIFloatingBase::OnExitEnd();

	if( m_hWebAnnounce != nullptr )
	{
		::DestroyWindow(m_hWebAnnounce);
		m_hWebAnnounce = nullptr;
		UninitWebCtrl();
	}

}

/*virtual*/ bool UIServerAndChannelBanner::OnEvent(UINT32 event, i3ElementBase* pObj, UINT32 param2, I3_EVT_CODE code)
{
	switch (event)
	{
		case UIEVENT_SET_FOCUS_POPUP:
		{
			if (code == I3_EVT_CODE_ACTIVATE)
			{
				if (m_hWebAnnounce != nullptr) ::ShowWindow(m_hWebAnnounce, SW_HIDE);
			}
		}
		break;

		case UIEVENT_KILL_FOCUS_POPUP:
		{
			if (code == I3_EVT_CODE_ACTIVATE)
			{
				if (m_hWebAnnounce != nullptr && m_bWebError == false)
					::ShowWindow(m_hWebAnnounce, SW_SHOW);
			}
		}
		break;
	}

	return UIFloatingBase::OnEvent(event, pObj, param2, code);
}

void UIServerAndChannelBanner::ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data & UserData)
{
	UIFloatingBase::ProcessGameEvent(evt, arg, UserData);

	switch (evt)
	{
		case EVENT_WEBCONTROL_ERROR:
		{
			_OffURLBanner();
		}break;

		case EVENT_GET_INGAME_URL:
			_SetEsportsBanner();
			break;
	}
}

void UIServerAndChannelBanner::AnnounceButton( INT32 idx)
{
	m_fAnnounceFlipTime = g_pConfigEx->GetMisc().AnnounceWaitingTimeAfterClicked;
	m_fAnnounceFilpElapsedTime = 0.f;

	if (g_pEnvSet->m_nUIVersion == UFV_1_1)
		idx = idx - (MAX_ANNOUNCE - g_pFramework->GetAnnounceCount());

	if (idx < 0 || idx > MAX_ANNOUNCE)
		idx = 0;

	SetCurrentSelectedIndex(idx);
	ChangeAnnounce( idx);
}

void UIServerAndChannelBanner::ChangeAnnounce( INT32 idx)
{
	if( idx >= MAX_ANNOUNCE)
		return;

	if( m_pImageBox == nullptr)
		return;

	if( m_pAnnounceTexture[ idx] != nullptr)
	{
		if (m_bAnnounceScroll[ idx])
		{
			INT32 height = m_pAnnounceTexture[ idx]->GetHeight();
			_ShowAnnounceLong(m_pAnnounceTexture[ idx], height);
		}
		else
		{
			_ShowAnnounceShort(m_pAnnounceTexture[ idx]);
		}
	}

	if( m_nNoticeIndex != -1)
	{
		if( idx == m_nNoticeIndex) m_bClickedNoticeCaption = true;
	}

	m_pImageBox->SetCurrentImage( idx);

	if (g_pEnvSet->m_nUIVersion == UFV_1_1)
		idx += (MAX_ANNOUNCE - g_pFramework->GetAnnounceCount());

	LuaState * L = _CallLuaFunction( "SetPushedButton");
	i3Lua::PushInteger( L, idx);
	_EndLuaFunction( L, 1);
}

void UIServerAndChannelBanner::_SetTabText()
{
	// UI 1.0일 경우 공지 이름을 받아와서 세팅합니다.
	if (g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		for(INT32 i = 0 ; i < g_pFramework->GetAnnounceCount(); i++)
		{
			i3::rc_wstring tabname; 
			i3::sprintf(tabname, L"%s", g_pFramework->GetAnnounceName(i));
			vecTabList[i]->SetText(tabname);
		}
	}
	else if (g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		INT32 mod_ = MAX_ANNOUNCE - g_pFramework->GetAnnounceCount();
		for(INT32 i = 0; i < g_pFramework->GetAnnounceCount(); i++)
		{
			i3::wstring tabname;
			i3::sprintf(tabname, L"0%d", i+1);
			vecTabList[i+mod_]->SetText(tabname);
		}
	}
}

void UIServerAndChannelBanner::OnLoadAllScenes()
{
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::L_TO_R);
}

void UIServerAndChannelBanner::ChannelListAfterProcess()
{
	if (m_hWebAnnounce != nullptr)
	{
		if(!m_bWebError)
			::ShowWindow(m_hWebAnnounce, SW_SHOW);
	}
	else
		_on_off_image_announce(true);
}

void UIServerAndChannelBanner::ShowHideWebAunnounce(bool onoff)
{
	if (m_hWebAnnounce == nullptr) return;
	if (m_bWebError == true) onoff = false;

	onoff ? ::ShowWindow(m_hWebAnnounce, SW_SHOW) : ::ShowWindow(m_hWebAnnounce, SW_HIDE) ;
}

void UIServerAndChannelBanner::_calc_web_announce_rect(i3::pack::RECT& rc)
{
	i3Viewer* pViewer = g_pFramework->GetViewer();
	i3UIImageBoxEx* calc_frame = (i3UIImageBoxEx*)m_pFrameWnd->FindChildByName("i3UIImageBoxEx");

	REAL32 ui_ratio = (REAL32)i3UI::getManager()->getWidth() / (REAL32)i3UI::getManager()->getHeight();
	REAL32 view_ratio = (REAL32)pViewer->GetViewWidth() / (REAL32)pViewer->GetViewHeight();

	//pos
	VEC3D* image_pos = calc_frame->getAccumulatedPos();

	INT32 offset_x = i3UI::getManager()->getDisplayOffsetX();
	INT32 offset_y = i3UI::getManager()->getDisplayOffsetY();

	VEC3D v;
	v.x = (REAL32)pViewer->GetViewWidth() / (REAL32)i3UI::getManager()->getWidth();
	v.y = (REAL32)pViewer->GetViewHeight() / (REAL32)i3UI::getManager()->getHeight();

	REAL32 ratio = v.y;
	if (ui_ratio > view_ratio)
		ratio = v.x;

	rc.left = (image_pos->x*ratio) + offset_x;
	rc.top = (image_pos->y*ratio) + offset_y;

	//size
	i3::pack::RECT cs;
	calc_frame->getAccumulatedRect(&cs);

	REAL32 w = cs.right - cs.left;
	REAL32 h = cs.bottom - cs.top;

	rc.right = rc.left + (w * ratio);
	rc.bottom = rc.top + (h * ratio);
}

void UIServerAndChannelBanner::_on_off_image_announce(bool onoff)
{
	I3ASSERT_RETURN(m_pFrameWnd);

	i3UIFrameWnd* close_control_0 = (i3UIFrameWnd*)m_pFrameWnd->FindChildByName("i3UIFrameWnd");
	i3UIFrameWnd* close_control_1 = (i3UIFrameWnd*)m_pFrameWnd->FindChildByName("i3UIFrameWnd_CB");

	I3ASSERT_RETURN(close_control_0 && close_control_1);

	close_control_1->EnableCtrl(onoff);
	close_control_0->EnableCtrl(onoff);
}

void UIServerAndChannelBanner::_OffURLBanner()
{
	// 웹 페이지가 정상적으로 출력되지 않는다면 웹배너를 띄우지 않습니다.
	CWebCtrl* pCtl = (CWebCtrl*)GetProp(m_hWebAnnounce, "this");
	if (pCtl != nullptr)
	{
		if (pCtl->GetNavigate2ErrorCode() != 0)
		{
			m_bWebError = true;
			if (m_pImgWebError && m_bNoImage)
			{
				m_pImgWebError->EnableCtrl(true);
				m_pImageBox->EnableCtrl(false);
			}

			_on_off_image_announce(true);
			if (m_hWebAnnounce != nullptr)
			{
				::DestroyWindow(m_hWebAnnounce);
				m_hWebAnnounce = nullptr;
				UninitWebCtrl();
			}
		}
	}

	
}

void UIServerAndChannelBanner::_SetEsportsBanner()
{
	if (g_pEnvSet->m_nUIVersion == UFV_1_2 ||
		g_pEnvSet->m_nUIVersion == UFV_1_5)
		return;

	bool esportsBannerEnable = false;
	i3::vector<BANNER_URL_INFO> url_list;

	//같은 타입의 리스트를 받지만 Esports배너 하나에만 사용할 것이기 때문에 0번 인덱스만 사용
	GameLoginContext::i()->GetIngameURLInfo(url_list, URL_TYPE_ESPORTS_BANNER);

	if (url_list.size() != 0)
	{
		i3::utf16_to_mb(url_list[0].wsURL, m_strEsportsBannerUrl);
		esportsBannerEnable = true;
	}
		
	LuaState * L = _CallLuaFunction("SetEsportsBannerEnable");
	if (L != NULL)
	{
		i3Lua::PushBoolean(L, esportsBannerEnable);
		_EndLuaFunction(L, 1);
	}
}