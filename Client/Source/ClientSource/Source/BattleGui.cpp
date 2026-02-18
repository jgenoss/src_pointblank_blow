#include "pch.h"
#include "BattleGui.h"

#include "GlobalVariables.h"
#include "StageBattle.h"
#include "AwayChecker.h"
#include "GameGUI.h"
#include "Shop.h"
#include "LogRespawn.h"
#include "GameExitPopup.h"
#include "GameStateMgr.h"

#include "BattleHUD_Revenge.h"

#include "UI/UIUtil.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIBattlePopupBase.h"
#include "UI/UIHUDIngameChat.h"
#include "ui/UIItemInfoUtil.h"
#include "ui/UIHudManager.h"

#include "i3Base/string/ext/integral_to_comma_str.h"

#include "./StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

#include "ClientStageInfoUtil.h"

#if legacy_gui_function
	#include "GuiNotifyReceiver.h"
#endif

I3_CLASS_INSTANCE(CBattleGui); //, i3GameObjBase);

CBattleGui::CBattleGui(void)
{	
	m_bEnableSelectWep = false;
	m_bEnableRespawn = false;
	m_bEnableResult = false;
	m_bAutoRespawn = false;

	m_pBattleGUIRoot = nullptr;

	m_fRespawnTime = 0.0f;
	m_fTimeBarScale = 0.0f;

	//m_pPanel_BOSS_PreStart = nullptr;

#if defined	USE_AWAY_INPUT
	m_pAwayChecker = nullptr;
	m_pAwayMessageBox = nullptr;
#endif
	
#if legacy_gui_scene
	m_pPanel_Stage_PreStart = nullptr;
#endif

#if legacy_not_use_operator
	m_rWheelDelta = 0.0f;
#endif
}

CBattleGui::~CBattleGui(void)
{
#if defined	USE_AWAY_INPUT
	if (m_pAwayMessageBox)
	{
		GameUI::CloseMsgBox( m_pAwayMessageBox);
		m_pAwayMessageBox = nullptr;
	}

	if (m_pAwayChecker)
	{
		RemoveChild(m_pAwayChecker);
		I3_SAFE_RELEASE(m_pAwayChecker);
	}
#endif

	if( m_pBattleGUIRoot != nullptr)
	{
		m_pBattleGUIRoot->getSceneGraphNode()->RemoveFromParent();
		I3_SAFE_RELEASE( m_pBattleGUIRoot);
	}

	m_pBattleGUIRoot = nullptr;
	g_pGUIRoot = nullptr;

	//I3_SAFE_RELEASE( m_pPanel_BOSS_PreStart);
#if legacy_gui_scene
	I3_SAFE_RELEASE( m_pPanel_Stage_PreStart);
#endif
}

bool CBattleGui::Create()
{
	LoadGUIFile( "Gui/Project/Respawn_Result.gui");	
	((i3GuiWindow*)(m_pBattleGUIRoot->getFirstChild()))->setState(I3GUI_WINDOW_STATE_ALLWAYS_FOCUS);
	_LinkControl();	
	InitGui();
	
	SetEnableGui( false);

#if defined	USE_AWAY_INPUT
	// AwayChecker 설정
	m_pAwayChecker = CAwayChecker::new_object();
	m_pAwayChecker->SetReceiver(this);
	m_pAwayChecker->Reset();
	I3_GAMENODE_ADDCHILD(this, m_pAwayChecker);
#endif

//	if( MyRoomInfoContext::i()->IsDinoEscapeMode())
//	{
//		m_pPanel_BOSS_PreStart = CBattleGUI_BOSS_PreStart::new_object();
//		m_pPanel_BOSS_PreStart->Create( this);
//		m_pPanel_BOSS_PreStart->SetEnable( false);
//
//		I3_GAMENODE_ADDCHILD( this, m_pPanel_BOSS_PreStart);
//	}
//#if legacy_gui_scene
//	if( g_pGameContext->IsHeadHunterRoom() )
//	{
//		m_pPanel_Stage_PreStart = CBattleGUI_Stage_PreStart::new_object();
//		m_pPanel_Stage_PreStart->Create( this);
//		m_pPanel_Stage_PreStart->SetEnable( false);
//
//		I3_GAMENODE_ADDCHILD( this, m_pPanel_Stage_PreStart);
//	}
//#endif
	return true;
}

