#include "pch.h"
#include "UIBattleFrame.h"

#include "UIBattle.h"

#include "UIPopupItemBase.h"
#include "UIShopFunction.h"
#include "UIUtil.h"

#include "../IGShopContext.h"
#include "../StageBattle.h"

#include "BattleGui.h"
#include "BattleHud.h"
#include "Minimap.h"

#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleServerContext.h"

#include "UIHudMultiWeapon.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"
#include "../ObserverBattle.h"

I3_CLASS_INSTANCE( UIBattleFrame); //, i3GameNode);

namespace
{
	class UIPopupFocusCallback_BattleFrame : public UIPopupFocusCallback
	{
	public:
		virtual void	ProcessInputDisableMainUI(bool bEnable)
		{
			UIBattleFrame::i()->InputDisableMainUI(bEnable);
		}
		virtual void	ProcessInputDisableFloatWindow(bool bEnable)
		{

		}
	} g_popup_focus_cb;
}


UIBattleFrame::UIBattleFrame() : m_shop_ctrl(nullptr), m_numRenderPopup(0)
{	
	m_popup_focus.SetCallback(&g_popup_focus_cb);

	m_shop_ctrl = new igshop_ctrl;
	MEMDUMP_NEW( m_shop_ctrl, sizeof( igshop_ctrl));

	i3mem::FillZero( m_pHUD, sizeof( m_pHUD));
	i3mem::FillZero( m_pPopup, sizeof( m_pPopup));

	m_InfoEquipWeapon.Reset();

	RegisterGameEventFunc( EVENT_OPEN_IGSHOP,				&UIBattleFrame::ReceiveGameEvent_Open_IGShop);
	RegisterGameEventFunc( EVENT_KICK_VOTE_SUGGEST_RESULT,	&UIBattleFrame::ReceiveGameEvent_KickVote_Suggest_Result);
	RegisterGameEventFunc( EVENT_KICK_VOTE_START,			&UIBattleFrame::ReceiveGameEvent_KickVote_Start);
	RegisterGameEventFunc( EVENT_KICK_VOTE,					&UIBattleFrame::ReceiveGameEvent_KickVote_Update);
}

UIBattleFrame::~UIBattleFrame()
{
	// 장착장비를 원상 복구시켜준다..로비로 나가면 야전상점구입무기의 장비는 해제...

	if (LocaleValue::Enable("FieldShop"))
	{

		if( BattleServerContext::i()->IsMovingStage() == false)
		{
			UserInfoContext::i()->SetMyUserInfoEquipmentWeapon(m_InfoEquipWeapon);
		}
	}

	INT32 i;

	for( i = 0; i < UIHUD_MAX; i++)
	{
		if(m_pHUD[ i])
			m_pHUD[ i]->Destroy();
	}

	for( i = 0; i < UBP_MAX; i++)
	{
		if(m_pPopup[ i])
			m_pPopup[ i]->Destroy();
	}

	I3_SAFE_DELETE( m_shop_ctrl);

	minimap_new::Minimap::Destroy();

	GameUI::ReleaseLoadingIcon();

	i3VirtualTexture* pVTex = i3UI::getVirtualTexture();
	pVTex->getCache()->TrashAllTexture();
}


void	UIBattleFrame::FocusPopup(UIPopupBase* popup, bool bFocus)
{
	if (bFocus)
		m_popup_focus.AddFocusPopup(popup);
	else
		m_popup_focus.RemoveFocusPopup(popup);
}

void	UIBattleFrame::InputDisableMainUI(bool bEnable)
{
	if( bEnable)
		g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);
	else
		g_pFramework->SetGameKeyMode( GAMEKEY_MODE_GAME);
}

void    UIBattleFrame::InputDisable(bool bEnable)
{
	INT32 i;

	for( i = 0; i < UIHUD_MAX; i++)
	{
		if(m_pHUD[ i])
			m_pHUD[ i]->SetInputDisable(bEnable);
	}
	for( i = 0; i < UBP_MAX; i++)
	{
		if(m_pPopup[ i])
			m_pPopup[ i]->SetInputDisable(bEnable);
	}
}

