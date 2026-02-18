#include "pch.h"
#include "UIMainFrame_V10.h"

#include "ChannelContext.h"
#include "UIPopupUserProfile.h"
#include "UI_V10/UIPhaseLobby_V10.h"
#include "UI_V10/UIPhaseReadyRoom_V10.h"
#include "UI/UIPhaseSelectServer.h"
#include "UI_V10/UITopMenu_V10.h"
#include "UI_V10/UIFloating_V10.h"
#include "UI_V10/UIPopup_V10.h"
#include "UI_V10/UIFloatCharacterView_V10.h"
#include "UI_V10/UIFloatCommunity.h"
#include "UI_V10/UIPopupDesigSelect_V10.h"

#include "UIServerAndChannelBanner.h"
#include "UINewAndHotBanner.h"
#include "UILobbyBanner.h"
#include "UILobbyClanInfo.h"
#include "UIPopupMedalRestriction.h"

#include "UIPopup.h"
#include "UIFloating.h"
#include "UIToolTip.h"

#include "UI_V10/UIPhaseProfile_V10.h"
#include "UISideMenu.h"
#include "UIPhaseSubBase.h"
#include "UIPhaseShop.h"
#include "UIPhaseInventory.h"
#include "UIPhaseClanLobby.h"
#include "UIFloatMercenaryList.h"
#include "UIPhaseNonClanMember.h"
#include "UIPhaseClanMember.h"
#include "UI_V10/UIPhaseTemp_V10.h"
#include "UI_V10/UIPopupReceiveMail_V10.h"

#include "UICharaInfoMgr.h"

#include "RoomMasterKickManager.h"

I3_CLASS_INSTANCE( UIMainFrame_V10);//, i3GameNode);

UIMainFrame_V10::UIMainFrame_V10(){

}

UIMainFrame_V10::~UIMainFrame_V10(){

}