bool CBattleGui::LoadGUIFile(const char * szGUIFileName)
{
	i3ResourceFile File;

	if( File.Load( szGUIFileName) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "CBattleGui::LoadGUIFile()");
		return false;
	}

	// GuiRoot를 찾아 초기화한다
	m_pBattleGUIRoot = (i3GuiRoot*)File.FindObjectByMeta(i3GuiRoot::static_meta());
	if( m_pBattleGUIRoot == nullptr) return false;

	I3ASSERT(m_pBattleGUIRoot != nullptr);

	const i3TextFontSetInfo * pFontTextInfo = GetDefaultFontName();
	m_pBattleGUIRoot->setTextNodeUsage(I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX);	
	m_pBattleGUIRoot->Create( g_pFramework->getGuiLayer(), pFontTextInfo->GetFontName(), pFontTextInfo->GetCharset());
	m_pBattleGUIRoot->SetViewer(g_pViewer, false);
	m_pBattleGUIRoot->setWidth( g_pViewer->GetViewWidth());
	m_pBattleGUIRoot->setHeight( g_pViewer->GetViewHeight());
	m_pBattleGUIRoot->setManualNotifyDispatch( true);

	_Rec_BuildScene((i3GuiObjBase*)(m_pBattleGUIRoot->getFirstChild()));

	m_pBattleGUIRoot->SetEnable(false);		// FadeIn 시작전까지 비활성화

	m_pBattleGUIRoot->OnUpdate( 0.0f);

	I3_MUST_ADDREF(m_pBattleGUIRoot);

	g_pGUIRoot = m_pBattleGUIRoot;

	return true;
}


void CBattleGui::_Rec_BuildScene( i3GuiObjBase * pObj)
{
	if( pObj == nullptr)	return;

	i3GameNode	* pParent		= pObj->getParent();
	i3Node		* pParentNode	= nullptr;

	pParentNode = ((i3GuiObjBase *)pParent)->GetNode();

	// GUI중 EditBox Control및 EditBox의 상속을 받는 경우 Text를 생성해야 합니다.
	if( i3::same_of<i3GuiEditBox*>(pObj)
		|| i3::same_of<i3GuiTextBox*>(pObj))
	{
		if (i3::generic_is_iequal(((i3NamedElement*)pObj)->GetName(), "Respawn_Notice"))
		{
			// 전투중 GUI는 해상도에 따라 확대되지 않는다.
			// 초기화 시 ReCreateTextEx()를 사용하면 확대됨으로, 이곳에서 미리 설정한다.
			((i3GuiEditBox*)pObj)->CreateTextEx( GetDefaultFontName(), FONT_COUNT_DEFAULT, GAME_FONT_SIZE_HUD_RESPAWN_TEXT, 0, 0, FW_NORMAL, true, FONT_SHADOW_QUALITY_HIGH);

			((i3GuiEditBox*)pObj)->SetTextSpace(g_pConfigEx->GetGUI().DefaultTextSpaceX, g_pConfigEx->GetGUI().DefaultTextSpaceY);
			((i3GuiEditBox*)pObj)->SetOffsetPos(g_pConfigEx->GetGUI().DefaultTextOffsetX, g_pConfigEx->GetGUI().DefaultTextOffsetY);
		}
		else
		{
			((i3GuiEditBox*)pObj)->CreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE);

			((i3GuiEditBox*)pObj)->SetTextSpace(g_pConfigEx->GetGUI().DefaultTextSpaceX, g_pConfigEx->GetGUI().DefaultTextSpaceY);
			((i3GuiEditBox*)pObj)->SetOffsetPos(g_pConfigEx->GetGUI().DefaultTextOffsetX, g_pConfigEx->GetGUI().DefaultTextOffsetY);
		}			
	}

	//Add Child
	if( pObj->GetNode() != nullptr)
	{
		pParentNode->AddChild( pObj->GetNode());
	}

	//Child
	i3GuiObjBase * pChild = ( i3GuiObjBase *)pObj->getFirstChild();
	if( pChild != nullptr)
	{
		_Rec_BuildScene( pChild);
	}

	//Next
	_Rec_BuildScene( (i3GuiObjBase *)pObj->getNext());
}