void UIBattleFrame::OnCreate( void)
{
	m_numRenderPopup = 0;
	INT32 i;

	i3ClassMeta * pMeta = nullptr;

	if( g_pEnvSet->m_nUIVersion == UFV_1_5 || g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		// Create HUD
		for( i = 0; i < UIHUD_MAX; i++)
		{
			pMeta = nullptr;

			switch(i)
			{
			case UIHUD_FORCE_REMOVE:	pMeta = UIHUDForceRemove::static_meta(); break;
			case UIHUD_NOTICE_SHOP:	pMeta = UIHUDNoticeShop::static_meta();	 break;
			case UIHUD_RECORD_BOARD:	pMeta = UIHUDRecordBoard::static_meta();	break;
			case UIHUD_INGAMECHATING:	pMeta = UIHUDIngameChat::static_meta(); break;
			case UIHUD_MANAGER:			pMeta = UIHudManager::static_meta();	break;
			case UIHUD_PLAYTIME_ALARM:	pMeta = UIHUDPlayTimeAlarm::static_meta(); break;
			case UIHUD_TABMINIMAP:		pMeta = UIHUDTabMinimap::static_meta();	break;
			case UIHUD_OBSERVER:		pMeta = UIHUDObserver::static_meta(); break;
//			default :					pMeta = nullptr; break;
			}

			if( pMeta)
			{
				m_pHUD[ i] = (UIHUDBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
				I3_MUST_ADDREF(m_pHUD[ i]);
				m_pHUD[ i]->OnCreate( this);
			}
		}

		pMeta = nullptr;

		// Create Popup
		for( i = 0; i < UBP_MAX; i++)
		{
			switch(i)
			{
			case UBP_ACCUSE_CRIME :		pMeta = UIBattlePopupAccuseCrime::static_meta(); break;
#ifdef USE_REPORT
			case UBP_USER_REPORT :		pMeta = UIBattlePopupUserReport::static_meta(); break;
#endif
			//case UBP_INGAME_SHOP:			pMeta = UIBattlePopupShop::static_meta(); break;
			
			//case UBP_BUYITEM:				pMeta = UIBattlePopupBuyItem::static_meta(); break;
			//case UBP_DELETEITEM:			pMeta = UIBattlePopupDeleteItem::static_meta(); break;
			
			case UBP_SELECT_WEAPON:			pMeta = UIBattlePopupSelectWeapon::static_meta(); break;
			case UBP_SELECT_CHARA:			pMeta = UIBattlePopupSelectChara::static_meta(); break;
			case UBP_OPTION:				pMeta = UIPopupOption::static_meta(); break;
			case UBP_EXIT :					pMeta = UIBattlePopupExit::static_meta(); break;
			case UBP_HUD_USURPATION:		pMeta = UIHudUsurpation::static_meta(); break;
			case UBP_HUD_ESCAPE:			pMeta = UIHUDEscape::static_meta(); break;
			// Domination용
			case UBP_SKILL_SHOP:			break;

			case UBP_TWITCHOPTION :			pMeta = UIPopupTwitchOption::static_meta(); break;

			case UBP_HUD_SPACE:				pMeta = UIHudSpace::static_meta(); break;

			default :						pMeta = nullptr; break;
			}

			if( pMeta)
			{
				m_pPopup[ i] = (UIBattlePopupBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
				I3_MUST_ADDREF(m_pPopup[ i]);
				m_pPopup[ i]->OnCreate( this);
			}
		}
	}
	else
	{
		// Create HUD
		for( i = 0; i < UIHUD_MAX; i++)
		{
			switch(i)
			{
			case UIHUD_FORCE_REMOVE:	pMeta = UIHUDForceRemove::static_meta(); break;
				//case UIHUD_NOTICE_SHOP:	pMeta = UIHUDNoticeShop::static_meta();	 break;
			case UIHUD_RECORD_BOARD:	pMeta = UIHUDRecordBoard::static_meta();	break;
			case UIHUD_INGAMECHATING:	pMeta = UIHUDIngameChat::static_meta(); break;
			case UIHUD_MANAGER:			pMeta = UIHudManager::static_meta();	break;
			case UIHUD_PLAYTIME_ALARM:	pMeta = UIHUDPlayTimeAlarm::static_meta(); break;
			case UIHUD_TABMINIMAP:		pMeta = UIHUDTabMinimap::static_meta();	break;
			case UIHUD_OBSERVER:		pMeta = UIHUDObserver::static_meta(); break;
			default :					pMeta = nullptr; break;
			}

			if( pMeta)
			{
				m_pHUD[ i] = (UIHUDBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
				I3_MUST_ADDREF(m_pHUD[ i]);
				m_pHUD[ i]->OnCreate( this);
			}
		}

		pMeta = nullptr;

		// Create Popup
		for( i = 0; i < UBP_MAX; i++)
		{
			switch(i)
			{
			case UBP_ACCUSE_CRIME :		pMeta = UIBattlePopupAccuseCrime::static_meta(); break;
#ifdef USE_REPORT
			case UBP_USER_REPORT :		pMeta = UIBattlePopupUserReport::static_meta(); break;
#endif
				//		case UBP_INGAME_SHOP:			pMeta = UIBattlePopupShop::static_meta(); break;
				//
				//		case UBP_BUYITEM:				pMeta = UIBattlePopupBuyItem::static_meta(); break;
				//		case UBP_DELETEITEM:			pMeta = UIBattlePopupDeleteItem::static_meta(); break;
				//
			case UBP_SELECT_WEAPON:			pMeta = UIBattlePopupSelectWeapon::static_meta(); break;
			case UBP_SELECT_CHARA:			pMeta = UIBattlePopupSelectChara::static_meta(); break;
			case UBP_OPTION:				pMeta = UIPopupOption::static_meta(); break;
			case UBP_EXIT :					pMeta = UIBattlePopupExit::static_meta(); break;
				//		case UBP_HUD_USURPATION:		pMeta = UIHudUsurpation::static_meta(); break;
				//
				// Domination용
			//case UBP_SKILL_SHOP:
			//	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DOMINATION))
			//		pMeta = UIBattlePopupSkillShop::static_meta();
			//	break;

			case UBP_HUD_ESCAPE:			pMeta = UIHUDEscape::static_meta(); break;
			case UBP_TWITCHOPTION :			pMeta = UIPopupTwitchOption::static_meta(); break;

			case UBP_HUD_SPACE:				pMeta = UIHudSpace::static_meta(); break;

			default :						pMeta = nullptr; break;
			}

			if( pMeta)
			{
				m_pPopup[ i] = (UIBattlePopupBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
				I3_MUST_ADDREF(m_pPopup[ i]);
				m_pPopup[ i]->OnCreate( this);
			}
		}
	}


	if ( LocaleValue::Enable("FieldShop") )
	{
		UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(m_InfoEquipWeapon);
	}
}

/*virtual*/ void UIBattleFrame::OnUpdate( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	m_shop_ctrl->OnUpdate(rDeltaSeconds);
	i3GameNode::OnUpdate( rDeltaSeconds);
}

void	UIBattleFrame::SetGameShopValidTime(INT32 sec)
{
	m_shop_ctrl->SetGameShopValidTime(sec);
}


void UIBattleFrame::SetRespawn()
{
	m_shop_ctrl->SetRespawn();
}

void	UIBattleFrame::SetDeath()
{
	m_shop_ctrl->SetDeath();
} 

void	UIBattleFrame::SetDeathDelay()
{
	m_shop_ctrl->SetDeathDelay();
}


void	UIBattleFrame::SetRoundEnd()
{
	UIBattleFrame::TogglePopup(UBP_NONE);				  // 배틀프레임의 모든 팝업은 다 닫아버린다..
	m_shop_ctrl->SetRoundEnd();
}


void	UIBattleFrame::SetGameEnd()
{
	m_shop_ctrl->SetGameEnd();
}

void	UIBattleFrame::SetIntermissionStart()
{
	m_shop_ctrl->SetIntermissionStart();
}

void	UIBattleFrame::SetIntermissionEnd()
{
	m_shop_ctrl->SetIntermissionEnd();
}


/*virtual*/ bool UIBattleFrame::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	INT32 i;

	for( i = 0; i < UIHUD_MAX; i++)
	{
		if( m_pHUD[ i] && m_pHUD[ i]->isEnable())
		{
			m_pHUD[ i]->OnEvent( event, pObj, param2, code);
		}
	}

	for( i = 0; i < UBP_MAX; i++)
	{
		if(  m_pPopup[ i] && m_pPopup[ i]->isEnable())
		{
			if( m_pPopup[ i]->OnEvent( event, pObj, param2, code))
				return true;
		}
	}

	if( code == I3_EVT_CODE_ACTIVATE)
	{
		switch( event)
		{
		case I3_EVT_INPUT :
			{
				i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
				i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

				if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
				{
					if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon()
						&& HUD::instance()->IsEnableHud(HUD::MULTIWEAPON) == TRUE)
					{
						// 다중무기 HUD가 켜져있으면 꺼줌
						UIHudMultiWeapon* pHud = 0;
						pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);
						pHud->ShowMultiWeaponSlot();
					}
					else
					{	// 다른 팝업이나 HUD에서 처리한게 없으면 Exit창에 대한 처리를 하자
						CBattleGui::i()->SetEnablePopup();
					}
				}
			}
			break;
		}
	}

	return false;
}

void UIBattleFrame::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
	
	/// 호출 순서를 뒤집었다... ////
	for(INT32 i = 0; i < UIHUD_MAX; i++)
	{
		if(i == UIHUD_INGAMECHATING)
			continue;

		if( m_pHUD[ i] && m_pHUD[ i]->isEnable())
			m_pHUD[ i]->ProcessGameEvent( evt, arg, UserData);
	}
}

