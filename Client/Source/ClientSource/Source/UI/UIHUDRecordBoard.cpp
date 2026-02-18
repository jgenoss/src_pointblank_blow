#include "pch.h"
#include "UIHUDRecordBoard.h"
#include "UIBattleFrame.h"
#include "ItemImageMgr.h"
#include "StageBattle.h"
#include "RevengeManager.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

#include "i3Base/string/compare/generic_is_equal.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

static UIHUDRecordBoard * GetThis()
{
	UIBattleFrame * pFrame = UIBattleFrame::i();

	return (UIHUDRecordBoard*) pFrame->GetHUD( UIHUD_RECORD_BOARD);
}

extern "C"
{
}

LuaFuncReg UIHUDRecordBoard_Glue[] = {
//	{"Vote_End",				UIHUDForciblyRemove_Vote_End			},
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIHUDRecordBoard);//, UIHUDBase);

UIHUDRecordBoard::UIHUDRecordBoard()
{
	m_bStartRecordBoard = false;
	t_Record = 0.0f;
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT; ++i)
	{
		m_Revenge[i].nWinPoint = 0;
		m_Revenge[i].nLosePoint = 0;
	}
}

UIHUDRecordBoard::~UIHUDRecordBoard()
{
}
 
void UIHUDRecordBoard::UIRecordRevenge(INT32 index, INT32 nPoint)
{
	i3_prof_func();

	const SLOT_INFO * pSlotInfo;
	m_index = index;

	if(BattleSlotContext::i()->IsRedTeam())
	{
		// Red
		for(INT32 i=0; i< SLOT_MAX_COUNT; i++)
		{				
			if(BattleSlotContext::i()->IsRedTeam(i) == false)
			{	
				pSlotInfo = BattleSlotContext::i()->getSlotInfo(i);

				if(pSlotInfo->_State == SLOT_STATE_BATTLE)
				{
					if( m_Revenge[i].wstrTargetName != BattleSlotContext::i()->getNickForSlot(i) )
					{
						m_Revenge[i].wstrTargetName = BattleSlotContext::i()->getNickForSlot(i);
						m_Revenge[i].nWinPoint = 0;
						m_Revenge[i].nLosePoint = 0;
					}
				}
			}
		}
	}
	else
	{
		// Blue
		for(INT32 i=0; i< SLOT_MAX_COUNT; i++)
		{				
			if(BattleSlotContext::i()->IsRedTeam(i))
			{	
				pSlotInfo = BattleSlotContext::i()->getSlotInfo(i);

				if(pSlotInfo->_State == SLOT_STATE_BATTLE)
				{
					if( m_Revenge[i].wstrTargetName != BattleSlotContext::i()->getNickForSlot(i) )
					{
						m_Revenge[i].wstrTargetName = BattleSlotContext::i()->getNickForSlot(i);
						m_Revenge[i].nWinPoint = 0;
						m_Revenge[i].nLosePoint = 0;
					}
				}
			}
		}
	}

	for(INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		if( m_Revenge[i].wstrTargetName == BattleSlotContext::i()->getNickForSlot(m_index)) 
		{
			if( nPoint < 0)
			{
				m_Revenge[i].nLosePoint -= nPoint;
			}
			else
			{
				m_Revenge[i].nWinPoint += nPoint;
			}
		}
	}

}

/*virtual*/ void UIHUDRecordBoard::_InitializeAtLoad(i3UIScene * pScene)
{
	i3_prof_func();

	UIHUDBase::_InitializeAtLoad(pScene);
	m_pLeftText = (i3UIStaticText*)pScene->FindChildByName("i3UIStaticTextL");
	m_pRightText = (i3UIStaticText*)pScene->FindChildByName("i3UIStaticTextR");
}

/*virtual*/ void UIHUDRecordBoard::OnCreate( i3GameNode * pParent)
{
	i3_prof_func();

	UIHUDBase::OnCreate( pParent);

	// load scene
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == false)
	{
		// 튜토리얼에서는 로드하지 않도록
		AddScene("Scene/HUD/PBRe_HUDRecordBoard.i3UIs", UIHUDRecordBoard_Glue);

	}
}

void	UIHUDRecordBoard::OnUpdate( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	UIHUDBase::OnUpdate(rDeltaSeconds);

	if(m_bStartRecordBoard)
	{
		t_Record += rDeltaSeconds;
		if( t_Record > 2.5f )
		{
			I3TRACE("Record Time Trace End : %f", t_Record);
			UIBattleFrame * pFrame = UIBattleFrame::i();
			pFrame->EnableHUD( UIHUD_RECORD_BOARD, false);
			m_bStartRecordBoard = false;
			t_Record = 0.0f;
		}
	}
}