/*virtual*/ void UIMainFrame_V10::OnLoadEnd(i3Stage* pStage)
{
	UIMainFrame::OnLoadEnd(pStage);

	INT32 i;
	m_pTopMenu = UITopMenu_V10::new_object();
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
		case UIPHASE_LOBBY :						pMeta = UIPhaseLobby_V10::static_meta();			break;
		case UIPHASE_CLANLOBBY :				pMeta = UIPhaseClanLobby::static_meta();		break;
		case UIPHASE_READY_ROOM :				pMeta = UIPhaseReadyRoom_V10::static_meta();		break;
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
		case USP_SHOP		: pMeta = UIPhaseShop::static_meta();		break;
		case USP_INVENTORY	: pMeta = UIPhaseInventory::static_meta();		break;
		case USP_PROFILE	: pMeta = UIPhaseProfile_V10::static_meta();	break;
		case USP_TEMP		: pMeta = UIPhaseTemp_V10::static_meta();	break;

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
		//case UFW_NEWANDHOT:					pMeta = UINewAndHotBanner_V11::static_meta();	break;
		//case UFW_MINIPROFILE:				pMeta = UIFloatMiniProfile_V11::static_meta();	break;
		case UFW_SCBANNER :					pMeta = UIServerAndChannelBanner::static_meta();	break;
		case UFW_MISSIONBOARD:				pMeta = UIFloatMissionBoard_V10::static_meta();		break;
		case UFW_CHAT :						pMeta = UIFloatChat_V10::static_meta();				break;
		case UFW_READYROOM_CHAT :			pMeta = UIFloatReadyRoomChat_V10::static_meta();	break;
		case UFW_USERLIST:					pMeta = UIFloatUserList_V10::static_meta();			break;
		case UFW_CHANNEL_LIST:				pMeta = UIFloatChannelList::static_meta();			break;
		case UFW_READYROOM_USERINFO:		pMeta = UIFloatReadyRoomUserInfo::static_meta();	break;
		case UFW_CLANLOBBY :				pMeta = UILobbyClanInfo::static_meta();		break;		
		case UFW_MERCENARY :				pMeta = UIFloatMercenaryList::static_meta();	break;
		case UFW_PLAYTIMEALARM :			pMeta = UIFloatPlayTimeAlarm::static_meta();	break;
		case UFW_CHARACTER :				pMeta = UIFloatCharacterView_V10::static_meta();	break;
		case UFW_COMMUNITY :				pMeta = UIFloatCommunity::static_meta();	break;
		
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
		case UPW_USERACTION :		pMeta = UIPopupUserAction_V10::static_meta();		break;
		case UPW_CREATEROOM_EX :	pMeta = UIPopupCreateRoom::static_meta();			break;
		case UPW_USERPROFILE :		pMeta = UIPopupUserProfile_V10::static_meta();			break;
		case UPW_MAILREV :			pMeta = UIPopupReceiveMail_V10::static_meta();		break;
		case UPW_SENDMAIL :			pMeta = UIPopupSendMail_V11::static_meta();			break;
		case UPW_ADDBLOCK :			pMeta = UIPopupAddBlockList_V11::static_meta();		break;
		case UPW_INVITELOBBYUSER :	pMeta = UIPopupInviteLobbyUser::static_meta();		break;
		case UPW_QUERY_PASSWORD :	pMeta = UIPopupQueryPassword::static_meta();		break;
		case UPW_OPTION :			pMeta = UIPopupOption::static_meta();				break;
		case UPW_EXIT_POPUP :		pMeta = UIPopupExit::static_meta();					break;
		case UPW_PCCAFE :			pMeta = UIPopupPCCafe::static_meta();				break;
		
		//1.0 상점용 팝업들 대부분 1.5 구현되어있는 내용으로 맞춤.
		case UPW_BUYITEM :					pMeta = UIPopupBuyItem::static_meta();			break; 
		case UPW_SHOPEXTENSION :			pMeta = UIPopupShopExtension::static_meta();						break;
		case UPW_QUERY_ITEM :				pMeta = UIPopupQueryItem::static_meta();							break;
		case UPW_RESULT_USEITEM :			pMeta = UIPopupResultUseItem::static_meta();						break;
		case UPW_CHANGENICK :				pMeta = UIPopupUseItemChangeNick::static_meta();					break;	// 닉네임 변경
		case UPW_CHANGE_CROSSHAIR_COLOR :	pMeta = UIPopupUseItemChangeCrosshairColor::static_meta();			break;	// 컬러 크로스헤어
		case UPW_DISGUISE_RANK :			pMeta = UIPopupUseItemDisguiseRank::static_meta();					break;	// 위장 계급
		case UPW_CHANGE_CLAN_MARK :			pMeta = UIPopupUseItemChangeClanMark::static_meta();				break;	// 클랜 마크 변경
		case UPW_SKILLOPEN_ITEM :			pMeta = UIPopupSkillOpen::static_meta();							break;
		case UPW_SKILLOPEN_RESULT :			pMeta = UIPopupSkillOpenResult::static_meta();						break;
		case UPW_CHANGE_COLOR_NICK:		    pMeta = UIPopupColorNickname::static_meta();						break;
		case UPW_COLOR_FIRE_MUZZLE_EFFECT : pMeta = UIPopupColorFireEffect::static_meta();						break;
		case UPW_INVEN_DELETE_ITEM_NEW:		pMeta = UIPopupInvenDeleteItem::static_meta();					break;

		case UPW_MISSIONSHOP :				pMeta = UIPopupMissionCardShop_V10::static_meta();					break;
		case UPW_MISSIONINFO :				pMeta = UIPopupMissionCardInfo_V10::static_meta();					break;
		case UPW_CREATENICKNAME:			pMeta = UIPopupNickNameSetting::static_meta();						break;
		case UPW_TWITCHLOGIN :				pMeta = UIPopupTwitchLogin::static_meta();			break;
		case UPW_TWITCHOPTION :				pMeta = UIPopupTwitchOption::static_meta();			break;
		case UPW_BREAK_INTO_GAME :			pMeta = UIPopupBreakIntoGame::static_meta();		break;
		case UPW_TS_REWARD :				pMeta = UIPopupTSReward::static_meta(); break;
		case UPW_CLAN_CREATE :				pMeta = UIPopupCreateClan::static_meta();			break;
		case UPW_CLAN_JOIN :				pMeta = UIPopupJoinClan::static_meta();				break;
		case UPW_CLAN_SEASON_OVERALL_RANKING:		pMeta = UIPopupClanSeason_OverallRanking::static_meta();		break;
		case UPW_CLAN_SEASON_INDIVISUAL_RANKING:	pMeta = UIPopupClanSeason_IndivisualRanking::static_meta();		break;
		case UPW_CLAN_NON_MEMBER :			pMeta = UIPhaseNonClanMember::static_meta();		break;
		case UPW_CLAN_MEMBER :				pMeta = UIPhaseClanMember::static_meta();			break;
		case UPW_CLANHONOR :				pMeta = UIPopupClanHonor::static_meta();			break;

		case UPW_REWARDITEM :				pMeta = UIPopupMissionReward::static_meta();					break;
		case UPW_REWARDITEM_FINALITEM :		pMeta = UIPopupMissionRewardFinalItem::static_meta();			break;
		case UPW_REWARDITEM_FINALMASTER :	pMeta = UIPopupMissionRewardFinalMaster::static_meta();			break;
		case UPW_RANK_UP :					pMeta = UIPopupRankup::static_meta();				break;
		case UPW_GM_CHATBOX :				pMeta = UIPopupGMChatBox::static_meta();			break;
		case UPW_GM_OPTION :				pMeta = UIPopupGMOption::static_meta();				break;
		case UPW_USERSEARCH :				pMeta = UIPopupUserSearch_V10::static_meta();		break;
		
		case UPW_QUICK_JOIN:				pMeta = UIPopupQuickJoin::static_meta();	break;
		case UPW_QUICK_JOIN_SECOND_BEST:	pMeta = UIPopupQuickJoinSecondBest::static_meta(); break;

		case UPW_MONTH_ATTENDANCE :			pMeta = UIPopupMonthAttendance::static_meta();	break;
		case UPW_DORMANT_EVENT:				pMeta = UIPopupDormantEvent::static_meta();		break;
		case UPW_DESIG_SELECT:				pMeta = UIPopupDesigSelect_V10::static_meta();	break;
		
		case UPW_WEB_BOX:					pMeta = UIPopupWebBox::static_meta();			break;

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

/*virtual*/ void UIMainFrame_V10::OnFinish( void)
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

/*virtual*/void UIMainFrame_V10::StartPhase( UIPHASE MovePhase)
{
	UIMainFrame::StartPhase(MovePhase);
	GetTopMenu_V10()->SetEnableCommunity(false);

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
			m_pTopMenu->SetMode( TOPMENU_MODE_READYROOM);	
		}
		break;
	}

	UpdateLocation();

	m_lastPhase = m_currentPhase;
}

