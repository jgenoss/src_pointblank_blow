#include "pch.h"
#include "UIMainFrame_V11.h"

#include "ChannelContext.h"
#include "UI_V11/UIPhaseLobby_V11.h"
#include "UI_V11/UITopMenu_V11.h"
#include "UI_V11/UIFloating_V11.h"
#include "UI_V11/UIPopup_V11.h"

#include "UI/UIPhaseSelectServer.h"
#include "UI_V11/UIFloatReadyRoomInfo_V11.h"
#include "UI_V11/UIPhaseReadyRoom_V11.h"
#include "UI_V11/UIFloatLobbyMapInfo_V11.h"
#include "UI_V11/UILobbyBanner_V11.h"
#include "UI_V11/UIPhaseProfile_V11.h"
#include "UI/UIPhaseShop.h"
#include "UI_V11/UIPopupInviteLobbyUser_V11.h"
#include "UI_V11/UIPopupMailBox_V11.h"
#include "UI_V11/UIPopupReceiveMail_V11.h"
#include "UI/UIPhaseInventory.h"
#include "UI_V11/UIPopupMissionCardInfo_V11.h"
#include "UI_V11/UIPopupMissionCardShop_V11.h"
#include "UI_V11/UIPopupRewardItem_V11.h"
#include "UI_V10/UIPopupMissionCardInfo_V10.h"
#include "UI_V11/UIPopupSendClanMail_V11.h"
#include "UI_V11/UIPopupReplyMail_V11.h"
#include "UI_V10/UIFloatCharacterView_V10.h"

#include "UIFloating.h"
#include "UIPopup.h"
#include "UIToolTip.h"

#include "UIServerAndChannelBanner.h"
#include "UINewAndHotBanner.h"
#include "UILobbyBanner.h"
#include "UILobbyClanInfo.h"
#include "UIPopupMedalRestriction.h"
#include "UIPhaseClanLobby.h"
#include "UIPhaseGiftShop.h"

#include "UITopMenu.h"
#include "UISideMenu.h"
#include "UIPhaseSubBase.h"

#include "UI_V10/UIPhaseTemp_V10.h"
#include "UIPhaseNonClanMember.h"

#include "UICharaInfoMgr.h"

#include "RoomMasterKickManager.h"

I3_CLASS_INSTANCE( UIMainFrame_V11);//, i3GameNode);

UIMainFrame_V11::UIMainFrame_V11(){

}

UIMainFrame_V11::~UIMainFrame_V11(){

}


