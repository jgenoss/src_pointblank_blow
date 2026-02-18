#include "pch.h"
#include "UIFloatMissionBoard_V10.h"

#include "UI/UIMainFrame_V10.h"
#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCard.h"
#include "MissionCard/MCardQuest.h"

I3_CLASS_INSTANCE( UIFloatMissionBoard_V10);

#define MISSIONBOARD_SIDESIZE 13.0f

static UIFloatMissionBoard_V10 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIFloatMissionBoard_V10*) pFrame->GetFloating( UFW_MISSIONBOARD );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {

	int UIFloatMissionBoard_V10_ClickButton( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		GetThis()->Click(idx);
		return 0;
	}
}

LuaFuncReg UIFloatMissionBoard_V10_Glue[] = 
{
	{ "Click",		UIFloatMissionBoard_V10_ClickButton	},
	{ nullptr,			nullptr}
};

UIFloatMissionBoard_V10::UIFloatMissionBoard_V10() : m_BastTextHeight(0.f), m_pMissionBoardWnd(nullptr), m_TextHeight(0.f)
, m_pButtomBoxImage(nullptr), m_BaseButtomBoxPosY(0.f), m_pHideBoardWnd(nullptr), m_origMinTextWidth(0.f), m_pTitleText(nullptr)
, m_pHideTitleText(nullptr), m_pMainWnd(nullptr)
{
	m_AlignedPos = UIFLOATING_POS_LEFTTOP;
	m_bFixedWindow = true;
}

UIFloatMissionBoard_V10::~UIFloatMissionBoard_V10()
{

}

void UIFloatMissionBoard_V10::OnCreate( i3GameNode * pParent )
{
	UIFloatingBase::OnCreate(pParent);
	
	AddScene("Scene/Main/PointBlankRe_MissionBoard.i3UIs", UIFloatMissionBoard_V10_Glue);
}