void UIBattleFrame::EnableHUD( UIHUD hud, bool bEnable)
{
	if ( m_pHUD[hud] == nullptr || m_pHUD[hud]->isEnable() == static_cast<bool>(bEnable))	// 일치하면 튕겨낸다..
		return;
	
	I3_BOUNDCHK( hud, UIHUD_MAX);
	I3ASSERT( m_pHUD[ hud]);

	if (bEnable)
	{
		bool res = m_pHUD[hud]->OnEntranceStart();
		I3ASSERT(res);
	}
	else
	{
		bool res = m_pHUD[hud]->OnExitStart();
		I3ASSERT(res);
	}
}

void UIBattleFrame::UpdateChatBuffer( )
{
	I3ASSERT( m_pHUD[ UIHUD_INGAMECHATING]);

	if( m_pHUD[UIHUD_INGAMECHATING] != nullptr)
	{
		((UIHUDIngameChat*)m_pHUD[UIHUD_INGAMECHATING])->UpdateChatBuffer();
	}
}

INT32 UIBattleFrame::FindPopup( LuaState * L)
{
	INT32 i;
	i3UIScene * pScene;

	for( i = 0; i < UBP_MAX; i++)
	{
		if(!m_pPopup[i])//없다고 크래쉬 내지말자.
			continue;
		pScene = m_pPopup[ i]->GetScene( 0);

		if( pScene != nullptr)
		{
			if( pScene->getLuaContext() == L)
				return i;
		}	
	}

	return -1;
}