void CBattleGui::InitGui()
{
	return;
/*
	m_bEnableSelectWep = false;
	m_bEnableRespawn = false;
	m_bEnableResult = true;
	m_bAutoRespawn = false;

	// 해상도 변경이 없다면, Viewer의 중심에 맞게 GUI를 이동합니다.
	{
		i3GuiRoot * pGUIRoot = getGUIRoot();

		if( !pGUIRoot->isChangeResolution())
		{
			// 윈도우를 중앙으로 이동
			i3GuiWindow* pBgWin= (i3GuiWindow*)pGUIRoot->getFirstChild();
			i3::pack::POINT2D ptBgWin= g_pFramework->GetCenterPosition((INT32)(pBgWin->getWidth() / (g_pViewer->GetViewWidth() / I3GUI_RESOLUTION_WIDTH)) , (INT32)(pBgWin->getHeight() / (g_pViewer->GetViewHeight() / I3GUI_RESOLUTION_HEIGHT)));
			
			pBgWin->setPositionX((REAL32)(ptBgWin.x));
			pBgWin->setPositionY((REAL32)(ptBgWin.y));
		}
	}

	// 보스 미션용 GUI
	if( m_pPanel_BOSS_PreStart != nullptr)
		m_pPanel_BOSS_PreStart->Init();

	// Stage GUI
#if legacy_gui_scene
	if( m_pPanel_Stage_PreStart != nullptr)
		m_pPanel_Stage_PreStart->Init();
#endif
*/
}

void CBattleGui::SetEnablePopup(void)
{
	////////////////////////////////////////////////////////////////////////
	// 게임중 중지 Popup을 활성화/비활성화 합니다.
	////////////////////////////////////////////////////////////////////////
	
	if( m_pBattleGUIRoot == nullptr)		// 어차피 크래시가 날 가능성이 있다면 이른 체크를 통해 바로 리턴..(11.11.07.수빈)
		return;

	bool bEnable = !GetEnablePopup();		//  이 함수로 대체한다...(11.11.22.수빈) //(m_pStaticExitPopupBG->isEnable());

	if( HUD::instance() != nullptr)
	{
		if( HUD::instance()->IsEnableHud(HUD::GAME_RESULT) == true ) //게임 종료창 활성화 상태에서는 UBP_EXIT 팝업을 안 띄운다.
			bEnable = false;

		if( HUD::instance()->IsEnableHud(HUD::DEFAUTOCHANGE) == true ) //게임 종료창 활성화 상태에서는 UBP_EXIT 팝업을 안 띄운다.
			bEnable = false;
	}

	if( bEnable)
	{
		g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);

		UIBattleFrame::i()->OpenPopup( UBP_EXIT);
	}
	else
	{
		UIHUDIngameChat * pChat = (UIHUDIngameChat *)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING);
		if(pChat && false == pChat->GetEnableChat())
		{	
			g_pFramework->SetGameKeyMode( GAMEKEY_MODE_GAME);
		}

		UIBattleFrame::i()->ClosePopup( UBP_EXIT);
	}

	_UpdateRootEnableState();
}

void CBattleGui::_UpdateRootEnableState(void)
{
	bool bEnable = UIBattleFrame::i()->IsOpenPopup( UBP_EXIT)?true:false;

//	if( m_pPanel_BOSS_PreStart != nullptr)
//	{
//		bEnable = bEnable || (m_pPanel_BOSS_PreStart->isVisible() == true);
//	}
//#if legacy_gui_scene
//	if( m_pPanel_Stage_PreStart != nullptr)
//	{
//		bEnable = bEnable || (m_pPanel_Stage_PreStart->isVisible() == true);
//	}
//#endif

	m_pBattleGUIRoot->SetEnable( bEnable);
}