void UIFloatMissionBoard_V10::_InitializeAtLoad( i3UIScene * pScene )
{
	UIFloatingBase::_InitializeAtLoad(pScene);

	m_pMissionBoardWnd = (i3UIFrameWnd *) pScene->FindChildByName("MissionFrameWnd");
	if(m_pMissionBoardWnd != nullptr)
	{
		m_BastTextHeight = m_pMissionBoardWnd->getHeight();
	}

	m_pHideBoardWnd = (i3UIFrameWnd *) pScene->FindChildByName("HideFrameWnd");
	if( m_pHideBoardWnd != nullptr)
	{
		m_pHideBoardWnd->EnableCtrl(false);
	}

	m_apText[0] = (i3UIStaticText *)pScene->FindChildByName("MissionString0");
	m_apText[1] = (i3UIStaticText *)pScene->FindChildByName("MissionString1");
	m_apText[2] = (i3UIStaticText *)pScene->FindChildByName("MissionString2");
	m_apText[3] = (i3UIStaticText *)pScene->FindChildByName("MissionString3");

	m_pTitleText = (i3UIStaticText *)pScene->FindChildByName("Static_MissionBoardTitle");
	m_pHideTitleText = (i3UIStaticText *)pScene->FindChildByName("Static_MissionBoardTitle2");

	m_origMinFrameWidth = (m_pMissionBoardWnd) ? m_pMissionBoardWnd->getSize()->x : 0.f;
	m_origMinTextWidth = (m_apText[0]) ? m_apText[0]->getSize()->x : 0.f;

	m_pButtomBoxImage = (i3UIImageBox *) pScene->FindChildByName("ButtomBox");
	if(m_pButtomBoxImage != nullptr)
	{
		m_BaseButtomBoxPosY = m_pButtomBoxImage->getPos()->y;
	}

	m_pMainWnd = (i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd3");

	m_ChangeUI[0].setBackFrame((i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd")	);
	m_ChangeUI[1].setBackFrame((i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd0") );
	m_ChangeUI[0].setBackButton((i3UIButton *)pScene->FindChildByName("MissionButton"));
	m_ChangeUI[1].setBackButton((i3UIButton *)pScene->FindChildByName("MissionButton2"));
}

bool UIFloatMissionBoard_V10::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if( UIFloatingBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;
	
	// 정보 세팅하기
	SetMissionCardSlot();
	return true;
}

void UIFloatMissionBoard_V10::SetMissionCardSlot( )
{
	MCard* pSelCard = MCardUtil::GetSelectedMCard();
	if(pSelCard == nullptr) return;
	
	// 전부 초기화 시킨다.
	LuaState * L = _CallLuaFunction("SetDefault");
	_EndLuaFunction(L, 0);

	INT32 count = -1;
	INT32 maxFitWidth = 0;

	for(INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW; i++)
	{
		MCardQuest* pQuest = pSelCard->GetMCardQuest(i);
		
		if (pQuest == nullptr || pQuest->IsActive() == false)
			continue;

		// 성공
		if ( pSelCard->IsQuestCompleted_OffBattle(i) )
		{
			L = _CallLuaFunction("SetMissionCheckBox");
			i3Lua::PushInteger(L, i);
			i3Lua::PushInteger(L, 2);
			_EndLuaFunction(L, 2);
		}
		else if(pQuest->GetCurrCompleteCount_OffBattle() > 0)
		{
			L = _CallLuaFunction("SetMissionCheckBox");
			i3Lua::PushInteger(L, i);
			i3Lua::PushInteger(L, 1);
			_EndLuaFunction(L, 2);
		}
		else
		{
			L = _CallLuaFunction("SetMissionCheckBox");
			i3Lua::PushInteger(L, i);
			i3Lua::PushInteger(L, 0);
			_EndLuaFunction(L, 2);
		}

		i3UIStaticText* pTextCtrl =	m_apText[count+1];		// 일단 -1로 시작되므로, 더하기 1을 해본다..

		if (pTextCtrl)
		{
			i3::rc_wstring wstrDescription = MCardUtil::GetCurrHUDDesc(i);
			
			pTextCtrl->setSize(300.f, pTextCtrl->getSize()->y);	// 가로로 최대한 늘리면 안됨. 
			pTextCtrl->SetText(wstrDescription);

			INT32 fitWidth = pTextCtrl->getUIText()->getExtentWidth() 
							 + pTextCtrl->getTextSpaceX() * 2 + 2;	// ExtentWidth가 해상도에 따른 오차 오류를 갖고 있어 2늘려본다..
			pTextCtrl->setSize( (REAL32) fitWidth, pTextCtrl->getSize()->y);
		
			if (fitWidth > maxFitWidth)
				maxFitWidth = fitWidth;
		}

//		L = _CallLuaFunction("SetMission");
//		i3Lua::PushInteger(L, i);
//		i3Lua::PushStringUTF16To8(L, wstrDescription);
//		_EndLuaFunction(L, 2);

		// 하나 씩 증가시킵니다.
		count++;
	}
	
	if ( REAL32(maxFitWidth) > m_origMinTextWidth )
	{
		REAL32 diff = REAL32(maxFitWidth) - m_origMinTextWidth;
		// 가로를 좀 더 늘려줘야 합니다.
		// Title, Hide Frame도 같이 작업 해줘야 다르지 않아 보입니다.
		m_pMissionBoardWnd->setSize( m_origMinFrameWidth + REAL32(diff) + MISSIONBOARD_SIDESIZE, m_pMissionBoardWnd->getSize()->y);
		m_pHideBoardWnd->setSize( m_origMinFrameWidth + REAL32(diff) + MISSIONBOARD_SIDESIZE, m_pHideBoardWnd->getSize()->y);

		m_pTitleText->setSize( m_origMinFrameWidth + REAL32(diff) + MISSIONBOARD_SIDESIZE, m_pTitleText->getSize()->y);
		m_pHideTitleText->setSize( m_origMinFrameWidth + REAL32(diff) + MISSIONBOARD_SIDESIZE, m_pHideTitleText->getSize()->y);

		m_pMainWnd->setSize(m_origMinFrameWidth + REAL32(diff) + MISSIONBOARD_SIDESIZE, m_pMainWnd->getSize()->y);
	}
	else
	{
		m_pMissionBoardWnd->setSize( m_origMinFrameWidth, m_pMissionBoardWnd->getSize()->y);
		m_pHideBoardWnd->setSize( m_origMinFrameWidth, m_pHideBoardWnd->getSize()->y);

		m_pTitleText->setSize( m_origMinFrameWidth, m_pTitleText->getSize()->y);
		m_pHideTitleText->setSize( m_origMinFrameWidth, m_pHideTitleText->getSize()->y);

		m_pMainWnd->setSize(m_origMinFrameWidth, m_pMainWnd->getSize()->y);
	}

	m_ChangeUI[0].setWidth(m_pMissionBoardWnd->getWidth());
	m_ChangeUI[1].setWidth(m_pMissionBoardWnd->getWidth());

	// 아무것도 없을 경우 끕니다.
	if(count == -1)
	{
		SetEnable(false);
	}
	else
	{
		SetEnable(true);
		_GetUIControlSize();
		_SetUIControl(count);

		for(INT32 i = 3; i > count; i--)
		{
			L = _CallLuaFunction("SetMissionCheckBoxEnable");
			i3Lua::PushInteger(L, i);
			i3Lua::PushBoolean(L, false);
			_EndLuaFunction(L, 2);

		}
	}

}

void UIFloatMissionBoard_V10::_GetUIControlSize()
{
	if(m_TextHeight == 0)
	{
		if(m_apText[0] != nullptr)
			m_TextHeight = m_apText[0]->getHeight();
	}
}

void UIFloatMissionBoard_V10::_SetUIControl(INT32 count)
{
	REAL32 height = m_BastTextHeight - ((3 - count) * m_TextHeight);
	REAL32 width = m_pMissionBoardWnd->getWidth();

	m_pMissionBoardWnd->setSize(width, height);

	VEC3D * pos = m_pButtomBoxImage->getPos();
	pos->y = m_BaseButtomBoxPosY - ((3-count) * m_TextHeight);
	m_pButtomBoxImage->setPos(pos);
}

void UIFloatMissionBoard_V10::Click( INT32 idx )
{
	if( idx  == 0 )
	{
		m_pMissionBoardWnd->EnableCtrl(false);
		m_pHideBoardWnd->EnableCtrl(true);
	}
	else
	{
		m_pMissionBoardWnd->EnableCtrl(true);
		m_pHideBoardWnd->EnableCtrl(false);
	}
}