UIBATTLE_POPUP UIBattleFrame::FindPopupByPointer( UIBattlePopupBase * pPopup)
{
	INT32 i;

	for( i = 0; i < UBP_MAX; i++)
	{
		//I3ASSERT( m_pPopup[ i]);
		if( m_pPopup[ i] == pPopup)
			return (UIBATTLE_POPUP)i;
	}

	return UBP_NONE;
}

void UIBattleFrame::TogglePopup( UIBATTLE_POPUP type, void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	UINT32 i;

	if( UBP_NONE == type)
	{	// 모두 닫기
		for( i = 0; i < UBP_MAX; i++)
		{
			if( m_pPopup[i] &&m_pPopup[i]->isEnable())
			{
				m_pPopup[i]->OnExitStart();
				FocusPopup(m_pPopup[i], false);
			}
		}
	}
	else
	{
		I3_BOUNDCHK( type, UBP_MAX);

		if( m_pPopup[type] && m_pPopup[type]->isEnable())
		{	// 해당 창이 열려있으면 닫기
			if( m_pPopup[type]->OnExitStart())
				FocusPopup(m_pPopup[type], false);
		}
		else
		{
			// 나머지는 닫고
			for( i = 0; i < UBP_MAX; i++)
			{
				if( m_pPopup[i] && m_pPopup[i]->isEnable())
				{
					m_pPopup[i]->OnExitStart();
					FocusPopup(m_pPopup[i], false);
				}
			}

			// 해당 창을 연다.
			if( m_pPopup[type] && m_pPopup[ type]->OnEntranceStart( pArg1, pArg2))
				FocusPopup(m_pPopup[type], true);
		}
	}
}