bool CBattleGui::GetEnablePopup(void)
{
	UIBattleFrame* frame = UIBattleFrame::i();		// UI팝업이 존재하는 모든 경우를 처리해야함.(11.11.22.수빈)
	if (frame != nullptr)
	{
		if( frame->IsPopupFocused() ) return true;
		return frame->IsOpenPopup( UBP_EXIT);
	}

	return false;
}
 
void CBattleGui::SetEnableGui( bool bFlag, GAME_GUI_STATUS nState, bool bDeath)
{
	if( m_pBattleGUIRoot == nullptr)		// 어차피 크래시가 날 가능성이 있다면 이른 체크를 통해 바로 리턴..(11.11.07.수빈)
		return;

	//로딩이 안되어 있을수가 충분히 있다..
	//if( m_pPanel_BOSS_PreStart != nullptr)
	//	m_pPanel_BOSS_PreStart->SetEnableGUI( bFlag == true, nState, bDeath);

//#if legacy_gui_scene
//	if( m_pPanel_Stage_PreStart != nullptr)
//		m_pPanel_Stage_PreStart->SetEnableGUI( bFlag == true, nState, bDeath);
//#endif

	if( bFlag)
	{					
		m_pBattleGUIRoot->ClearMouseState();

		// Respawn
		switch( nState)
		{
			case GAME_GUI_RESPAWN :
				{
#if defined( DEF_OBSERVER_MODE)
					if( BattleSlotContext::i()->isObserverMe())
#else
					if(UserInfoContext::i()->IsGM_Observer())
#endif
						break;

					m_fRespawnTime = 0.0f;

					m_bEnableRespawn = true;
					m_pBattleGUIRoot->setCapturedMouseControl(nullptr);

					m_bAutoRespawn = false;
					m_bEnableResult = false;

#if defined	USE_AWAY_INPUT
					Check_AwayCheck();
#endif
				}
				break;

			// Result
			case GAME_GUI_RESULT :
				if( UIBattleFrame::i()->IsOpenPopup( UBP_EXIT) )
				{
					SetEnablePopup();
				}

				g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);
				m_bEnableResult = true;

				m_bEnableSelectWep = false;
				m_bEnableRespawn = false;
				break;

			case GAME_GUI_PRESTART_ROUND :
				m_bEnableResult = false;
				m_bEnableSelectWep = false;
				m_bEnableRespawn = false;
				break;
		}

		CHUD_Revenge::instance()->SetRevenge( false);
	}
	else
	{
		UIBattleFrame * pBattleFrame	= UIBattleFrame::i();

		if( pBattleFrame )
		{
			bool bForcebly_Remove_Popup = false;

			if( pBattleFrame->IsOpenPopup(UBP_ACCUSE_CRIME) )
				bForcebly_Remove_Popup = true;

			if (false == gexit_popup::i()->is_opened() && this->GetEnablePopup() == false && bForcebly_Remove_Popup == false)
			{	
				UIHUDIngameChat * pUIIngameChat = ((UIHUDIngameChat *)pBattleFrame->GetHUD(UIHUD_INGAMECHATING));
				if( pUIIngameChat && false == pUIIngameChat->GetEnableChat())
				{
					if (nullptr == m_pParent || false == pUIIngameChat->GetEnableChatScroll())
					{
						g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);
					}
				}
			}
		}
		
		m_bAutoRespawn = false;
		m_bEnableSelectWep = false;
		m_bEnableRespawn = false;

#if defined	USE_AWAY_INPUT
		// 리스폰 제한 검사 끝
		if (m_pAwayChecker) m_pAwayChecker->Reset();