/*virtual*/void UIMainFrame_V10::EndPhase( void)
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
		case UIPHASE_LOBBY:
			{
				// 껏다가 다시 킵니다.
				DisableFloatWin( UFW_CHAT);
				DisableFloatWin( UFW_USERLIST);
				DisableFloatWin( UFW_MERCENARY );
				DisableFloatWin( UFW_READYROOM_USERINFO);
			}
			break;
		case UIPHASE_READY_ROOM :
			{	// 대기방으로 들어가는 경우 배너를 닫는다.
				//DisableFloatWin( UFW_CHARACTER);
				DisableFloatWin( UFW_LOBBYBANNER);
				DisableFloatWin( UFW_NEWANDHOT);
				DisableFloatWin( UFW_CLANLOBBY);
				DisableFloatWin( UFW_MERCENARY );
				
				// 껏다가 다시 킵니다.
				DisableFloatWin( UFW_CHAT);
				DisableFloatWin( UFW_USERLIST);
				_SideMenu_Exit_();
			}
			break;
		case UIPHASE_CREATE_CHARA :
			{
				//DisableFloatWin( UFW_CHARACTER);
				DisableFloatWin( UFW_CHAT);
				DisableFloatWin( UFW_USERLIST);
				DisableFloatWin( UFW_NEWANDHOT);
				DisableFloatWin( UFW_MINIPROFILE );
				DisableFloatWin( UFW_CLANLOBBY);
				DisableFloatWin( UFW_MISSIONBOARD);
				DisableFloatWin( UFW_READYROOM_USERINFO);
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
			DisableFloatWin( UFW_READYROOM_USERINFO);
			
			// 껏다가 다시 킵니다.
			DisableFloatWin( UFW_READYROOM_CHAT);
			DisableFloatWin( UFW_USERLIST);
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

void UIMainFrame_V10::UpdateChatBuffer( void )
{
	if(m_currentPhase != UIPHASE_READY_ROOM)
	{
		UIFloatingBase * curfloat = GetFloating(UFW_CHAT);
		if(curfloat != nullptr)
		{
			((UIFloatChat_V10*)curfloat)->UpdateChatBuffer();
		}
	}
	else
	{
		UIFloatingBase * curfloat = GetFloating(UFW_READYROOM_CHAT);
		if(curfloat != nullptr)
		{
			((UIFloatChat_V10*)curfloat)->UpdateChatBuffer();
		}
	}
}

void UIMainFrame_V10::UpdateMegaPhoneBuffer(void)
{
	if (m_currentPhase != UIPHASE_READY_ROOM)
	{
		UIFloatingBase * curfloat = GetFloating(UFW_CHAT);
		if (curfloat != nullptr)
		{
			((UIFloatChat_V10*)curfloat)->UpdateMegaPhoneBuffer();
		}
	}
	else
	{
		UIFloatingBase * curfloat = GetFloating(UFW_READYROOM_CHAT);
		if (curfloat != nullptr)
		{
			((UIFloatChat_V10*)curfloat)->UpdateMegaPhoneBuffer();
		}
	}
}

void UIMainFrame_V10::OpenUserActionPopup( void * pEventReceiver, INT32 slotIdx )
{
	UIPopupUserAction_V10 * pUAP = (UIPopupUserAction_V10*)m_Popup[UPW_USERACTION];

	if (pUAP->IsEmptyCommand()) return;

	bool resPopup = m_Popup[ UPW_USERACTION]->OnEntranceStart( pEventReceiver, &slotIdx );
	I3ASSERT(resPopup);
}

void UIMainFrame_V10::AddUserActionCommand( USER_CONTEXT_MENU cmd )
{
	UIPopupUserAction_V10 * pUAP = (UIPopupUserAction_V10*) m_Popup[ UPW_USERACTION];
	pUAP->AddCommand( cmd);
}

void UIMainFrame_V10::ResetUserActionPopup( void )
{
	UIPopupUserAction_V10 * pUAP = (UIPopupUserAction_V10*) m_Popup[ UPW_USERACTION];
	pUAP->ResetCommand();
}

void UIMainFrame_V10::_EnableFloatingList()
{
	switch( m_currentPhase)
	{
	case UIPHASE_START :
	case UIPHASE_SERVER_CHANNEL_LIST:
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
			SetUICharaCamera(CL_IDLE);

			// Floating window를 활성화
			EnableFloatWin( UFW_CHAT);
			EnableFloatWin( UFW_CHARACTER);
			EnableFloatWin( UFW_MISSIONBOARD);
			EnableFloatWin( UFW_USERLIST);
			EnableFloatWin( UFW_CLANLOBBY);
			EnableFloatWin( UFW_MERCENARY);
			//EnableFloatWin( UFW_COMMUNITY);

			CharaInfoMgr::instance()->list_up();
			CharaInfoMgr::instance()->m_current_state.ctg = item_def::SCTG_WEAPON;
			CharaInfoMgr::instance()->m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

			UIFloatCharacterView_V10 * pCharaView = (UIFloatCharacterView_V10*) GetFloating( UFW_CHARACTER);
			if( pCharaView != nullptr)
				pCharaView->UpdateCharaSlot();
		}
		break;
	case UIPHASE_LOBBY:
		{
			SetUICharaCamera(CL_IDLE);
			// Floating window를 활성화
			EnableFloatWin( UFW_NEWANDHOT );
			EnableFloatWin( UFW_MINIPROFILE );
			EnableFloatWin( UFW_CHARACTER );
			EnableFloatWin( UFW_MISSIONBOARD);
			EnableFloatWin( UFW_CHAT);
			EnableFloatWin( UFW_USERLIST);
			//EnableFloatWin( UFW_COMMUNITY);

			if( ChannelContext::i()->GetBattleChannel() )
			{
				//EnableFloatWin( UFW_CLANLOBBY);
				EnableFloatWin( UFW_MERCENARY);
			}
			else
			{
				EnableFloatWin( UFW_NEWANDHOT);
			}

			CharaInfoMgr::instance()->list_up();
			CharaInfoMgr::instance()->m_current_state.ctg = item_def::SCTG_WEAPON;
			CharaInfoMgr::instance()->m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

			UIFloatCharacterView_V10 * pCharaView = (UIFloatCharacterView_V10*) GetFloating( UFW_CHARACTER);
			if( pCharaView != nullptr)
				pCharaView->UpdateCharaSlot();
		}
		break;

	case UIPHASE_READY_ROOM:
		{
			SetUICharaCamera(CL_IDLE);
			EnableFloatWin( UFW_CHARACTER);
			EnableFloatWin( UFW_MINIPROFILE);
			EnableFloatWin( UFW_MISSIONBOARD);
			EnableFloatWin( UFW_READYROOM_CHAT);
			//EnableFloatWin( UFW_USERLIST);
			EnableFloatWin( UFW_READYROOMINFO);
			EnableFloatWin( UFW_READYROOM_USERINFO);
			//EnableFloatWin( UFW_COMMUNITY);

			CharaInfoMgr::instance()->list_up();
			CharaInfoMgr::instance()->m_current_state.ctg = item_def::SCTG_WEAPON;
			CharaInfoMgr::instance()->m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

			UIFloatCharacterView_V10 * pCharaView = (UIFloatCharacterView_V10*) GetFloating( UFW_CHARACTER);
			if( pCharaView != nullptr)
				pCharaView->UpdateCharaSlot();
		}
		break;
	}

	for(size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);

		if( pFloatWin != nullptr && pFloatWin->isEnable())
			pFloatWin->OnMovePhase( m_currentPhase);
	}
}

