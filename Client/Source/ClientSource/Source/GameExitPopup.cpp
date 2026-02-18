#include "pch.h"
#include "GameExitPopup.h"
#include "BattleGui.h"
#include "StageBattle.h"
#include "UI/UIUtil.h"
#include "AwayInput.h"
#include "MessageBoxManager.h"
#include "MyRoomInfoContext.h"
#include "UI/UIBattleFrame.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/snprintf.h"

namespace detail
{
	void CbGiveupBattle(void* pThis,INT32 nParam)
	{
		if(UIBattleFrame::i() != nullptr)
			UIBattleFrame::i()->InputDisable(false);

		((gexit_popup*)pThis)->m_pExitMessageBox = nullptr;

		if( nParam == MBR_OK)
		{
			ISTEST_S
			{	
				CGameFramework::JumpToLogin();
				return;
			}
			ISTEST_E

			I3PRINTLOG(I3LOG_NOTICE,"||||||||||||>>>> Game Exit by User Force!!! (Lene) <<<<||||||||||||");

			CBattleGui::i()->SetEnableGui( false);
			i3GuiRoot::setGuiDisableInput(true);
			INT32 nArg = 0;
			GameEventSender::i()->SetEvent(EVENT_GIVEUP_BATTLE, &nArg );

			g_pFramework->SetNightVisionEnable(false);

			// 홀펀칭 관련하여 패킷이 안 올 수 있기 때문에 자신의 전투포기는 패킷을 안받아도 방으로 돌아간다.
			CAwayInput::i()->setOffAwayInput();

			CGameFramework::JumpToReady();

			//	 레디룸에 나갔을 경우 자유 이동 팝업창을 띄우기위해 값을 셋팅
			//	'자유 이동' 캐시템 사용중인지 체크 (인증 안된 것은 무시)
			//	PhaseReadyRoom:OpenPopupUsingCashItem 에서 팝업 호출에 사용
			if (!MyRoomInfoContext::i()->getStage()->IsAiMode())
			{
				if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_FREE_MOVE) == true )
				{
					CInvenList::i()->SetUseBattleGiveupCashItem( true);
				}
			}
		}
	}


	void CbGiveupBattleUsingBattlefieldMove(void* pThis, INT32 nParam)
	{
		((gexit_popup*)pThis)->m_pExitMessageBox = nullptr;

		//	'전장 이동' 유료 아이템을 사용함
		if( nParam == MBR_USER)		//	'아이템사용'
		{
			//	전장 이동 아이템
			INT64 authFlag = CInvenList::i()->FindWareDBIdx_ByItemID( CASHITEM_ITEMID_BATTLEFIELD_MOVE);

			//	정상적으로 전장 이동 아이템을 사용하여 레디룸에 나갔을 경우 전장 이동 팝업창을 띄우기위해 값을 셋팅
			//	PhaseReadyRoom:OpenPopupUsingCashItem 에서 팝업 호출에 사용
			if (!MyRoomInfoContext::i()->getStage()->IsAiMode())
				CInvenList::i()->SetUseBattleGiveupCashItem( true);

			//	아이템 사용
			GameEventSender::i()->SetEvent(EVENT_GIVEUP_BATTLE, &authFlag);

			// 홀펀칭 관련하여 패킷이 안 올 수 있기 때문에 자신의 전투포기는 패킷을 안받아도 방으로 돌아간다.
			CAwayInput::i()->setOffAwayInput();

			CGameFramework::JumpToReady();
		}
		//	'전장 이동' 유료 아이템을 사용안함. (일반 패널티 적용 퇴장하기 안내 창 출력)
		else if( nParam == MBR_OK)
		{
			gexit_popup::i()->OpenGiveupBattle(GAME_RCSTRING("STBL_IDX_EP_GAME_ENTER_LOBBY_Q"));
		}
	}
	
}


gexit_popup::gexit_popup() : m_pExitMessageBox(nullptr)
{

}