#endif

		m_bEnableResult = false;
	}

	_UpdateRootEnableState();
	
	// 관전 모드 시 연계 하지 않음
	if( g_pCamera != nullptr 
		&& !i3::same_of<CGameCamera_Observer*>(g_pCamera)
		&& !i3::same_of<CGameCamera_Observer2*>(g_pCamera)
		&& !i3::same_of<CGameCamera_FLY*>(g_pCamera))
	{
		gstatemgr::i()->EnableHUD(!bFlag, (GAME_GUI_STATUS) nState, bDeath);
	}
}

bool CBattleGui::GetEnableGui( INT32 nState)
{
	bool bRet = false;

	if( nState == GAME_GUI_RESPAWN)
	{
		if( m_bEnableRespawn) bRet = true;
	}
	else if( nState == GAME_GUI_RESULT)
	{
		if( m_bEnableResult) bRet = true;
	}

	return bRet;
}

void CBattleGui::OnUpdate( REAL32 rDeltaSeconds )
{
	ProcessSelectHud(rDeltaSeconds);

	// 리스폰 정보를 업데이트 합니다.
	if( m_bEnableRespawn)
	{
		ProcessRespawn( rDeltaSeconds);
	}

#if legacy_not_use_operator
	_ProcessWheel();
#endif

	i3GameObjBase::OnUpdate( rDeltaSeconds );
}

bool CBattleGui::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
	#if defined	USE_AWAY_INPUT
		switch(event)
		{
		case ACE_AWAY:
			{
				SetEnableGui( false);
				i3GuiRoot::setGuiDisableInput(true);
				INT32 nArg = 0;
				GameEventSender::i()->SetEvent(EVENT_GIVEUP_BATTLE, &nArg);

				// 홀펀칭 관련하여 패킷이 안 올 수 있기 때문에 자신의 전투포기는 패킷을 안받아도 방으로 돌아간다.
				CGameFramework::JumpToReady();
			}
			return true;
		case ACE_CAUTION:
			m_pAwayMessageBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_BATTLEGUI_NOTICE_LEAVEROOM"));
			return true;
		}
	#endif
	}
	return false;
}

void CBattleGui::OnGameStart(void)
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
	{
		I3TRACE( "inObserverMe : call OnGameStart.....\n");
		return;
	}
#endif

	// Local Mode
	ISTEST_S
	{
		g_pCharaManager->RespawnChara( BattleSlotContext::i()->getMySlotIdx(), true, true );

		SetEnableGui(false, GAME_GUI_RESPAWN);
		return;
	}
	ISTEST_E

	if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ||
		 MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION)
	   )
	{
#if defined	USE_AWAY_INPUT
		// 리스폰 제한 검사 끝
		m_pAwayChecker->Reset();
#endif		
	}
	else if( UserContext::i()->Death[BattleSlotContext::i()->getMySlotIdx()])
	{	// 캐릭터가 죽은 경우
		m_bEnableSelectWep = false;

		g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);

		return;
	}	

	SetEnableGui(false, GAME_GUI_RESPAWN);
}


void CBattleGui::OnSelectWeaponClose(void)
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
	{
		I3TRACE( "isObserverMe : call OnSelectWeaponClose.....\n");
		return;
	}
#endif

	m_bEnableSelectWep = false;

	UIBattleFrame* pFrame = UIBattleFrame::i();
	if( pFrame )
		pFrame->ClosePopup( cc::GetStyle());

	g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);
}

void CBattleGui::Check_AwayCheck()
{
	//선택창 활성화된 상태에서 리스폰 시작시
	UIBattleFrame* pFrame =	UIBattleFrame::i();
	if( pFrame->IsOpenPopup( cc::GetStyle()) )
		m_pAwayChecker->SetTime(AWAY_INPUT_RESPAWN_TIME, AWAY_INPUT_RESPAWN_CAUTION);
}

void	 CBattleGui::ProcessSelectHud(REAL32 rDeltaSeconds)
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
	{
		I3TRACE( "isObserverMe : call ProcessRespawn.....\n");
		return;
	}