/*virtual*/ bool UIHUDRecordBoard::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	i3_prof_func();

	if( CRevengeManager::i() == nullptr)
		return false;

	// revision 57605 폭파 오브젝트로 죽으면 1:1전적창에 무기 이미지가 없음 (hansoft.5118)
	INT32 Item_ID = CRevengeManager::i()->GetDeathWeaponNum(); // Item_ID가 -1이면 오브젝트에 의해 죽은 경우다 ( 헬기는 M197무기 이미지를 표시하도록 처리되었음 )
	if(Item_ID == -1) return false;

	if( UIHUDBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	i3::stack_wstring wstrTemp;

	m_bStartRecordBoard = true;

	t_Record = 0.0f;
	INT32 MySlotIdx = BattleSlotContext::i()->getMySlotIdx();

	LuaState* L = nullptr;
	i3UIButtonImageSet* btnset = nullptr;

	i3UIScene * pScene = UIBase::GetScene("PBRe_HUDRecordBoard");
	L = UILua::CallLuaFunction(pScene, "GetImageSetCtrl");
	if( L != nullptr )
	{
		btnset = reinterpret_cast< i3UIButtonImageSet* >(UILua::ReturnLuaPointer(L, 0));
		m_ItemSetCtrl = GetItemImageMgr()->CreateImgSetCtrl(btnset);
	}

	SetWeaponFrame(true);
	m_ItemSetCtrl->SetVisible(true);
	GetItemImageMgr()->SelectImage(m_ItemSetCtrl, Item_ID);

	//무기 이름 설정
	SetWeaponName( CRevengeManager::i()->GetDeathWeaponName());

	for(INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		if( m_Revenge[i].wstrTargetName == BattleSlotContext::i()->getNickForSlot(m_index) )
		{
			if( BattleSlotContext::i()->IsRedTeam(m_index))
			{
				SetRedTargetName(m_Revenge[i].wstrTargetName);
				i3::sprintf( wstrTemp, L"%d", m_Revenge[i].nLosePoint);
				SetRedTargetCount(wstrTemp);
				SetRedTargetOnColor();

				SetBlueTargetName(BattleSlotContext::i()->getNickForSlot(MySlotIdx));
				i3::sprintf( wstrTemp, L"%d", m_Revenge[i].nWinPoint);
				SetBlueTargetCount(wstrTemp);
			}
			else
			{
				// Blue(Enemy)
				SetBlueTargetName(m_Revenge[i].wstrTargetName);
				i3::sprintf( wstrTemp, L"%d", m_Revenge[i].nLosePoint);
				SetBlueTargetCount(wstrTemp);
				SetBlueTargetOnColor();

				SetRedTargetName(BattleSlotContext::i()->getNickForSlot(MySlotIdx));
				i3::sprintf( wstrTemp, L"%d", m_Revenge[i].nWinPoint);
				SetRedTargetCount(wstrTemp);
			}	
			break;
		}
	}

	return true;
}

void	UIHUDRecordBoard::OnExitEnd( void)
{
	I3_SAFE_RELEASE(m_ItemSetCtrl);
	UIHUDBase::OnExitEnd();
}

void UIHUDRecordBoard::SetWeaponFrame(bool bEnable)
{
	i3_prof_func();

	LuaState * L = _CallLuaFunction("SetWeaponFrame");
	i3Lua::PushBoolean(L, bEnable);
	_EndLuaFunction(L, 1);
}

void UIHUDRecordBoard::SetWeaponName(const i3::rc_wstring& strName)
{
	i3_prof_func();

	LuaState * L = _CallLuaFunction("SetWeaponName");
	i3Lua::PushStringUTF16To8(L, strName.c_str(), strName.size() );
	_EndLuaFunction(L, 1);
}

void UIHUDRecordBoard::SetRedTargetName(const i3::rc_wstring& wstrText)
{
	LuaState * L = _CallLuaFunction("SetRedTargetName");
	i3Lua::PushStringUTF16To8(L, wstrText);
	_EndLuaFunction(L, 1);
}

void UIHUDRecordBoard::SetBlueTargetName(const i3::rc_wstring& wstrText)
{
	i3_prof_func();

	LuaState * L = _CallLuaFunction("SetBlueTargetName");
	i3Lua::PushStringUTF16To8(L, wstrText);
	_EndLuaFunction(L, 1);
}

void UIHUDRecordBoard::SetRedTargetCount(const i3::wliteral_range& wTextRng)
{
	i3_prof_func();

	LuaState * L = _CallLuaFunction("SetRedTargetCount");
	i3Lua::PushStringUTF16To8(L, wTextRng);
	_EndLuaFunction(L, 1);
}

void UIHUDRecordBoard::SetRedTargetOnColor()
{
	i3_prof_func();

	LuaState * L = _CallLuaFunction("SetRedTargetOnColor");
	_EndLuaFunction(L, 0);
}

void UIHUDRecordBoard::SetBlueTargetCount(const i3::wliteral_range& wTextRng)
{
	i3_prof_func();

	LuaState * L = _CallLuaFunction("SetBlueTargetCount");
	i3Lua::PushStringUTF16To8(L, wTextRng);
	_EndLuaFunction(L, 1);
}

void UIHUDRecordBoard::SetBlueTargetOnColor()
{
	i3_prof_func();

	LuaState * L = _CallLuaFunction("SetBlueTargetOnColor");
	_EndLuaFunction(L, 0);
}

/*virtual*/ void UIHUDRecordBoard::ClearData()
{
	if( g_pEnvSet->m_b1VS1)
	{
		m_Revenge[m_index].wstrTargetName.clear();
		m_Revenge[m_index].nWinPoint = 0;
		m_Revenge[m_index].nLosePoint = 0;
	}
}