void	UIBattleFrame::OpenPopup(UIBATTLE_POPUP type, void * pArg1, void * pArg2)
{
	if( type != UBP_NONE)
	{
		if ( m_pPopup[type] && m_pPopup[type]->isEnable() == false)
		{
			if( m_pPopup[ type]->OnEntranceStart(pArg1, pArg2))
			{
				FocusPopup( m_pPopup[ type], true);
			}
		}
	}

}
 


void	UIBattleFrame::ClosePopup( UIBATTLE_POPUP type)
{
	if( type != UBP_NONE)
	{
		if ( m_pPopup[type] && m_pPopup[type]->isEnable() == true )
		{
			if( m_pPopup[ type]->OnExitStart())
				FocusPopup(m_pPopup[ type], false);
		}
	}
	
}

void	UIBattleFrame::CloseAll()
{
	// 팝업창이 열려있으면 제거..
	for( INT32 i = 0; i < UBP_MAX; ++i)
	{
		if( m_pPopup[i] && m_pPopup[i]->isEnable())
		{
			m_pPopup[i]->SetEnable(false);
			m_pPopup[i]->OnExitStart();
			FocusPopup(m_pPopup[i], false);
		}
	}
	
	//
	for( INT32 i = 0; i < UIHUD_MAX; ++i)
	{
		if ( m_pHUD[i] && i != UIHUD_MANAGER && m_pHUD[i]->isEnable() ) //UIHUD_MANAGER에 GameResult(게임 결과창 때문에 늦게 끊다.)
			m_pHUD[i]->SetEnable(false);
	}
}


void	UIBattleFrame::AddPopupCount(INT32 cnt)
{
	m_numRenderPopup += cnt;
}

bool	UIBattleFrame::IsOpenPopup( UIBATTLE_POPUP type )
{
	if( type != UBP_NONE )
		return (m_pPopup[type]!=nullptr && m_pPopup[type]->isEnable()) ? true : false;

	return false;
}

void UIBattleFrame::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIBattleFrame::ReceiveGameEvent_Open_IGShop( INT32 arg, const i3::user_data&)
{
	// 열리지 않은걸 확인한 상태이므로 토글만 처리한다..
	UINT64 press  = g_pFramework->getKeyPress();

	if (press & GAME_KEY_CHANGEWEAPON )		// 무기교체 키처리는 StageBattle에 잔존..나중에 다른 곳에서 처리하는게 좋을것이다.
		return;

	if (observer::IsObserverState()) return;

	if ( CBattleGui::i()->GetEnablePopup() == false )		// 구 UI의 팝업이 열려있다면 처리를 하지 않도록 조절..
	{
		this->TogglePopup(UBP_INGAME_SHOP);
	}
}