void UIMainFrame_V10::PreStartSubPhase( UISUBPHASE phase)
{
	m_SubOldPhase = m_CurrentSubPhase;

	if( phase == m_CurrentSubPhase)
		m_CurrentSubPhase = USP_NONE;
	else
		m_CurrentSubPhase = phase;

	GetTopMenu_V10()->SetEnableCommunity(false);

	if( USP_NONE != m_CurrentSubPhase)
	{
		//기본  Phase 처리
		switch( m_currentPhase)
		{
		case UIPHASE_LOBBY:
			DisableFloatWin( UFW_LOBBYBANNER);
			DisableFloatWin( UFW_NEWANDHOT);
			DisableFloatWin( UFW_LOBBYMAPINFO );
			DisableFloatWin( UFW_CHAT);
			DisableFloatWin( UFW_USERLIST);
			DisableFloatWin( UFW_READYROOM_USERINFO);
			break;

		case UIPHASE_READY_ROOM :
			DisableFloatWin( UFW_READYROOMINFO );
			DisableFloatWin( UFW_READYROOM_CHAT);
			break;
		}

		//진입 하는 SubPhase에 대해서 다르게 행동한다.
		switch( phase )
		{
		case USP_TEMP:
			SetUICharaCamera(CL_WEAPON);
			if(ChannelContext::i()->GetBattleChannel())
			{
				DisableFloatWin(UFW_MERCENARY);
				DisableFloatWin(UFW_CHAT);
				DisableFloatWin(UFW_USERLIST);
			}
			break;
		case USP_PROFILE:
			DisableFloatWin( UFW_NEWANDHOT);
			DisableFloatWin( UFW_READYROOMINFO);
			DisableFloatWin( UFW_CLANLOBBY);
			DisableFloatWin( UFW_READYROOM_CHAT);
			DisableFloatWin( UFW_CHAT);
			DisableFloatWin( UFW_USERLIST);
			DisableFloatWin( UFW_MINIPROFILE);
			DisableFloatWin( UFW_READYROOM_USERINFO);

			SetUICharaCamera(CL_WEAPON);

			if(ChannelContext::i()->GetBattleChannel())
			{
				DisableFloatWin(UFW_MERCENARY);
			}
			break;
		case USP_INVENTORY:
		case USP_SHOP:
			DisableFloatWin(UFW_READYROOM_USERINFO);
			SetUICharaCamera(CL_SHOP);
			if(ChannelContext::i()->GetBattleChannel())
			{
				DisableFloatWin(UFW_MERCENARY);
				DisableFloatWin(UFW_CHAT);
				DisableFloatWin(UFW_USERLIST);
			}
			break;
		default:
			SetUICharaCamera(CL_IDLE);
			break;
		}

		GetCurrentPhase()->OnExitStart();

		if (m_Popup[UPW_SHOPEXTENSION]->isEnable())
			m_Popup[UPW_SHOPEXTENSION]->OnExitStart();
	}
}