#endif

	if (UserInfoContext::i()->IsGM_Observer())
		return;

	if( ((CStageBattle*)m_pParent)->GetChatEnable() == TRUE ) return;	//채팅창 입력 메시지다.

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) && 
		MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON)
	  )
		return;

	UIBattleFrame* pFrame =	UIBattleFrame::i();

	if (pFrame->IsOpenPopup( UBP_INGAME_SHOP )) return;	//야전 상점 활성화 상태면 나가라.

	if ( (	g_pFramework->getKeyStroke() & GAME_KEY_CHANGEWEAPON || g_pFramework->getSysKeyStroke() & GAME_KEY_CHANGEWEAPON )
			&& HUD::instance()->GetHud(HUD::MULTIWEAPON)->IsVisible() == false)
	{
		//종료창을 닫는다.
		if( pFrame->IsOpenPopup( UBP_EXIT) ) pFrame->ClosePopup(UBP_EXIT);

		UIBATTLE_POPUP popup = cc::GetStyle();
		if( popup != UBP_NONE )
		{
			if(pFrame->IsOpenPopup(popup) == false )
			{
				g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);
				m_bEnableSelectWep = true;

				pFrame->OpenPopup( popup );

#if defined	USE_AWAY_INPUT
				if( m_bEnableRespawn)		//리스폰 중에 선택창 활성화
				{
					m_pAwayChecker->Reset();
				}
#endif
			}
			else
			{
				OnSelectWeaponClose();

#if defined	USE_AWAY_INPUT
				// 리스폰 제한 검사 끝
				m_pAwayChecker->Reset();
#endif
			}
		}
	}
}

void CBattleGui::ProcessRespawn( REAL32 rDeltaSeconds )
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
	{
		I3TRACE( "isObserverMe : call ProcessRespawn.....\n");
		return;
	}
#endif

	INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();

	REAL32 rRespawnLimit = MyRoomInfoContext::i()->GetRespawnTime( mySlotIdx );
	m_fRespawnTime += rDeltaSeconds;	

	log_respawn::i()->AddRespawnTime(rDeltaSeconds);

	if( m_fRespawnTime > rRespawnLimit) m_fRespawnTime = rRespawnLimit;

	// revision 50214 GAMESTAGE_STATE_BATTLE 가 아닌 상태에서 리스폰 되는 것을 방지. (hansoft.1293)
	if( gstatemgr::i()->isPlayingBattle() )
	{
		// 유저가 무기를 고르지 않고 시간이 경과했다면 바로 게임을 시작합니다.
		if( m_fRespawnTime >= rRespawnLimit && m_bEnableSelectWep == false && m_bAutoRespawn == false )
		{
			m_bEnableRespawn = false;
			m_bAutoRespawn = true;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//#if defined( FINDCTRL_CRC)
//컨트롤들을 이름으로 비교하여 일일이 링크하는부분이 부하를 주고있어서.
//컨트롤을 이진 crc검색으로 하기위해 binlist에 넣어 정렬해둡니다.
struct FCRCIF
{
	i3::vector_set<STRCINFO*>* plist;
	STRCINFO	* pFirst;
	INT32		nCurCnt;
};

static bool _SetSoundInfoProc( i3GameNode * pNode)
{
	if( !i3::kind_of<i3GuiObjBase*>(pNode))
		return false;

	i3GuiObjBase * pObj = (i3GuiObjBase *)pNode;

	for( INT32 i = 0; i < I3GUI_CONTROL_STATE_RESERVED3; ++i)
	{
		I3GUI_EVENT_DATA * pEvent = pObj->getEventData( (GUI_CONTROL_STATE)i);
		if( pEvent != nullptr && (pEvent->m_nEventStyle & I3GUI_EVENT_SOUND))
		{
			i3SoundNode * pSNode = pEvent->m_pSoundNode;

			i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->QuaryResource( pSNode->GetName());
			if( pRes == nullptr)
				continue;

			i3SoundPlayInfo * pInfo = pSNode->getSoundPlayInfo();
			i3SoundPlayInfo * pResInfo = pRes->getSound();

			pInfo->SetDefaultVolume( pResInfo->GetDefaultVolume());
			pInfo->setListenerVolume( pResInfo->getListenerVolume());
		}
	}

	return true;
}

static bool _SetRCCountProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( pNode == nullptr)
		return false;

	if( pNode->hasName())
	{
		INT32 * pCnt = (INT32*)pUserData;

		*pCnt = *pCnt+1;
	}

	//추가분..사운드 설정하는 부분 끼워넣었습니다.현재 프로시저네임과는 별도라고 생각해주세요 -raja-
	_SetSoundInfoProc( pNode);

	return true;
}


static bool _SetRCInfoAddProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( pNode == nullptr)
		return false;

	if( pNode->hasName())
	{
		FCRCIF * pInfo = (FCRCIF*)pUserData;

		STRCINFO * pNewInfo = pInfo->pFirst + pInfo->nCurCnt;

		pNewInfo->crc = CRC32( 0xFFFFFFFF, (UINT8 *) pNode->GetName(), i3::generic_string_size( pNode->GetName()));
		pNewInfo->pCtrl = pNode;

		pInfo->plist->insert( pNewInfo);

		pInfo->nCurCnt++;
	}

	return true;
}