/*virtual*/ void UIMainFrame_V11::OnLoadEnd(i3Stage* pStage)
{
	UIMainFrame::OnLoadEnd(pStage);

	INT32 i;
	m_pTopMenu = UITopMenu_V11::new_object();
	m_pTopMenu->OnCreate( pStage);

	m_pSideMenu = UISideMenu::new_object();
	m_pSideMenu->OnCreate( pStage);

	i3ClassMeta * pMeta = nullptr;

	// Create Phase
	m_Phase.resize(UIPHASE_MAX);
	m_SubPhase.resize(USP_MAX);
	m_FloatWins.resize(UFW_MAX);
	m_Popup.resize(UPW_MAX);


	for( i = 0; i < UIPHASE_MAX; i++)
	{
		switch( i)
		{
		case UIPHASE_SERVER_CHANNEL_LIST :		pMeta = UIPhaseSelectServer::static_meta();	break;
		//case UIPHASE_CREATE_CHARA :				pMeta = UIPhaseCreateChara::static_meta();		break;
		case UIPHASE_LOBBY :						pMeta = UIPhaseLobby_V11::static_meta();			break;
		case UIPHASE_CLANLOBBY :				pMeta = UIPhaseClanLobby::static_meta();		break;
		case UIPHASE_READY_ROOM :				pMeta = UIPhaseReadyRoom_V11::static_meta();		break;
			//case UIPHASE_CLAN :						pMeta = UIPhaseNonClanMember::static_meta();	break;
		default :
			pMeta = nullptr;
			I3PRINTLOG(I3LOG_NOTICE,  "Not Setted phase class...!!!");
			break;
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
		/*if (g_pEnvSet->m_nUIVersion > 0)
		{
			GameEventSender::i()->SetEvent(EVENT_GET_SYSTEM_INFO);
			GameEventSender::i()->SetEvent(EVENT_GET_MYOPTION_N);
		}
		else*/
		MovePhase( UIPHASE_SERVER_CHANNEL_LIST);
#endif
	}
	else
	{
		m_PhaseQueue.push_back( m_lastPhase);
		m_currentPhase = m_lastPhase;
	}


	for( i = 0; i < USP_MAX; i++)
	{
		pMeta = nullptr;
		switch( i)
		{
		case USP_PROFILE	:	pMeta = UIPhaseProfile_V11::static_meta();		break;
		case USP_SHOP		:   pMeta = UIPhaseShop::static_meta();			break;
		case USP_INVENTORY  :   pMeta = UIPhaseInventory::static_meta();	break;
		case USP_GIFTSHOP:		pMeta = UIPhaseGiftShop::static_meta(); break;
		case USP_TEMP :			pMeta = UIPhaseTemp_V10::static_meta();	break;

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

	for( i = 0; i < UFW_MAX; i++)
	{
		pMeta = nullptr;
		switch( i)
		{
		case UFW_CHARACTER :				pMeta = UIFloatCharacterView_V10::static_meta();	break;
		case UFW_NEWANDHOT:					pMeta = UINewAndHotBanner_V11::static_meta();		break;
		case UFW_MINIPROFILE:				pMeta = UIFloatMiniProfile_V11::static_meta();		break;
		case UFW_CHAT :						pMeta = UIFloatChat_V11::static_meta();				break;
		case UFW_READYROOMINFO :			pMeta = UIFloatReadyRoomInfo_V11::static_meta();	break;
		case UFW_LOBBYMAPINFO:				pMeta = UIFloatLobbyMapInfo_V11::static_meta();		break;
		case UFW_LOBBYBANNER:				pMeta = UILobbyBanner_V11::static_meta();			break;
		case UFW_CHANNEL_LIST:				pMeta = UIFloatChannelList::static_meta();			break;
		case UFW_SCBANNER :					pMeta = UIServerAndChannelBanner::static_meta();	break;
		case UFW_CLANLOBBY :				pMeta = UILobbyClanInfo::static_meta();		break;		
		case UFW_MERCENARY :				pMeta = UIFloatMercenaryList::static_meta();	break;
		case UFW_PLAYTIMEALARM :			pMeta = UIFloatPlayTimeAlarm::static_meta();	break;
		////case UFW_USERLIST:				pMeta = UIFloatUserList::static_meta();		break;

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

	for( i = 0; i < UPW_MAX; i++)
	{
		pMeta = nullptr;
		switch( i)
		{
		case UPW_CREATEROOM_EX :	pMeta = UIPopupCreateRoom::static_meta();			break;
		case UPW_USERPROFILE:		pMeta = UIPopupUserProfile_V11::static_meta();		break;
		case UPW_USERACTION :		pMeta = UIPopupUserAction_V11::static_meta();		break;
		case UPW_SENDMAIL :			pMeta = UIPopupSendMail_V11::static_meta();			break;
		case UPW_ADDBLOCK :			pMeta = UIPopupAddBlockList_V11::static_meta();		break;
		case UPW_INVITELOBBYUSER :  pMeta = UIPopupInviteLobbyUser_V11::static_meta();	break;
		case UPW_QUERY_PASSWORD :	pMeta = UIPopupQueryPassword::static_meta();		break;
		case UPW_OPTION :			pMeta = UIPopupOption::static_meta();				break;
		case UPW_TWITCHLOGIN :		pMeta = UIPopupTwitchLogin::static_meta();			break;
		case UPW_TWITCHOPTION :		pMeta = UIPopupTwitchOption::static_meta();			break;
		case UPW_PCCAFE:			pMeta = UIPopupPCCafe::static_meta();				break;
		case UPW_COUPON_INPUT:		pMeta = UIPopupCouponInput::static_meta();			break;
		
		//1.0 상점용 팝업들 대부분 1.5 구현되어있는 내용으로 맞춤.
		case UPW_BUYITEM :					pMeta = UIPopupBuyItem::static_meta();							break;
		case UPW_QUERY_ITEM :				pMeta = UIPopupQueryItem::static_meta();						break;
		case UPW_RESULT_USEITEM :			pMeta = UIPopupResultUseItem::static_meta();					break;	// 아이템 사용 결과 팝업
		case UPW_SHOPEXTENSION :			pMeta = UIPopupShopExtension::static_meta();					break;
		case UPW_DISGUISE_RANK :			pMeta = UIPopupUseItemDisguiseRank::static_meta();				break;	// 위장 계급
		case UPW_CHANGE_CLAN_MARK :			pMeta = UIPopupUseItemChangeClanMark::static_meta();			break;	// 클랜 마크 변경
		case UPW_CHANGENICK :				pMeta = UIPopupUseItemChangeNick::static_meta();				break;	// 닉네임 변경
		case UPW_CHANGE_CROSSHAIR_COLOR :	pMeta = UIPopupUseItemChangeCrosshairColor::static_meta();		break;	// 컬러 크로스헤어
		case UPW_SKILLOPEN_ITEM :			pMeta = UIPopupSkillOpen::static_meta();						break;
		case UPW_SKILLOPEN_RESULT :			pMeta = UIPopupSkillOpenResult::static_meta();					break;
		case UPW_CHANGE_COLOR_NICK:		    pMeta = UIPopupColorNickname::static_meta();					break;
		case UPW_COLOR_FIRE_MUZZLE_EFFECT : pMeta = UIPopupColorFireEffect::static_meta();					break;
		case UPW_INVEN_DELETE_ITEM_NEW:		pMeta = UIPopupInvenDeleteItem::static_meta();					break;

		// 쪽지함 - MailBox
		case UPW_MAILBOX :			pMeta = UIPopupMailBox_V11::static_meta();			break;
		case UPW_MAILREV :			pMeta = UIPopupReceiveMail_V11::static_meta();		break;
		case UPW_MAILREP :			pMeta = UIPopupReplyMail_V11::static_meta();		break;
		case UPW_EXIT_POPUP:		pMeta = UIPopupExit::static_meta();					break;
		case UPW_CREATENICKNAME :	pMeta = UIPopupNickNameSetting::static_meta();	break;
		case UPW_MISSIONINFO:		pMeta = UIPopupMissionCardInfo_V10::static_meta();	break;
		case UPW_MISSIONSHOP:		pMeta = UIPopupMissionCardShop_V11::static_meta();	break; 
		case UPW_REWARDITEM :				pMeta = UIPopupMissionReward::static_meta();					break;
		case UPW_REWARDITEM_FINALITEM :		pMeta = UIPopupMissionRewardFinalItem::static_meta();			break;
		case UPW_REWARDITEM_FINALMASTER :	pMeta = UIPopupMissionRewardFinalMaster::static_meta();			break;

		case UPW_BREAK_INTO_GAME :	pMeta = UIPopupBreakIntoGame::static_meta();				break;
		case UPW_TS_REWARD :		pMeta = UIPopupTSReward::static_meta();						break;
		case UPW_CLAN_NON_MEMBER :			pMeta = UIPhaseNonClanMember::static_meta();		break;
		case UPW_CLAN_MEMBER :				pMeta = UIPhaseClanMember::static_meta();			break;
		case UPW_RANK_UP :			pMeta = UIPopupRankup::static_meta();				break;
		case UPW_GM_CHATBOX :		pMeta = UIPopupGMChatBox::static_meta();			break;
		case UPW_GM_OPTION :		pMeta = UIPopupGMOption::static_meta();				break;

		case UPW_CLAN_CREATE :		pMeta = UIPopupCreateClan::static_meta();			break;
		case UPW_CLAN_JOIN :		pMeta = UIPopupJoinClan::static_meta();				break;
		case UPW_CLANHONOR :		pMeta = UIPopupClanHonor::static_meta();			break;	
		case UPW_CLAN_SEASON_OVERALL_RANKING:		pMeta = UIPopupClanSeason_OverallRanking::static_meta();		break;
		case UPW_CLAN_SEASON_INDIVISUAL_RANKING:	pMeta = UIPopupClanSeason_IndivisualRanking::static_meta();		break;
		case UPW_SEND_CLANMAIL:		pMeta = UIPopupSendClanMail_V11::static_meta();		break; 

		case UPW_QUICK_JOIN:		pMeta = UIPopupQuickJoin::static_meta();	break;
		case UPW_QUICK_JOIN_SECOND_BEST: pMeta = UIPopupQuickJoinSecondBest::static_meta(); break;

		case UPW_MONTH_ATTENDANCE :		pMeta = UIPopupMonthAttendance::static_meta();	break;
		case UPW_DORMANT_EVENT:			pMeta = UIPopupDormantEvent::static_meta();		break;

		case UPW_GIFT_SEND:			pMeta = UIPopupGiftSend::static_meta();			break;

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

/*virtual*/ void UIMainFrame_V11::OnFinish( void)
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

/*virtual*/void UIMainFrame_V11::StartPhase( UIPHASE MovePhase)
{
	UIMainFrame::StartPhase(MovePhase);

	// TopMenu의 버튼 구성을 바꿉니다.
	switch( m_currentPhase)
	{
	case UIPHASE_START :
	case UIPHASE_SERVER_CHANNEL_LIST:
		m_pTopMenu->SetMode( TOPMENU_MODE_SERVER);
		break;
	case UIPHASE_CREATE_CHARA:
		{
		}
		break;
	case UIPHASE_CLANLOBBY:
		{
			// Lobby입장에 대한 처리를 한다.
			m_pTopMenu->SetMode( TOPMENU_MODE_READY);
		}
		break;
	case UIPHASE_LOBBY:
		{
			// Lobby입장에 대한 처리를 한다.
			if(m_pTopMenu)
				m_pTopMenu->SetMode( TOPMENU_MODE_READY);
		}
		break;

	case UIPHASE_READY_ROOM:
		{
		if(m_pTopMenu != nullptr)
			m_pTopMenu->SetMode( TOPMENU_MODE_READYROOM);	
		}
		break;
	}

	UpdateLocation();

	m_lastPhase = m_currentPhase;
}

/*virtual*/void UIMainFrame_V11::EndPhase( void)
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
					//if( i == UFW_MINIPROFILE) continue;
					DisableFloatWin( (UIFLOATWIN) i);
				}

				_SideMenu_Exit_();
			}
			break;
		case UIPHASE_CLANLOBBY:
		case UIPHASE_LOBBY:
			{
				DisableFloatWin( UFW_MERCENARY );
			}
			break;
		case UIPHASE_READY_ROOM :
			{	// 대기방으로 들어가는 경우 배너를 닫는다.
				DisableFloatWin( UFW_CHARACTER);
				DisableFloatWin( UFW_LOBBYBANNER);
				DisableFloatWin( UFW_NEWANDHOT);
				DisableFloatWin( UFW_READYROOMINFO );
				DisableFloatWin( UFW_LOBBYMAPINFO );
				DisableFloatWin( UFW_CLANLOBBY);
				DisableFloatWin( UFW_MERCENARY );
				_SideMenu_Exit_();
			}
			break;
		case UIPHASE_CREATE_CHARA :
			{
				DisableFloatWin( UFW_CHARACTER);
				DisableFloatWin( UFW_CHAT);
				//DisableFloatWin( UFW_USERLIST);
				DisableFloatWin( UFW_NEWANDHOT);
				DisableFloatWin( UFW_READYROOMINFO );
				DisableFloatWin( UFW_LOBBYBANNER);
				DisableFloatWin( UFW_LOBBYMAPINFO );
				DisableFloatWin( UFW_MINIPROFILE );
				DisableFloatWin( UFW_CLANLOBBY);
				DisableFloatWin( UFW_MERCENARY );
				_SideMenu_Exit_();
			}
		}
		break;
	case UIPHASE_READY_ROOM:
		if( NextPhase == UIPHASE_LOBBY 
			|| NextPhase == UIPHASE_CLANLOBBY 
			|| NextPhase == UIPHASE_CLANLOBBY )
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

void UIMainFrame_V11::UpdateChatBuffer( void )
{
	UIFloatingBase * curfloat = GetFloating(UFW_CHAT);
	if(curfloat != nullptr)
	{
		((UIFloatChat_V11*)curfloat)->UpdateChatBuffer();
	}
}

void UIMainFrame_V11::UpdateMegaPhoneBuffer(void)
{
	UIFloatingBase * curfloat = GetFloating(UFW_CHAT);
	if (curfloat != nullptr)
	{
		((UIFloatChat_V11*)curfloat)->UpdateMegaPhoneBuffer();
	}
}

void UIMainFrame_V11::OpenUserActionPopup( void * pEventReceiver, INT32 slotIdx )
{
	bool resPopup = m_Popup[ UPW_USERACTION]->OnEntranceStart( pEventReceiver, &slotIdx );
	I3ASSERT(resPopup);
}

void UIMainFrame_V11::AddUserActionCommand( USER_CONTEXT_MENU cmd )
{
	UIPopupUserAction_V11 * pUAP = (UIPopupUserAction_V11*) m_Popup[ UPW_USERACTION];
	pUAP->AddCommand( cmd);
}

void UIMainFrame_V11::ResetUserActionPopup( void )
{
	UIPopupUserAction_V11 * pUAP = (UIPopupUserAction_V11*) m_Popup[ UPW_USERACTION];
	pUAP->ResetCommand();
}

void UIMainFrame_V11::_EnableFloatingList()
{
	switch (m_currentPhase)
	{
	case UIPHASE_START:
	case UIPHASE_SERVER_CHANNEL_LIST:
		EnableFloatWin(UFW_SCBANNER);
		//EnableFloatWin( UFW_MINIPROFILE);
		break;
	case UIPHASE_CREATE_CHARA:
	{
		EnableFloatWin(UFW_CHARACTER);
	}
	break;
	case UIPHASE_CLANLOBBY:
	{
		// Floating window를 활성화
		// Floating window를 활성화
		EnableFloatWin(UFW_CHAT);
		//EnableFloatWin(UFW_CHARACTER);
		EnableFloatWin(UFW_USERLIST);
		EnableFloatWin(UFW_MINIPROFILE);
		EnableFloatWin(UFW_NEWANDHOT);
		EnableFloatWin(UFW_MERCENARY);

		CharaInfoMgr::instance()->list_up();
		CharaInfoMgr::instance()->m_current_state.ctg = item_def::SCTG_WEAPON;
		CharaInfoMgr::instance()->m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

		UIFloatCharacterView_V10 * pCharaView = (UIFloatCharacterView_V10*)GetFloating(UFW_CHARACTER);
		if (pCharaView != nullptr)
			pCharaView->UpdateCharaSlot();
	}
	break;
	case UIPHASE_LOBBY:
	{
		// Floating window를 활성화
		EnableFloatWin(UFW_NEWANDHOT);
		EnableFloatWin(UFW_MINIPROFILE);

		EnableFloatWin(UFW_CHAT);
		//EnableFloatWin( UFW_USERLIST);
		EnableFloatWin(UFW_NEWANDHOT);

		if (ChannelContext::i()->GetBattleChannel())
		{
			//EnableFloatWin( UFW_CLANLOBBY);
			EnableFloatWin(UFW_MERCENARY);
		}
		else
		{

			EnableFloatWin(UFW_LOBBYBANNER);
		}

		CharaInfoMgr::instance()->list_up();
		CharaInfoMgr::instance()->m_current_state.ctg = item_def::SCTG_WEAPON;
		CharaInfoMgr::instance()->m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

		UIFloatCharacterView_V10 * pCharaView = (UIFloatCharacterView_V10*)GetFloating(UFW_CHARACTER);
		if (pCharaView != nullptr)
			pCharaView->UpdateCharaSlot();
	}
	break;

	case UIPHASE_READY_ROOM:
	{
		//EnableFloatWin(UFW_CHARACTER);
		EnableFloatWin(UFW_MINIPROFILE);
		EnableFloatWin(UFW_CHAT);
		//EnableFloatWin( UFW_USERLIST);
		EnableFloatWin(UFW_READYROOMINFO);

		CharaInfoMgr::instance()->list_up();
		CharaInfoMgr::instance()->m_current_state.ctg = item_def::SCTG_WEAPON;
		CharaInfoMgr::instance()->m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

		UIFloatCharacterView_V10 * pCharaView = (UIFloatCharacterView_V10*)GetFloating(UFW_CHARACTER);
		if (pCharaView != nullptr)
			pCharaView->UpdateCharaSlot();
	}
	break;
	}

	for (size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);

		if (pFloatWin != nullptr && pFloatWin->isEnable())
			pFloatWin->OnMovePhase(m_currentPhase);
	}
}

void UIMainFrame_V11::PreStartSubPhase( UISUBPHASE phase)
{
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
		case UIPHASE_LOBBY:
			DisableFloatWin( UFW_LOBBYBANNER);
			DisableFloatWin( UFW_NEWANDHOT);
			DisableFloatWin( UFW_LOBBYMAPINFO );
			break;

		case UIPHASE_READY_ROOM :
			DisableFloatWin( UFW_READYROOMINFO );
			break;
		}

		//진입 하는 SubPhase에 대해서 다르게 행동한다.
		switch( phase )
		{
		case USP_PROFILE:
			DisableFloatWin( UFW_NEWANDHOT);
			DisableFloatWin( UFW_READYROOMINFO);
			DisableFloatWin( UFW_CLANLOBBY);
			DisableFloatWin( UFW_MINIPROFILE);
			((UIFloatChat_V11*)m_FloatWins[ UFW_CHAT])->OnMinimizeStart();

			if(ChannelContext::i()->GetBattleChannel())
			{
				DisableFloatWin(UFW_MERCENARY);
			}
			break;
		case USP_INVENTORY:
		case USP_SHOP:
		case USP_GIFTSHOP:
		//case USP_ROULETTE_SHOP:
			EnableFloatWin( UFW_CHARACTER);
			DisableFloatWin( UFW_MINIPROFILE );
			((UIFloatChat_V11*)m_FloatWins[ UFW_CHAT])->OnMinimizeStart();

			if(ChannelContext::i()->GetBattleChannel())		
			{	
				DisableFloatWin(UFW_MERCENARY);
			}
			((UIFloatCharacterView_V10*)g_pFramework->GetUIMainframe()->GetFloating(UFW_CHARACTER))->DoUpdateCharacter();
			
			break;

		case USP_TEMP:	// 클랜 팝업을 띄우기 위한 빈 SubPhase
			DisableFloatWin( UFW_MERCENARY );
			DisableFloatWin( UFW_NEWANDHOT );
			DisableFloatWin( UFW_CLANLOBBY );

			if( m_FloatWins[ UFW_MINIPROFILE]->isEnable() == false)
				EnableFloatWin( UFW_MINIPROFILE);

			((UIFloatChat_V11*)m_FloatWins[ UFW_CHAT])->OnMaximizeStart();
			break;
		}

		GetCurrentPhase()->OnExitStart();

		if (m_Popup[UPW_SHOPEXTENSION]->isEnable())
			m_Popup[UPW_SHOPEXTENSION]->OnExitStart();
	}

	for (size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);
		if (pFloatWin != nullptr && pFloatWin->isEnable())
		{
			pFloatWin->OnMoveSubPhase(m_CurrentSubPhase);
		}
	}
}

void UIMainFrame_V11::EndSubPhase( UISUBPHASE phase)
{
	if( phase == USP_NONE)
	{
		GetCurrentPhase()->OnModifyPhase_EndSubPhase( m_currentPhase );
	
		// SubPhase가 끝나면 Phase에게 Input을 돌려준다.
		GetCurrentPhase()->SetInputDisable( false);			// 이것이 꼭 필요하지 않을수 있음.(11.10.20.수빈)

		if (false == m_Popup[UPW_SHOPEXTENSION]->isEnable())
		{
			// Phase를 Entrance 시키고 기타 Floatingwindow를 띄운다.
			if( GetCurrentPhase()->OnEntranceStart() )
			{
				//기본  Phase 처리 
				//어떤 SubPhase에서 나오는지 알 수 없기에 기본으로 나와야 하는 모든 Floating을 띄운다.
				switch( m_currentPhase)
				{
				case UIPHASE_LOBBY :
					{
						if( m_FloatWins[ UFW_MINIPROFILE]->isEnable() == false)
							EnableFloatWin( UFW_MINIPROFILE);

						if( m_FloatWins[ UFW_LOBBYBANNER]->isEnable() == false &&
							!ChannelContext::i()->GetBattleChannel()) // 클랜 채널일때는 배너가 출력되지 않습니다
							EnableFloatWin( UFW_LOBBYBANNER);

						if( LocaleValue::Enable("EnableLobbyNewHot") )
							EnableFloatWin( UFW_NEWANDHOT);

						if( ChannelContext::i()->GetBattleChannel() )
							EnableFloatWin(UFW_MERCENARY);

						DisableFloatWin(UFW_CHARACTER);
						((UIFloatChat_V11*)m_FloatWins[ UFW_CHAT])->OnMaximizeStart();
					}
					break;

				case UIPHASE_READY_ROOM :
					EnableFloatWin( UFW_READYROOMINFO);
					EnableFloatWin( UFW_MINIPROFILE);
					((UIFloatChat_V11*)m_FloatWins[ UFW_CHAT])->OnMaximizeStart();
					break;
				
				case UIPHASE_CLANLOBBY:
					if( LocaleValue::Enable("EnableLobbyNewHot") )
						EnableFloatWin( UFW_NEWANDHOT);

					EnableFloatWin( UFW_MINIPROFILE);
					EnableFloatWin(UFW_MERCENARY);
					((UIFloatChat_V11*)m_FloatWins[ UFW_CHAT])->OnMaximizeStart();
					break;
				}
			}
		}
	}
}

void	UIMainFrame_V11::EntranceSubPhase( void)
{
	if( GetCurrentSubPhase()->OnEntranceStart())
	{
		//SubPhase에서 특징적인 행동을 처리하면 된다.
		switch(m_CurrentSubPhase)
		{
		case USP_INVENTORY:
		case USP_SHOP:
		case USP_PROFILE:
		case USP_TEMP:
		case USP_GIFTSHOP:
			{
				if(m_pTopMenu != nullptr)
					m_pTopMenu->SetMode( TOPMENU_MODE_SHOP );

				//랜더링 순서 때문에 캐릭터 뷰 활성화 후에 진행되어야 한다. (USP_INVENTORY, USP_SHOP)
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

void	UIMainFrame_V11::ExitSubPhase( void)
{
	switch(m_CurrentSubPhase)
	{
	case USP_INVENTORY:
		DisableFloatWin( UFW_CHARACTER);
		break;
	case USP_SHOP:
		DisableFloatWin( UFW_CHARACTER);
		break;
	case USP_GIFTSHOP:
		DisableFloatWin(UFW_CHARACTER);
		break;
	//case USP_CLAN_NON_MEMBER:
	//case USP_CLAN_MEMBER:
	//	{
	//		DisableFloatWin( UFW_MINIPROFILE);
	//		((UIFloatChat*)m_pFloatWins[ UFW_CHAT])->OnMinimizeStart();
	//	}
	//	break;
		default:break;
	}
	_SideMenu_Exit_();
}

void	UIMainFrame_V11::InitItemImageMgr(void)
{
	//임시 부분 수정해야됨.
	ItemImageMgr_Init init;

	for (size_t i=0; i<g_pWeaponInfoDataBase->GetUITemplateInfoCount(); i++)
	{
		const CUITemplateInfo_Weapon* info = g_pWeaponInfoDataBase->GetUITemplateInfo(i);
		I3ASSERT(info != nullptr);

		if (info)
		{
			i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
			init.AddWeaponTemplate(strName.c_str());
		}
	}

	for (size_t i=0; i<g_pCharaInfoDataBase->GetUITemplateInfoCount_Chara(); i++)
	{
		const CUITemplateInfo_Chara* info = g_pCharaInfoDataBase->GetUITemplateInfo_Chara(i);
		I3ASSERT(info != nullptr);

		if (info)
		{
			init.AddCharacterTemplate(info->GetType(), info->GetLoopLoad(), info->GetName().c_str());
		}
	}

	for (size_t i=0; i<g_pCharaInfoDataBase->GetUITemplateInfoCount_Parts(); i++)
	{
		const CUITemplateInfo_Parts* info = 
			static_cast<const CUITemplateInfo_Parts*>(g_pCharaInfoDataBase->GetUITemplateInfo_Parts(i));
		I3ASSERT(info != nullptr);

		if (info)
		{
			i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
			init.AddPartsTemplate(info->GetType(), info->GetLoopLoad(), strName.c_str());
		}
	}

	//init.AddCharacterTemplate(CHARACTER_CLASS_CHARA_TYPE, "ButtonImageSet_Cha_hs");
	//init.AddCharacterTemplate(CHARACTER_CLASS_HEADGEAR, "ButtonImageSet_Item_Head");
	//init.AddCharacterTemplate(CHARACTER_CLASS_HEADGEAR, "ButtonImageSet_Item_Head_01");
	//init.AddCharacterTemplate(CHARACTER_CLASS_HEADGEAR, "ButtonImageSet_Item_Beret");		// trunk/Media/UIRe/Image/Item 템플릿 이미지 정리되면서 베레모 item_Beret.tga로 통합
	//init.AddCharacterTemplate(CHARACTER_CLASS_HEADGEAR, "ButtonImageSet_Item_Beret_New2");	// 이 부분은 아직 확신이 들지 않아 일단 주석.
	//init.AddCharacterTemplate(CHARACTER_CLASS_FACEGEAR, "BtImgSet_ItemMask");

	for (size_t i=0; i<g_pShopItemInfoDataBase->GetUITemplateCount(); i++)
	{
		const CUITemplateInfo_Item* info = 
			static_cast<CUITemplateInfo_Item*>(g_pShopItemInfoDataBase->GetUITemplateInfo(i));
		I3ASSERT(info != nullptr);

		if (info)
		{
			i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
			init.AddItemTemplate(info->GetType(), strName.c_str());
		}
	}

	GetItemImageMgr()->Initialize(init);
}
