#include "pch.h"
#include "UIMainFrame_V15.h"

#include "ChannelContext.h"
#include "UIPhase.h"
#include "UIFloating.h"
#include "UIPopup.h"
#include "UIToolTip.h"

#include "UIServerAndChannelBanner.h"
#include "UINewAndHotBanner.h"
#include "UILobbyBanner.h"
#include "UILobbyClanInfo.h"
#include "UIPopupMedalRestriction.h"

#include "UI/UITopMenu_V15.h"
#include "UISideMenu.h"

#include "UI_V15/UIPhaseReadyRoom_V15.h"

#include "RoomMasterKickManager.h"

#include "MyRoomInfoContext.h"

I3_CLASS_INSTANCE( UIMainFrame_V15);//, i3GameNode);

UIMainFrame_V15::UIMainFrame_V15(){

}

UIMainFrame_V15::~UIMainFrame_V15(){

}


/*virtual*/ void UIMainFrame_V15::OnLoadEnd(i3Stage* pStage)
{
	UIMainFrame::OnLoadEnd(pStage);
	INT32 i;

	// Key : User Guide System
	UserGuideMgr::i()->InitUserGuideList();

	m_pTopMenu = UITopMenu_V15::new_object();
	m_pTopMenu->OnCreate( pStage);

	m_pSideMenu = UISideMenu::new_object();
	m_pSideMenu->OnCreate( pStage);

	i3ClassMeta * pMeta = nullptr;

	// Create Phase
	m_Phase.resize(UIPHASE_MAX);
	for( i = 0; i < UIPHASE_MAX; i++)
	{
		switch( i)
		{
		case UIPHASE_SERVER_CHANNEL_LIST :		pMeta = UIPhaseSelectServer::static_meta();	break;
		case UIPHASE_CREATE_CHARA :				pMeta = UIPhaseCreateChara::static_meta();		break;
		case UIPHASE_LOBBY :					pMeta = UIPhaseLobby::static_meta();			break;
		case UIPHASE_CLANLOBBY :				pMeta = UIPhaseClanLobby::static_meta();		break;
		case UIPHASE_READY_ROOM :				pMeta = UIPhaseReadyRoom_V15::static_meta();		break;
			//case UIPHASE_CLAN :						pMeta = UIPhaseNonClanMember::static_meta();	break;
			/*
		default :
			pMeta = nullptr;
			I3PRINTLOG(I3LOG_NOTICE,  "Not Setted phase class...!!!");
			break;
			*/
		}

		if( pMeta != nullptr)
		{
			m_Phase[i] = (UIPhaseBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
			I3ASSERT( m_Phase[i]->same_of( pMeta));

			m_Phase[i]->OnCreate( pStage);
			I3_MUST_ADDREF(m_Phase[i]);
		}
	}

	// Phase 기본 설정
	if( m_lastPhase == UIPHASE_START)
	{
#if defined(AUTO_LOBBY)
		GameEventSender::i()->SetEvent(EVENT_GET_SYSTEM_INFO);
		GameEventSender::i()->SetEvent(EVENT_GET_MYOPTION_N);
#else
		MovePhase( UIPHASE_SERVER_CHANNEL_LIST);
#endif
	}
	else
	{
		m_PhaseQueue.push_back( m_lastPhase);
		m_currentPhase = m_lastPhase;
	}

	// Create Sub Phase
	m_SubPhase.resize(USP_MAX);
	for( i = 0; i < USP_MAX; i++)
	{
		switch( i)
		{	
		case USP_SHOP		: pMeta = UIPhaseShop::static_meta();		break;
		case USP_INVENTORY	: pMeta = UIPhaseInventory::static_meta();	break;	
		case USP_GACHA		: pMeta = UIPhaseGachaShop::static_meta();	break;
		case USP_GIFTSHOP	: pMeta = UIPhaseGiftShop::static_meta();	break;
		default	:
			pMeta = nullptr;
			I3PRINTLOG(I3LOG_NOTICE,  "Not Setted Subphase class...!!!");
			break;
		}

		if( pMeta != nullptr)
		{
			m_SubPhase[i] = (UIPhaseSubBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
			I3ASSERT( m_SubPhase[i]->same_of( pMeta));

			m_SubPhase[i]->OnCreate( pStage);
			I3_MUST_ADDREF(m_SubPhase[i]);
		}
	}

	// Create Floating window
	m_FloatWins.resize(UFW_MAX);
	for( i = 0; i < UFW_MAX; i++)
	{
		pMeta = nullptr;
		switch( i)
		{
		case UFW_CHAT :					pMeta = UIFloatChat::static_meta();			break;
		case UFW_USERLIST:				pMeta = UIFloatUserList::static_meta();		break;
		case UFW_MINIPROFILE :			pMeta = UIFloatMiniProfile::static_meta();		break;
		case UFW_CHARACTER :			pMeta = UIFloatCharacterView::static_meta();	break;
		case UFW_READYROOMINFO :		pMeta = UIFloatReadyRoomInfo::static_meta();	break;
		case UFW_CHANNEL_LIST:			pMeta = UIFloatChannelList::static_meta();		break;
		case UFW_SCBANNER :				pMeta = UIServerAndChannelBanner::static_meta();	break;
		case UFW_LOBBYBANNER :			pMeta = UILobbyBanner::static_meta();			break;
		case UFW_NEWANDHOT :			pMeta = UINewAndHotBanner::static_meta();		break;		
		case UFW_CLANLOBBY :			pMeta = UILobbyClanInfo::static_meta();		break;		
		case UFW_PLAYTIMEALARM :		pMeta = UIFloatPlayTimeAlarm::static_meta();	break;
		case UFW_MERCENARY :			pMeta = UIFloatMercenaryList::static_meta();	break;

		default :
			I3PRINTLOG(I3LOG_NOTICE,  "Not Setted floating window...!!!");
			break;
		}

		if( pMeta != nullptr)
		{
			m_FloatWins[i] = (UIFloatingBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
			I3ASSERT( m_FloatWins[i]->same_of( pMeta));

			m_FloatWins[i]->OnCreate( pStage);
			I3_MUST_ADDREF(m_FloatWins[i]);
		}
	}

	// Create Popup windows
	m_Popup.resize(UPW_MAX);
	for( i = 0; i < UPW_MAX; i++)
	{
		pMeta = nullptr;
		switch( i)
		{
		case UPW_BUYITEM :			pMeta = UIPopupBuyItem::static_meta();			break;
		case UPW_SHOPEXTENSION :	pMeta = UIPopupShopExtension::static_meta();	break;
		case UPW_COUPONLIST :		pMeta = UIPopupCouponList::static_meta();		break;
		case UPW_CREATEROOM_EX :	pMeta = UIPopupCreateRoom::static_meta();	break;
		case UPW_MAILBOX :			pMeta = UIPopupMailBox::static_meta();			break;
		case UPW_MAILREV :			pMeta = UIPopupReceiveMail::static_meta();		break;
		case UPW_QUICKJOINOPT :		pMeta = UIPopupQuickJoinOption::static_meta();	break;
		case UPW_USERPROFILE :		pMeta = UIPopupUserProfile::static_meta();		break;
		case UPW_INVITELOBBYUSER :	pMeta = UIPopupInviteLobbyUser::static_meta();	break;
		case UPW_QUERY_PASSWORD :	pMeta = UIPopupQueryPassword::static_meta();	break;
		case UPW_OPTION :			pMeta = UIPopupOption::static_meta();			break;
		case UPW_REPORT :			pMeta = UIPopupReport::static_meta();			break;
		case UPW_CLAN_CREATE :		pMeta = UIPopupCreateClan::static_meta();		break;
		case UPW_CLAN_NAME_CHECK:	pMeta = UIPopupClanNameSearch::static_meta();	break; /* 클랜 이름 중복체크 : 2011-06-29 김재진 */
		case UPW_CLAN_WARNING :		pMeta = UIPopupClanWarning::static_meta();		break; /* 클랜 생성시 경고창 추가 : 2011-06-29 김재진 */
		case UPW_CLAN_JOIN :		pMeta = UIPopupJoinClan::static_meta();		break; /* 클랜 가입시 추가        : 2011-06-29 김재진 */
		case UPW_LINK_SHOP :		pMeta = UIPopupLinkShop::static_meta();		break;
		case UPW_CREATENICKNAME :	pMeta = UIPopupNickNameSetting::static_meta();	break;
		case UPW_RANK_UP :			pMeta = UIPopupRankup::static_meta();			break;
		case UPW_PCCAFE :			pMeta = UIPopupPCCafe::static_meta();			break;
		case UPW_ADDBLOCK :			pMeta = UIPopupAddBlockList::static_meta();	break;
		case UPW_PLAYWITH :			pMeta = UIPopupTrackingUser::static_meta();	break;
		case UPW_QUERY_ITEM :		pMeta = UIPopupQueryItem::static_meta();	break;						// 아이템 사용자 문의 팝업
		case UPW_RESULT_USEITEM :	pMeta = UIPopupResultUseItem::static_meta();	break;						// 아이템 사용 결과 팝업
		case UPW_CHANGENICK :		pMeta = UIPopupUseItemChangeNick::static_meta(); break;					// 닉네임 변경
		case UPW_CHANGE_COLOR_NICK: pMeta = UIPopupColorNickname::static_meta(); break;					// 컬러 닉네임 설정
		case UPW_CHANGE_CROSSHAIR_COLOR : pMeta = UIPopupUseItemChangeCrosshairColor::static_meta(); break;	// 컬러 크로스헤어
		case UPW_DISGUISE_RANK :	pMeta = UIPopupUseItemDisguiseRank::static_meta(); break;					// 위장 계급
		case UPW_CHANGE_CLAN_MARK :	pMeta = UIPopupUseItemChangeClanMark::static_meta(); break;				// 클랜 마크 변경
		case UPW_SKILLOPEN_ITEM :	pMeta = UIPopupSkillOpen::static_meta(); break;
		case UPW_SKILLOPEN_RESULT :	pMeta = UIPopupSkillOpenResult::static_meta(); break;
		case UPW_COLOR_FIRE_MUZZLE_EFFECT : pMeta = UIPopupColorFireEffect::static_meta(); break;

		case UPW_COUPON_INPUT:		pMeta = UIPopupCouponInput::static_meta();		break;

		case UPW_EVENT:				pMeta = UIPopupEvent::static_meta();		break;
		case UPW_DISCOUNT_COUPON:	pMeta = UIPopupDiscountCoupon::static_meta();	break;			// 할인쿠폰

		case UPW_EXIT_POPUP: pMeta = UIPopupExit::static_meta(); break;
		case UPW_TS_REWARD :		pMeta = UIPopupTSReward::static_meta(); break;

			// Key : 출석 체크 추가
		case UPW_VISIT_EVENT:			pMeta = UIPopupVisitEvent::static_meta(); break;
		case UPW_ATTENDANCE_CHECK:		pMeta = UIPopupAttendanceCheck::static_meta(); break;
		case UPW_ATTENDANCE_COMPLETE:	pMeta = UIPopupAttendanceComplete::static_meta(); break;

		case UPW_PROFILE :				pMeta = UIPhaseProfile::static_meta();	break;

		case UPW_CLANHONOR :			pMeta = UIPopupClanHonor::static_meta();	break;	

		case UPW_CLAN_NON_MEMBER :		pMeta = UIPhaseNonClanMember::static_meta();		break;
		case UPW_CLAN_MEMBER :			pMeta = UIPhaseClanMember::static_meta();			break;
		case UPW_MEDAL_REWARD :			pMeta = UIPopupMedalReward::static_meta();			break;
		case UPW_MEDAL_QUICK_REWARD:	pMeta = UIPopupQuickMedaiReward::static_meta();		break;
		case UPW_BREAK_INTO_GAME :		pMeta = UIPopupBreakIntoGame::static_meta();		break;
		case UPW_WEB_BOX :				pMeta = UIPopupWebBox::static_meta();				break;

		case UPW_DOMI_SCORE:			pMeta = UIPopupDomiScore::static_meta();			break;
		case UPW_DOMI_ROUND:			pMeta = UIPopupDomiRound::static_meta();			break;
		case UPW_DOMI_ENTER:			pMeta = UIPopupDomiEnter::static_meta();			break;

		case UPW_GACHA_WEAPON_DETAIL:	pMeta = UIPopupGachaWeaponDetail::static_meta();	break;
		case UPW_GACHA_RESULT:			pMeta = UIPopupGachaResult::static_meta();			break;	
		case UPW_GACHA_HELP_GUIDE:		pMeta = UIPopupGachaHelpGuide::static_meta();			break;	

#if legacy_gui_scene
		case UPW_ERROR :			pMeta = UIPopupError::static_meta();			break;
		case UPW_MAILREP :			pMeta = UIPopupReplyMail::static_meta();		break;
		case UPW_ABUSE :			pMeta = UIPopupAbuse::static_meta();			break;
		case UPW_CLAN_MARKCHANGE_OLD : pMeta = UIPopup_Clan_ChangeMark_Old::static_meta();break;
		case UPW_CHANGE_CLAN_NAME :	pMeta = UIPopup_Clan_ChangeName::static_meta();break;						// 클랜관리 이름 변경
#endif

		case UPW_QUICK_JOIN:		pMeta = UIPopupQuickJoin::static_meta();	break;
		case UPW_QUICK_JOIN_SECOND_BEST: pMeta = UIPopupQuickJoinSecondBest::static_meta(); break;

		case UPW_INVEN_DELETE_ITEM_NEW:	pMeta = UIPopupInvenDeleteItem::static_meta();	break;

			// Key : GM 강화 기능 추가
		case UPW_GM_OPTION:				pMeta = UIPopupGMOption::static_meta();			break;
		case UPW_GM_CHATBOX:			pMeta = UIPopupGMChatBox::static_meta();		break;

		case UPW_MEDAL_ITEM :			pMeta = UIPopupMedalRestriction::static_meta();	break;
		case UPW_GIFT_SEND :			pMeta = UIPopupGiftSend::static_meta();			break;
		case UPW_CLAN_SEASON_OVERALL_RANKING: pMeta = UIPopupClanSeason_OverallRanking::static_meta();	break;
		case UPW_CLAN_SEASON_INDIVISUAL_RANKING: pMeta = UIPopupClanSeason_IndivisualRanking::static_meta(); break;
		case UPW_TWITCHLOGIN :			pMeta = UIPopupTwitchLogin::static_meta();		break;
		case UPW_TWITCHOPTION :			pMeta = UIPopupTwitchOption::static_meta();		break;

		case UPW_MONTH_ATTENDANCE :		pMeta = UIPopupMonthAttendance::static_meta();	break;
		case UPW_DORMANT_EVENT:			pMeta = UIPopupDormantEvent::static_meta();		break;

		default :
			I3PRINTLOG(I3LOG_NOTICE,  "Not Setted popup class...!!!");
			break;
		}

		if( pMeta != nullptr)
		{
			m_Popup[i] = (UIPopupBase*) pMeta->create_instance();
			I3ASSERT( m_Popup[i]->same_of( pMeta));

			m_Popup[i]->OnCreate( pStage);
			I3_MUST_ADDREF(m_Popup[i]);
		}
	}
}

/*virtual*/ void UIMainFrame_V15::OnFinish( void)
{
	UIMainFrame::OnFinish();
	//Destroy는 클래스 제거, OnExitStart 클래스 내부 소거 루틴이다.
	//클래스 내부에 소거 루틴을 거친후에 클래스를 제거해야 된다.
	if(m_pTopMenu)
	{
		//m_pTopMenu->OnExitStart(); 이거 사용 안 해도 잘 돌아가서 주석 처리함..
		m_pTopMenu->Destroy();
		m_pTopMenu = nullptr;
	}

	if( m_pSideMenu )
	{
		m_pSideMenu->OnExitStart();
		m_pSideMenu->Destroy();
		m_pSideMenu = nullptr;
	}

	for(size_t i = 0; i < m_Phase.size(); i++)
	{
		if( m_Phase[i])
		{
			m_Phase[i]->Destroy();
			m_Phase[i] = nullptr;
		}
	}

	for(size_t i = 0; i < m_SubPhase.size(); i++)
	{
		if( m_SubPhase[i])
		{
			m_SubPhase[i]->Destroy();
			m_SubPhase[i] = nullptr;
		}
	}

	for(size_t i = 0; i < m_FloatWins.size(); i++)
	{
		if( m_FloatWins[i])
		{
			m_FloatWins[i]->OnExitStart();
			m_FloatWins[i]->Destroy();
			m_FloatWins[i] = nullptr;
		}
	}

	for(size_t i = 0; i < m_Popup.size(); i++)
	{
		if( m_Popup[i])
		{
			//
			// 팝업이 Destroy되기 전에 Enable검사를 하고...Enable중이라면 FocusPopup을 끌수도 있다...  
			// (메세지박스와의 혼용문제가 있기 때문에, 아직 개념정리중..)
			// 그렇지 않다면 이 시점에서 Enable된 팝업이 없어야함..

			m_Popup[i]->Destroy();
			m_Popup[i] = nullptr;
		}
	}
}

/*virtual*/void UIMainFrame_V15::StartPhase( UIPHASE MovePhase)
{
	UIMainFrame::StartPhase(MovePhase);

	// TopMenu의 버튼 구성을 바꿉니다.
	switch( m_currentPhase)
	{
	case UIPHASE_START :
	case UIPHASE_SERVER_CHANNEL_LIST:
		if (m_pTopMenu != nullptr)
			m_pTopMenu->SetMode( TOPMENU_MODE_SERVER);

		EnableFloatWin( UFW_SCBANNER);
		EnableFloatWin( UFW_MINIPROFILE);
		break;
	case UIPHASE_CREATE_CHARA:
		{
			EnableFloatWin( UFW_CHARACTER);
		}
		break;
	case UIPHASE_CLANLOBBY:
		{
			// Lobby입장에 대한 처리를 한다.
			if (m_pTopMenu != nullptr)
				m_pTopMenu->SetMode( TOPMENU_MODE_READY);

			// Floating window를 활성화
			EnableFloatWin( UFW_CHAT);
			EnableFloatWin( UFW_CHARACTER);
			EnableFloatWin( UFW_USERLIST);
			EnableFloatWin( UFW_CLANLOBBY);
			EnableFloatWin( UFW_MERCENARY);

			_SideMenu_Entrance_();

			UIFloatCharacterView * pCharaView = (UIFloatCharacterView*) GetFloating( UFW_CHARACTER);
			if( pCharaView != nullptr)
				pCharaView->UpdateCharaSlot();
		}
		break;
	case UIPHASE_LOBBY:
		{
			// Lobby입장에 대한 처리를 한다.
			if (m_pTopMenu != nullptr)
				m_pTopMenu->SetMode( TOPMENU_MODE_READY);

			// Floating window를 활성화
			EnableFloatWin( UFW_CHAT);
			EnableFloatWin( UFW_CHARACTER);
			EnableFloatWin( UFW_USERLIST);

			if( ChannelContext::i()->GetBattleChannel() )
			{
				EnableFloatWin( UFW_CLANLOBBY);
				EnableFloatWin( UFW_MERCENARY);
			}
			else
			{
				EnableFloatWin( UFW_NEWANDHOT);
			}

			_SideMenu_Entrance_();

			UIFloatCharacterView * pCharaView = (UIFloatCharacterView*) GetFloating( UFW_CHARACTER);
			if( pCharaView != nullptr)
				pCharaView->UpdateCharaSlot();
		}
		break;

	case UIPHASE_READY_ROOM:
		{
			if (m_pTopMenu != nullptr)
				m_pTopMenu->SetMode( TOPMENU_MODE_READYROOM);	

			EnableFloatWin( UFW_CHARACTER);
			EnableFloatWin( UFW_MINIPROFILE);
			EnableFloatWin( UFW_CHAT);
			EnableFloatWin( UFW_USERLIST);
			EnableFloatWin( UFW_READYROOMINFO);

			_SideMenu_Entrance_();

			UIFloatCharacterView * pCharaView = (UIFloatCharacterView*) GetFloating( UFW_CHARACTER);
			if( pCharaView != nullptr)
				pCharaView->UpdateCharaSlot();
		}
		break;
	}

	UpdateLocation();

	for(size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);

		if( pFloatWin != nullptr && pFloatWin->isEnable())
			pFloatWin->OnMovePhase( m_currentPhase);
	}

	m_lastPhase = m_currentPhase;
}

/*virtual*/void UIMainFrame_V15::EndPhase( void)
{
	UIPHASE NextPhase = UIPHASE_START;

	if( m_PhaseQueue.size() > 0)
		NextPhase = m_PhaseQueue[0];

	// Floating Window를 끕니다
	switch( m_currentPhase)
	{
	case UIPHASE_SERVER_CHANNEL_LIST:
		DisableFloatWin( UFW_SCBANNER);
		DisableFloatWin( UFW_CHANNEL_LIST);
		break;
	case UIPHASE_CREATE_CHARA:
		{
			DisableFloatWin( UFW_CHARACTER);
		}
		break;
	case UIPHASE_CLANLOBBY:
	case UIPHASE_LOBBY:
		switch( NextPhase)
		{
		case UIPHASE_SERVER_CHANNEL_LIST :
			{	// 채널 리스트로 나가는 경우 로비의 floating을 닫아준다.
				// 모든 Float window를 닫는다.
				for(size_t i = 0; i < m_FloatWins.size(); i++)
				{			
					// MiniProfile은 제외 항시 상주해야 하는 UI
					if( i == UFW_MINIPROFILE) continue;
					DisableFloatWin( (UIFLOATWIN) i);
				}

				_SideMenu_Exit_();
			}
			break;

		case UIPHASE_CLANLOBBY:
			break;
		case UIPHASE_LOBBY:
			{
				DisableFloatWin( UFW_MERCENARY );
			}
			break;
		case UIPHASE_READY_ROOM :
			{	// 대기방으로 들어가는 경우 배너를 닫는다.
				//DisableFloatWin( UFW_CHARACTER);
				DisableFloatWin( UFW_LOBBYBANNER);
				DisableFloatWin( UFW_NEWANDHOT);
				DisableFloatWin( UFW_CLANLOBBY);
				DisableFloatWin( UFW_MERCENARY );
				_SideMenu_Exit_();
			}
			break;
		case UIPHASE_CREATE_CHARA :
			{
				//DisableFloatWin( UFW_CHARACTER);
				DisableFloatWin( UFW_CHAT);
				DisableFloatWin( UFW_USERLIST);
				DisableFloatWin( UFW_LOBBYBANNER);
				DisableFloatWin( UFW_NEWANDHOT);
				DisableFloatWin( UFW_CLANLOBBY);
				DisableFloatWin( UFW_MERCENARY );
				_SideMenu_Exit_();
			}
		}
		break;
	case UIPHASE_READY_ROOM:
		if( NextPhase == UIPHASE_LOBBY ||
			NextPhase == UIPHASE_CLANLOBBY )
		{
			DisableFloatWin( UFW_READYROOMINFO);
			_SideMenu_Exit_();
		}
		break;
	}

	GetCurrentPhase()->OnExitStart();

	// Key : User Guide System
	{
		UserGuideMgr::i()->DisableUserGuideAll();
	}
}

void UIMainFrame_V15::UpdateChatBuffer( void )
{
	UIFloatingBase * curfloat = GetFloating(UFW_CHAT);
	if(curfloat != nullptr)
	{
		((UIFloatChat*)curfloat)->UpdateChatBuffer();
	}
}

void UIMainFrame_V15::UpdateMegaPhoneBuffer(void)
{
	UIFloatingBase * curfloat = GetFloating(UFW_CHAT);
	if (curfloat != nullptr)
	{
		((UIFloatChat*)curfloat)->UpdateMegaPhoneBuffer();
	}
}

void UIMainFrame_V15::PreStartSubPhase( UISUBPHASE phase)
{
	UIPopupBase * pPopupbase = GetPopup(UPW_SHOPEXTENSION);

	m_SubOldPhase = m_CurrentSubPhase;

	if( phase == m_CurrentSubPhase)
		m_CurrentSubPhase = USP_NONE;
	else
		m_CurrentSubPhase = phase;

	if( USP_NONE != m_CurrentSubPhase)
	{
		//기본  Phase 처리
		switch( m_currentPhase)
		{
		case UIPHASE_CLANLOBBY:
		case UIPHASE_LOBBY:
			DisableFloatWin( UFW_LOBBYBANNER );
			DisableFloatWin( UFW_NEWANDHOT );
			DisableFloatWin( UFW_CHANNEL_LIST);
			DisableFloatWin( UFW_MERCENARY );
			_SideMenu_Exit_();
			break;

		case UIPHASE_READY_ROOM:
			DisableFloatWin( UFW_LOBBYBANNER);
			DisableFloatWin( UFW_NEWANDHOT);
			DisableFloatWin( UFW_READYROOMINFO);
			_SideMenu_Exit_();
			break;
		}


		//진입 하는 SubPhase에 대해서 다르게 행동한다.
		switch( phase )
		{
		case USP_INVENTORY:
		case USP_SHOP:
			DisableFloatWin( UFW_CHAT );
			DisableFloatWin( UFW_USERLIST);
			DisableFloatWin( UFW_NEWANDHOT);
			DisableFloatWin( UFW_CLANLOBBY);
			DisableFloatWin( UFW_READYROOMINFO);
			GetCurrentPhase()->OnExitStart();	// 메인 페이지 퇴장

			if (m_pTopMenu != nullptr)
			{
				if( phase == USP_INVENTORY )
					SetUICharaCamera(CL_WEAPON);
				else
					SetUICharaCamera(CL_SHOP);
			}

			break;
		case USP_GACHA:		
			EnableFloatWin( UFW_CHAT );
			EnableFloatWin( UFW_USERLIST);
			DisableFloatWin( UFW_NEWANDHOT);
			DisableFloatWin( UFW_CLANLOBBY);
			DisableFloatWin( UFW_READYROOMINFO);
			GetCurrentPhase()->OnExitStart();	// 메인 페이지 퇴장
			break;

		case USP_GIFTSHOP:
			DisableFloatWin( UFW_CHAT );
			DisableFloatWin( UFW_USERLIST);
			DisableFloatWin( UFW_NEWANDHOT);
			DisableFloatWin( UFW_CLANLOBBY);
			DisableFloatWin( UFW_READYROOMINFO);
			GetCurrentPhase()->OnExitStart();	// 메인 페이지 퇴장
			break;

		case USP_PROFILE:
			DisableFloatWin( UFW_NEWANDHOT);
			DisableFloatWin( UFW_READYROOMINFO);
			DisableFloatWin( UFW_CLANLOBBY);
			DisableFloatWin( UFW_MINIPROFILE);
			GetCurrentPhase()->OnExitStart();
			break;

		default:
			SetUICharaCamera(CL_IDLE);
			break;
		}

		if (m_CurrentSubPhase != USP_PROFILE)
		{
			if (pPopupbase->isEnable())
			{
				pPopupbase->OnExitStart();
			}
		}
	}
	else
	{
		EnableFloatWin(UFW_CHARACTER);
	}

	for(size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);
		if( pFloatWin != nullptr && pFloatWin->isEnable())
		{
			pFloatWin->OnMoveSubPhase( m_CurrentSubPhase);
		}
	}

	// Key : User Guide System
	{
		UserGuideMgr::i()->DisableUserGuideAll();
	}
}

void UIMainFrame_V15::EndSubPhase( UISUBPHASE phase)
{
	UIPopupBase * pPopupbase = GetPopup(UPW_SHOPEXTENSION);

	//ChannelType channeltype;
	if( phase == USP_NONE)
	{
		GetCurrentPhase()->OnModifyPhase_EndSubPhase( m_currentPhase );

		// SubPhase가 끝나면 Phase에게 Input을 돌려준다.	
		GetCurrentPhase()->SetInputDisable( false);			// 이것이 꼭 필요하지 않을수 있음.(11.10.20.수빈)

		if (false == pPopupbase->isEnable())
		{
			// Phase를 Entrance 시키고 기타 Floatingwindow를 띄운다.
			if( GetCurrentPhase()->OnEntranceStart() )
			{
				//기본  Phase 처리 
				//어떤 SubPhase에서 나오는지 알 수 없기에 기본으로 나와야 하는 모든 Floating을 띄운다.
				//정리가 필요한 부분
				switch( m_currentPhase)
				{
				case UIPHASE_CREATE_CHARA :
				case UIPHASE_LOBBY :
					EnableFloatWin(UFW_CHAT);
					EnableFloatWin( UFW_USERLIST);

					if( ChannelContext::i()->GetBattleChannel())
					{
						EnableFloatWin( UFW_CLANLOBBY);
						EnableFloatWin( UFW_MERCENARY);
					}
					else
						EnableFloatWin( UFW_NEWANDHOT);

					_SideMenu_Entrance_();
					break;
				case UIPHASE_CLANLOBBY:
					EnableFloatWin(UFW_CHAT);
					EnableFloatWin( UFW_USERLIST);
					EnableFloatWin( UFW_CLANLOBBY);	
					EnableFloatWin( UFW_MERCENARY);
					_SideMenu_Entrance_();
					break;
				case UIPHASE_READY_ROOM :
					EnableFloatWin(UFW_CHAT);
					EnableFloatWin( UFW_USERLIST);
					EnableFloatWin( UFW_READYROOMINFO);
					_SideMenu_Entrance_();
					break;
				}
			}
		}
	}
}

void	UIMainFrame_V15::EntranceSubPhase( void)
{
	if( GetCurrentSubPhase()->OnEntranceStart())
	{
		//SubPhase에서 특징적인 행동을 처리하면 된다.
		switch(m_CurrentSubPhase)
		{
		case USP_INVENTORY:
		case USP_SHOP:
		case USP_PROFILE:
			{
				if(m_pTopMenu != nullptr)
					m_pTopMenu->SetMode( TOPMENU_MODE_SHOP );

				//랜더링 순서 때문에 캐릭터 뷰 활성화 후에 진행되어야 한다. (USP_INVENTORY, USP_SHOP)
				GetCurrentSubPhase()->OnEntranceEnd();

				_SideMenu_Entrance_();
			}
			break;
		case USP_GIFTSHOP:
			{
				if(m_pTopMenu != nullptr)
					m_pTopMenu->SetMode( TOPMENU_MODE_SHOP );

				GetCurrentSubPhase()->OnEntranceEnd();
				_SideMenu_Entrance_();
			}
			break;
		}
	}
	CMessageBoxManager * pManager = g_pFramework->GetMessageBoxManager();
	if( pManager->GetTopMsgBox() != nullptr)
	{
		pManager->RefreshTop();
	}

	// SubPhase 이동 시점에 팝업을 표시 한다.
	GetRoomMasterAwayCheckerManager()->MoveSubPhase();
}

void	UIMainFrame_V15::ExitSubPhase( void)
{
	switch(m_CurrentSubPhase)
	{
	case USP_INVENTORY:
	case USP_SHOP:
	case USP_GACHA:
	case USP_GIFTSHOP:
		_SideMenu_Exit_();
		break;
	}

	SetUICharaCamera(CL_IDLE);
}

void UIMainFrame_V15::UpdateLocation( void)
{
	i3::stack_wstring wstrText;

	switch( m_currentPhase)
	{
	case UIPHASE_SERVER_CHANNEL_LIST :	wstrText = GAME_RCSTRING( "STR_UI_SERVER_LIST");		break;
	default:
		{
			i3::stack_wstring wstrServer;
			i3::stack_wstring wstrRoom;

			//	현재 서버 위치 가져오기
			GetCurrentSeverInfo( wstrServer);

			// ReadyRoom이면
			if( m_currentPhase == UIPHASE_READY_ROOM)
			{
				ROOM_INFO_BASIC roomInfo;
				MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);

				// 방 번호를 표시
				//	몇번방
				i3::sprintf( wstrRoom, GAME_RCSTRING("STBL_IDX_ROOM_TEXT1"), roomInfo._Idx + 1);
				i3::sprintf( wstrText, L"%s - %s", wstrServer, wstrRoom);
			}
			else
				wstrText = wstrServer;
		}
		break;
	}

	UIFloatMiniProfile * pFrame = static_cast<UIFloatMiniProfile*>(GetFloating(UFW_MINIPROFILE));
	if( pFrame != nullptr && pFrame->isEnable())
	{
		pFrame->OnModifyCurrentPosition( wstrText);
	}
}