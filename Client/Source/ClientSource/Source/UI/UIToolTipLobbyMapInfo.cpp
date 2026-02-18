#include "pch.h"
#include "UIToolTipLobbyMapInfo.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"

#include "../TextSet.h"
#include "UIUtil.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "ClientStageInfoUtil.h"

static UIPhaseLobby * GetLobby(void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	if(pFrame->GetCurrentPhaseType() == UIPHASE_LOBBY)
	{
		UIPhaseLobby * pPhase = static_cast<UIPhaseLobby *>(pFrame->GetCurrentPhase());
		return pPhase;
	}
	
	return nullptr;
}

extern "C" {
}

LuaFuncReg UIToolTipLobbyMapInfo_Glue[] = {
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIToolTipLobbyMapInfo);//, UIToolTipBase);


UIToolTipLobbyMapInfo::UIToolTipLobbyMapInfo()
{

}

UIToolTipLobbyMapInfo::~UIToolTipLobbyMapInfo()
{

}

void UIToolTipLobbyMapInfo::_GetBalanceText(i3::rc_wstring& out, UINT8 ui8Balance) const
{
	switch(ui8Balance)
	{
	case 0:		
		out = GAME_RCSTRING("STR_TBL_GUI_READY_TEAM_BALANCE_OFF");/*없음*/
		break;
	case 1:
		out = GAME_RCSTRING("STR_TBL_GUI_READY_TEAM_BALANCE_PERSON");/*인원*/
		break;
	default:
		out = GAME_RCSTRING("STR_TBL_GUI_READY_TEAM_BALANCE_SKILLED"); /*실력*/
		break;
	}
}

void UIToolTipLobbyMapInfo::_SetMapInfo( const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo)
{
	if( nullptr == pInfo)
	{	// 해당 방은 없어진경우..
		return;
	}


	INT32 stageImageIdx;							// Map Image

	// 맵 이름 & 맵 이미지 인덱스
	i3::rc_wstring wstrStageName;

	bool Random;
	Random = (pInfo->_InfoFlag & ROOM_INFO_FLAG_RANDOM_MODE) ? true : false;
	if( Random)
	{
		wstrStageName = GAME_RCSTRING("STR_POPUP_RANDOMMAP");		
		stageImageIdx = 26;
	}
	else
	{
		CSI_MAP* mapData = CStageInfoUtil::GetMapData(pInfo->_StageID);
		wstrStageName = mapData != nullptr ? mapData->getDisplayName() : L"NotHaveData";
		stageImageIdx = g_pFramework->GetMapShapeIdx(StageID::GetStageUID(pInfo->_StageID));
	}


	// 게임모드
	i3::rc_wstring wstrModeName;

	{
		wstrModeName = CStageInfoUtil::GetRuleData(pInfo->_StageID)->getRuleName();
	}

	// 팀밸런스
	//char szBalance[ MAX_STRING_COUNT] = {0};
	i3::rc_wstring	wstrBalance;

	_GetBalanceText(wstrBalance, pAddInfo->_LockTeamBalance);

	//New 마크 표시
	INT32 newMark = 0;
	if ( CStageInfoUtil::GetStageData(pInfo->_StageID)->m_ui8Mark == GOODS_MARK_NEW )
		newMark = 1;

	// 승리 조건
	i3::wstring wstrWinObject;
	UINT8 GameSetPrim					= pAddInfo->_SubType & 0x0F;
	UINT8 GameSetSec					= pAddInfo->_SubType & 0xF0;
	TextSet::GetWinObjectText(wstrWinObject, CStageInfoUtil::GetStageData(pInfo->_StageID), GameSetPrim, GameSetSec);

	//  X-Mas Mark
	bool bXMasEnable = false;
	if( TESTBIT( CStageInfoUtil::GetStageData(pInfo->_StageID)->m_ui8Mark, GOODS_MARK_XMAS) )
		bXMasEnable = true;

	LuaState * L = _CallLuaFunction( "SetInfo");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wstrStageName);					// Map Name
		i3Lua::PushStringUTF16To8( L, wstrModeName);					// Mode Name
		i3Lua::PushInteger( L, stageImageIdx);					// Map Image Idx
		i3Lua::PushInteger( L, newMark);						// New Mark
		i3Lua::PushBoolean( L , bXMasEnable ? true : false );	// X-Mas Mark
		i3Lua::PushStringUTF16To8( L, wstrWinObject);				// 승리 조건
		i3Lua::PushStringUTF16To8( L, wstrBalance);					// 팀 밸런스 설정

		_EndLuaFunction( L, 7);
	}	
}

/*virtual*/ void UIToolTipLobbyMapInfo::OnCreate( i3GameNode * pParent)
{
	UIToolTipBase::OnCreate( pParent);

	AddScene( "Scene/Main/PointBlankRe_Lobby_MapInfo_ToolTip.i3UIs", UIToolTipLobbyMapInfo_Glue);
}

/*virtual*/ bool UIToolTipLobbyMapInfo::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIToolTipBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_SetMapInfo( static_cast<ROOM_INFO_BASIC*>(pArg1), static_cast<ROOM_INFO_ADD*>(pArg2));
	m_slide.ForceEnd();

	return true;
}

/*virtual*/ bool UIToolTipLobbyMapInfo::OnExitStart( void)
{
	if( UIToolTipBase::OnExitStart() == false)
		return false;

	m_slide.ForceEnd();
	return true;
}

void UIToolTipLobbyMapInfo::EnableToolTip( const ROOM_INFO_BASIC * pArg1 /* = nullptr */, const ROOM_INFO_ADD * pArg2 /* = nullptr */)
{
	// Popup이 켜져 있을 경우 키지 않는다.
	UIMainFrame* pMainframe = g_pFramework->GetUIMainframe();
	if(pMainframe->GetCurrentPopup() != nullptr) return;

	OnEntranceStart( const_cast<ROOM_INFO_BASIC*>(pArg1), const_cast<ROOM_INFO_ADD*>(pArg2) );
	I3TRACE("UIToolTipLobbyMapInfo::EnableToolTip()\n");
}

void UIToolTipLobbyMapInfo::DisableToolTip(void)
{
	OnExitStart();
}

void UIToolTipLobbyMapInfo::ChangeToolTip( const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo)
{
	_SetMapInfo(pInfo, pAddInfo);
	I3TRACE("UIToolTipLobbyMapInfo::ChangeToolTip()\n");
}