//투표 시작(신고자용)
void UIBattleFrame::ReceiveGameEvent_KickVote_Suggest_Result( INT32 arg, const i3::user_data&)
{
	i3::rc_wstring wstrMsg;
	switch(arg)
	{
	case	EVENT_ERROR_SUCCESS:
		/*강제 퇴장 투표가 승낙되어 투표 진행합니다.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_OK");
		EnableHUD(UIHUD_FORCE_REMOVE, true); //투표 ui 호출
		break;

	case EVENT_ERROR_KICKVOTE_ALREADY_EXIST_OTHER_VOTE:
		/*이미 다른 투표가 진행 중입니다.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_ALREADY");
		break;

	case EVENT_ERROR_KICKVOTE_SAME_SUGGEST_IN_ONE_MINUTE:
		/*투표 종료후 1분 이내에 동일 신고자 또는 동일 피신고자에게 신고할수 없습니다.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_TIMELIMIT");
		break;

	case EVENT_ERROR_KICKVOTE_SMALL_USER_IN_ROOM:
		/*방안에 투표 인원이 2인 이하라 진행할 수 없습니다.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_NUMPLAYERS");
		break;

	case EVENT_ERROR_KICKVOTE_NOT_ENOUGH_VOTE_TIME:
		/* 방의 게임 잔여시간이 얼마 남지 않아 투표 진행의 의미가 없습니다. */
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_REMAINTIME");
		break;
	case EVENT_ERROR_KICKVOTE_UNDER_RANK_LIMIT:
		/* 최소 랭크에 충족되지 않아 신고할 수 없음 */
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAILRANK");
		break;
	case EVENT_ERROR_KICKVOTE_SAME_SUGGEST_IN_GUARD_TIME:
		/* 피신고자 보호기간 */
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_SAFETIME"); 
		break;
	case EVENT_ERROR_KICKVOTE_MAX_VOTE_COUNT_OVER:
		/* 일일 최대 신고 횟수 초과 */
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_VOTE_MAX"); 
		break;
	default: 
		/*알 수 없는 오류입니다.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_UNKNOWN_ERROR");
		break;
	}

	g_pFramework->PutSystemChatting(wstrMsg);
}

void UIBattleFrame::ReceiveGameEvent_KickVote_Start( INT32 arg, const i3::user_data&)
{
	//투표 시작(비 신고자용)
	if ( GetPopup(UBP_ACCUSE_CRIME)->isEnable() == true )
		ClosePopup(UBP_ACCUSE_CRIME);

	EnableHUD(UIHUD_FORCE_REMOVE, true); //투표 ui 호출
}


// 투표한 사람만 받게 되고, VOTE_COUNT패킷은 오지 않으므로 직접 카운트해야됩니다. (신고자용)
void UIBattleFrame::ReceiveGameEvent_KickVote_Update( INT32 arg, const i3::user_data&)
{
	i3::rc_wstring wstrMsg;

	switch (arg)
	{
	case EVENT_ERROR_SUCCESS:	
		/*투표가 반영되었습니다.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_VOTE_OK"); break;

	case EVENT_ERROR_KICKVOTE_NOTAPPLY_NONE_VOTE:
		// 진행중인 투표가 없습니다. ( 투표가 종료되면, 서버에서 투표종료 패킷을 보내서 클라이언트가 투표 패킷을 보낼수 없게 막기 때문에 발생 가능성 낮습니다. )
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_VOTE_FAIL_NOVOTE"); break;

	case EVENT_ERROR_KICKVOTE_NOTAPPLY_ALREADY_VOTED:
		// 이미 투표를 했습니다. ( 투표를 처음하면, 클라이언트 자체내에서 더 이상 투표 패킷을 보낼수 없게 되기 때문에 발생 가능성 극히 낮습니다. 이 에러가 자주 발생한다면, 버그나 해킹 의심 )
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_VOTE_FAIL_ALREADY"); break;
	case 	EVENT_ERROR_KICKVOTE_NOTAPPLY_NO_RIGHT_TO_VOTE:		
		// 투표권이 없습니다. ( 투표권이 없는 신고자, 피신고자, 난입자는 클라이언트 자체내에서 투표 패킷을 보낼수 없기 때문에 발생 가능성 극리 낮습니다. 이 에러가 자주 발생한다면, 버그나 해킹 의심 )
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_VOTE_FAIL_NORIGHT"); break;

	default:
		/*알 수 없는 오류입니다.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_UNKNOWN_ERROR");
		break;
	}
	g_pFramework->PutSystemChatting(wstrMsg);
}

#include "UIHudManager.h"
namespace CrossCounter_Util
{
	UIBATTLE_POPUP GetStyle()
	{
		if( UIBattleFrame::i()->IsOpenPopup(UBP_OPTION) == true ) return UBP_NONE; //옵션창이 열려있음 작동 안 되게 한다.

		CGameCharaBase* pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->GetMySlotIdxEx());
		if( pMyChara == nullptr)
		{
			UIBattleFrame* pFrame = UIBattleFrame::i();
			if( pFrame != nullptr)
			{
				if( pFrame->IsOpenPopup( UBP_SELECT_WEAPON))		return UBP_SELECT_WEAPON;
				else if( pFrame->IsOpenPopup( UBP_SELECT_CHARA))	return UBP_SELECT_CHARA;
				else												return UBP_SELECT_WEAPON;
			}
			else return UBP_SELECT_WEAPON;
		}

		return (pMyChara->IsDino()) ? UBP_SELECT_CHARA : UBP_SELECT_WEAPON;
	}
};