void CBattleGui::_LinkControl(void)
{
	// 첫번째 컨트롤을 얻어온다
	i3GuiWindow* pWindow = (i3GuiWindow*)m_pBattleGUIRoot->getFirstChild();
	
	// 자식 컨트롤이 없다면 연결하지 않는다
	if (nullptr == pWindow)
	{
		return;
	}

	i3::vector_set<STRCINFO*> CtrlList;
	FCRCIF inf;

	{
		i3GameNodeTraversal spTraverse;

		INT32 nCnt = 0;
		
		spTraverse.setUserProc( _SetRCCountProc);
		spTraverse.setUserData((void*)&nCnt);

		spTraverse.Traverse( pWindow);

#if defined(USE_LOCALE_UI)
		if(nCnt <= 0)
		{
			return;
		}
#endif

		I3ASSERT( nCnt > 0);
		
		inf.nCurCnt = 0;
		inf.pFirst	= (STRCINFO*)i3MemAlloc( sizeof( STRCINFO) * nCnt);
		inf.plist	= &CtrlList;

		spTraverse.setUserProc( _SetRCInfoAddProc);
		spTraverse.setUserData((void*)&inf);

		spTraverse.Traverse( pWindow);
	}

	I3MEM_SAFE_FREE_POINTER( inf.pFirst);

	CtrlList.clear();
}

void CBattleGui::UpdateScore( bool bVisible, REAL32 tm)
{
	_UpdateRootEnableState();
}


#if legacy_not_use_operator
void CBattleGui::OnResultConfrim(void)
{
	gstatemgr::i()->setEndTime(100.f);
}

void CBattleGui::OnTrainingConfrim(void)
{
	gstatemgr::i()->setEndTime(100.f);
}

void CBattleGui::OnTrainingUpdateScore(void)
{
	gstatemgr::i()->setEndTime(100.f);
}

void CBattleGui::_ProcessWheel(void)
{
	// 마우스 휠버튼 검사
	VEC3D mouseDelta;
	i3Vector::Copy(&mouseDelta, g_pFramework->getMouseMoveDelta());

	m_rWheelDelta += mouseDelta.z;

	INT32 scrollby = (INT32)(m_rWheelDelta / WHEEL_DELTA);

	// 업데이트 되지 않았으면 처리하지 않는다
	if (0 == scrollby)
	{
		return;
	}

	// 휠 델타 초기화 - WHEEL_DELTA보다 작게 움직였다면, 커질때까지 누적해야한다
	if (WHEEL_DELTA <= m_rWheelDelta || -WHEEL_DELTA >= m_rWheelDelta)
	{
		m_rWheelDelta = 0.0f;
	}
}
#endif