void UIMainFrame_V10::EndSubPhase( UISUBPHASE phase)
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
				_EnableFloatingList();
				////기본  Phase 처리 
				////어떤 SubPhase에서 나오는지 알 수 없기에 기본으로 나와야 하는 모든 Floating을 띄운다.
				//switch( m_currentPhase)
				//{
				//case UIPHASE_LOBBY :
				//	{
				//		EnableFloatWin( UFW_NEWANDHOT );
				//		EnableFloatWin( UFW_MINIPROFILE );
				//		EnableFloatWin( UFW_CHAT);
				//		//EnableFloatWin( UFW_CHARACTER );
				//		EnableFloatWin( UFW_USERLIST);

				//		if( LocaleValue::Enable("EnableLobbyNewHot") )
				//			EnableFloatWin( UFW_NEWANDHOT);

				//		//((UIFloatChat_V10*)m_FloatWins[ UFW_CHAT])->OnMaximizeStart();
				//	}
				//	break;

				//case UIPHASE_READY_ROOM :
				//	EnableFloatWin( UFW_READYROOMINFO);
				//	EnableFloatWin( UFW_MINIPROFILE);
				//	EnableFloatWin( UFW_CHAT);
				//	//((UIFloatChat_V10*)m_FloatWins[ UFW_CHAT])->OnMaximizeStart();
				//	break;

				//	//case UIPHASE_CLANMATCH:
				//	//	EnableFloatWin( UFW_MINIPROFILE);
				//	//	((UIFloatChat*)m_FloatWins[ UFW_CHAT])->OnMaximizeStart();
				//	//	break;
				//}
			}
		}
	}
}

void	UIMainFrame_V10::EntranceSubPhase( void)
{
	if( GetCurrentSubPhase()->OnEntranceStart())
	{
		//EnableFloatWin( UFW_COMMUNITY);
		//SubPhase에서 특징적인 행동을 처리하면 된다.
		switch(m_CurrentSubPhase)
		{
		case USP_INVENTORY:
		case USP_SHOP:
		case USP_PROFILE:
		case USP_TEMP:
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

void	UIMainFrame_V10::ExitSubPhase( void)
{
}

void	UIMainFrame_V10::InitItemImageMgr(void)
{
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

	GetItemImageMgr()->Initialize(init);
}