void	gexit_popup::open()
{
#if defined (ENABLE_TEST_COMMAND)
	//	AI 모드일 경우
	g_pFramework->getCommandSet()->setShowSkill(false);
#endif // ENABLE_TEST_COMMAND

	if( g_pFramework->GetMessageBoxManager()->IsTopMsgBox(m_pExitMessageBox) == true ) return; //종료창이 활성화 상태이므로 리턴한다.

	if (!MyRoomInfoContext::i()->getStage()->IsAiMode())
	{
		/*게임을 중단하고 방으로 이동하시겠습니까?*/
		OpenGiveupBattle(GAME_RCSTRING("STR_TBL_INGAME_HUD_MESSAGE_GIVEUP_GAME"));
	}
	else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		/*게임을 중단하고 방으로 이동하시겠습니까?*/
		OpenGiveupBattle(GAME_RCSTRING("STR_TBL_INGAME_HUD_MESSAGE_GIVEUP_TUTORIAL"));
	}
	//	일반 모드일 경우
	else
	{
		i3::rc_wstring strName;
		i3::stack_wstring wstrMessage;
	//	char szMessage[MAX_STRING_COUNT] = {0,};

		//	'자유 이동' 유료 아이템
		INT32 itemFreeMove = CASHITEM_ITEMID_FREE_MOVE;

		//	'전장 이동' 유료 아이템
		INT32 itemBattlefieldMove = CASHITEM_ITEMID_BATTLEFIELD_MOVE;

		//	'자유 이동' 캐시템 사용중인지 체크 (인증 안된 것은 무시)
		//	최우선 체크
		if( CInvenList::i()->IsUsingCashItem( itemFreeMove))
		{
			g_pShopItemInfoDataBase->GetItemName(itemFreeMove, strName);

			/*사용 중인 {col:255,153,51,255}%s{/col} 아이템으로 인하여\n퇴장시 페널티가 부여되지 않습니다.\n \n방으로 이동하시겠습니까?*/
			i3::sprintf( wstrMessage, GAME_RCSTRING("STR_TBL_INGAME_HUD_USE_ITEM_TEXT"), strName);

			m_pExitMessageBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(&detail::CbGiveupBattle), this);
		}
		//	'전장 이동' 캐시템을 보유하고 있는지 체크
		else if( CInvenList::i()->IsUsingCashItem( itemBattlefieldMove))
		{	
			g_pShopItemInfoDataBase->GetItemName(itemBattlefieldMove, strName);

			/*{col:255,153,51,255}%s{/col} 아이템을 사용하여 퇴장하면\n페널티가 부여되지 않습니다.\n \n아이템을 사용하시겠습니까?*/
			i3::sprintf( wstrMessage, GAME_RCSTRING("STR_TBL_INGAME_HUD_USE_ITEM_TEXT2"), strName);

			// 사용안함, 취소, 아이템사용은 LuaScript에서 설정합니다. User 타입으로 설정. 다른 caption을 주려면 다른 타입으로 설정.. 또는 다르게 구현해야함...
			m_pExitMessageBox = GameUI::MsgBox( MSG_TYPE_GAME_USER, wstrMessage, nullptr, MAKE_CALLBACK(&detail::CbGiveupBattleUsingBattlefieldMove), this);
		}
		else
		{
			OpenGiveupBattle( GAME_RCSTRING("STBL_IDX_EP_GAME_ENTER_LOBBY_Q"));
		}
	}

	//alt + F4로 활성화 할 경우, 종료창이 활성화된 상태에서 키입력이 바로 되기 때문에 임의로 비활성화 한다.(중요한 코드)
	//만약, 아래 코드가 없는 경우엔 종료창이 활성화 즉시 비활성화 된다.
	i3InputKeyboard * pKeyboard = g_pFramework->getKeyboard();
	if( pKeyboard->GetStrokeState(I3I_KEY_F4) == true) 
		pKeyboard->SetStrokeState(I3I_KEY_F4, false); 

	i3UI::getGlobalIme()->KillFocus(); //채팅창에 포커스가 가능 경우가 있어서 임의로 포커스를 죽인다.

	if(UIBattleFrame::i() != nullptr)
		UIBattleFrame::i()->InputDisable(true);
}	



// 스테이지배틀 종료시 close를 명시해주는게 좋겠다..
void	gexit_popup::ProcessDestroyStage()
{	
	if( m_pExitMessageBox != nullptr)
	{
		GameUI::CloseMsgBox( m_pExitMessageBox);
		m_pExitMessageBox = nullptr;
	}
}

void	gexit_popup::OpenGiveupBattle(const i3::rc_wstring& wstrMessage)		//	게임 퇴장 팝업 // private이 적합..
{
	m_pExitMessageBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(&detail::CbGiveupBattle), this);
}
