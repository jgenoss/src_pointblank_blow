#include "pch.h"
#if defined(SKILLSYSTEM)
#include "UITabProfileAbility.h"
#include "UIMainFrame.h"
#include "UIPhaseProfile.h"
#include "UITabProfileSkill.h"
#include "SkillManager.h"
#include "UIUtil.h"


static UITabProfileAbility * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	UIPopupBase* pSubPhase = pFrame->GetPopup(UPW_PROFILE);
	UITabProfileBase* pTab = static_cast<UIPhaseProfile*>(pSubPhase)->GetCurrentTab();
	I3ASSERT( i3::same_of<UITabProfileSkill*>(pTab)); //->IsExactTypeOf( UITabProfileSkill::static_meta()));
	UISkillMenu * pScene = static_cast<UITabProfileSkill*>(pTab)->GetCurrentScene();
	return static_cast<UITabProfileAbility*>(pScene);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UITabProfile_Ability_Scroll( LuaState * L)
	{
		GetThis()->Scroll();
		return 0;
	}
	int UITabProfile_Ability_Learn( LuaState * L)
	{
		GetThis()->Learn();
		return 0;
	}
}

LuaFuncReg UITabProfileAbility_Glue[] =
{
	{"Scroll",				UITabProfile_Ability_Scroll	},
	{"Learn",				UITabProfile_Ability_Learn	},
	{NULL,					NULL						},
};

I3_CLASS_INSTANCE( UITabProfileAbility);//, UISkillMenu);

UITabProfileAbility::UITabProfileAbility()
{
	m_StartIdx		= 0;
	m_CurrentSlot	= -1;
	m_pImageDummy	= NULL;
	m_pScrollBar	= NULL;
	for(INT32 i=0; i<MAX_SKILL_SLOT; i++)
		m_SlotSet[i] = NULL;
}

UITabProfileAbility::~UITabProfileAbility()
{
	for(INT32 i = 0; i < MAX_SKILL_SLOT; i++)
	{
		if( m_SlotSet[i])
		{
			m_SlotSet[i]->Destroy();
			m_SlotSet[i] = NULL;
		}
	}
}

void UITabProfileAbility::UpdateSlotList()
{
	UINT8 _Class = g_pGameContext->getSkillManager()->getCurrentClass(GetCurrentAvatar(), GetCurrentTab());
	INT32 _SlotCount = g_pGameContext->getSkillManager()->getMaxSkillCountByClass(_Class);

	for(INT32 i=0; i<MAX_SKILL_SLOT; i++)
	{
		if(i + m_StartIdx > _SlotCount - 1)
		{
			m_SlotSet[i]->SetSlotEnable(FALSE);
			continue;
		}
		m_SlotSet[i]->SetSkillImage(i + m_StartIdx);//SetSkillImage(_SkillImage[i]);
		m_SlotSet[i]->SetSkillText(i + m_StartIdx);
		m_SlotSet[i]->SetSlotEnable(TRUE);

		if( _Class > SKILL_SHOTGUN)
		{
			m_SlotSet[i]->SetCheckBoxEnable(5);
		}
		else
		{
			m_SlotSet[i]->SetCheckBoxEnable((INT32)_Class);
		}
	}
	for(INT32 i=0; i<MAX_SKILL_SLOT; i++)
	{
		SkillManager::SLOT_STATE _Slot = (SkillManager::SLOT_STATE)GetSlotState(i + m_StartIdx);

		switch(_Slot)
		{
		case SkillManager::SKILL_STATE_NONE:
			if(getCheckEnable(i + m_StartIdx) == FALSE)	// 선택 가능 상태
			{
				m_CurrentSlot = i;
				m_SlotSet[i]->SetBG(FALSE);
				m_SlotSet[i]->SetArrowDirection(3,0);
				m_SlotSet[i]->SetSelectionSlot(3);
			}
			else
			{
				m_SlotSet[i]->SetBG(FALSE);
				m_SlotSet[i]->SetArrowDirection(-1,-1);
				m_SlotSet[i]->SetSelectionSlot(0);
			}
			
			break;
		case SkillManager::SKILL_STATE_LEFT:
		case SkillManager::SKILL_STATE_RIGHT:
			m_SlotSet[i]->SetSelectionSlot(_Slot);
			m_SlotSet[i]->SetBG(FALSE);
			if(_Slot == 1)
				m_SlotSet[i]->SetArrowDirection(3,-1);
			else
				m_SlotSet[i]->SetArrowDirection(-1,0);
			break;
		default:
			return;
		}
	}
}

INT32 UITabProfileAbility::GetSlotState(INT32 idx)
{
	SkillManager * skillMng = g_pGameContext->getSkillManager();
	return skillMng->GetSlotState(GetCurrentAvatar(), GetCurrentTab(), idx);
}

BOOL UITabProfileAbility::getCheckEnable(INT32 idx)
{
	if(idx == 0)
		return FALSE;
	SkillManager::SLOT_STATE _Slot = (SkillManager::SLOT_STATE)GetSlotState(idx - 1);
	if(_Slot > SkillManager::SKILL_STATE_NONE)
		return FALSE;	//슬롯 선택 가능
	return TRUE;		//슬롯 선택 불가
}

void UITabProfileAbility::LoadUserInfo()
{
	SkillManager * skillMng = g_pGameContext->getSkillManager();
	CCharaInfoContext * CChara = g_pFramework->GetCharaInfoContext();
	const CHARA_PARTS_INFO* parts = CChara->GetCharaPartsInfo(GetCurrentAvatar());
	INT32 nCharaImageID = 0;
	if( parts )
		nCharaImageID = GameUI::GetCharaImageIndex( parts->GetCharaItemID() );
	
	LuaState * L = _CallLuaFunction("LoadUserInfo");
	i3Lua::PushA2UTF8String(L, CChara->GetNickName(GetCurrentAvatar()));								//UserNick
	i3Lua::PushInteger(L, nCharaImageID);												//Avatar Image index
	i3Lua::PushInteger(L, skillMng->getCurrentClass(GetCurrentAvatar(), GetCurrentTab()));		//Skill Image index
	_EndLuaFunction(L, 3);

	LoadPlayPointInfo();
}

void UITabProfileAbility::LoadSkillSlotInfo(UISkillSelectionSlot* _slot)
{
	SkillManager * skillMng = g_pGameContext->getSkillManager();
	char _CostStr[MAX_STRING_COUNT];
	char _SkillNum[MAX_STRING_COUNT];
	INT32 ImgIdx = 0;
	INT32 line = 0;
	
	if(_slot != NULL)
	{
		for(line=0; line < MAX_SKILL_SLOT; line++)
		{
			if(m_SlotSet[line] != NULL && m_SlotSet[line] == _slot)
				break;
		}
		ImgIdx = line + m_StartIdx;
	}
	else
	{
		ImgIdx = m_CurrentSlot + m_StartIdx;
	}

	eSKILL_CLASS _Class= (eSKILL_CLASS)skillMng->getCurrentClass(GetCurrentAvatar(), GetCurrentTab());

	INT32 cost = 0;
	if(_slot != NULL)
	{
		cost = skillMng->getSkillCostByClass(_Class, ImgIdx);
		sprintf_s(_SkillNum, "[No.%d]", ImgIdx+1);
	}
	else
	{
		cost = skillMng->getSkillCostByClass(_Class, m_CurrentSlot);
		sprintf_s(_SkillNum, "[No.%d]", m_CurrentSlot+1);
	}
	sprintf_s(_CostStr, GAME_STRING("STR_TBL_SKILL_SELECT_COST"), cost);

	LuaState * L = _CallLuaFunction("LoadSkillSlotInfo");
	i3Lua::PushInteger(L, ImgIdx + skillMng->getImageStartIndexByClass(_Class));
	i3Lua::PushA2UTF8String(L, _SkillNum);
	i3Lua::PushA2UTF8String(L, _CostStr);
	_EndLuaFunction(L,3);
}

void UITabProfileAbility::LoadSkillSlotDetailInfo(UISkillSelectionSlot* _slot, INT32 _state)
{
	SkillManager * skillMng = g_pGameContext->getSkillManager();
	INT32 line = 0;
	for(line=0; line < MAX_SKILL_SLOT; line++)
	{
		if(m_SlotSet[line] != NULL && m_SlotSet[line] == _slot)
			break;
	}
	
	INT32 ImgIdx = line + m_StartIdx;
	SkillManager::SLOT_DIRECTION dir = static_cast<SkillManager::SLOT_DIRECTION>(_state);
	eSKILL_CLASS _Class= (eSKILL_CLASS)skillMng->getCurrentClass(GetCurrentAvatar(), GetCurrentTab());
	const INT32 _Skill = skillMng->getSkillByClass(_Class,dir,ImgIdx);

	const char * _SubStr = skillMng->getSkillSubName(_Skill);
	const char * _Comment = skillMng->getSkillComment(_Skill);
	
	LuaState * L = _CallLuaFunction("LoadSkillSlotDetailInfo");
	i3Lua::PushA2UTF8String(L, _SubStr);
	i3Lua::PushA2UTF8String(L, _Comment);
	_EndLuaFunction(L,2);
}

void UITabProfileAbility::LoadPlayPointInfo()
{
	char _playPoint[MAX_STRING_COUNT] = {0,};
	SkillManager * skillMng = g_pGameContext->getSkillManager();

	UINT16 _point = skillMng->getSkillPlayPoint(GetCurrentAvatar(), GetCurrentTab());
	UINT32 _time = skillMng->getSkillPlayTime(GetCurrentAvatar(), GetCurrentTab());
	sprintf_s(_playPoint, "%d", _point);
	float time = (float)_time / 3600;

	LuaState * L = _CallLuaFunction("LoadPlayPointInfo");
	i3Lua::PushNumber(L, time);
	i3Lua::PushA2UTF8String(L, _playPoint);
	_EndLuaFunction(L,2);
}

void UITabProfileAbility::Learn()
{
	SKILL_PROPERTY _Skill;
	SkillManager * skillMng = g_pGameContext->getSkillManager();

	UINT8 _class = skillMng->getCurrentClass(GetCurrentAvatar(), GetCurrentTab());
	INT32 _SlotCount = skillMng->getMaxSkillCountByClass(_class);

	INT32	_cost	= skillMng->getSkillCostByClass(_class, m_CurrentSlot);
	UINT16	_myPoint= skillMng->getSkillPlayPoint(GetCurrentAvatar(), GetCurrentTab());
	if(m_CurrentSlot < _SlotCount && _myPoint != 0 && _myPoint >= _cost)

	if(m_CurrentSlot < _SlotCount)
	{
		_Skill._currentAvater = GetCurrentAvatar();
		_Skill._currentTab = GetCurrentTab();
		_Skill._class = skillMng->getCurrentClassForPacket(GetCurrentAvatar(), GetCurrentTab());
		_Skill._line = m_CurrentSlot + m_StartIdx;
		_Skill._direction = m_SlotSet[m_CurrentSlot]->GetDirection();
		if(_Skill._direction == 0)
			return;
		skillMng->setSkillTree(GetCurrentAvatar(), GetCurrentTab(),_Skill._line, _Skill._direction, &_Skill._skilltree);

		GetMainFrame()->OpenPopup(UPW_SKILL_LEARN, (void*)&_Skill);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_NO_POINT"));
	}
}

void UITabProfileAbility::Scroll()
{
	if(m_pScrollBar == NULL)
		return;
	m_StartIdx = m_pScrollBar->getScrollPos();
	if(m_StartIdx < 0)	m_StartIdx = 0;
	I3TRACE("Scroll position : %d\n", m_StartIdx);
	UpdateSlotList();
}

void UITabProfileAbility::CreateSelectSlot()
{
	for( INT32 i=0; i < MAX_SKILL_SLOT; i++)
	{
		if( NULL == m_SlotSet[i] )
		{
			m_SlotSet[i] = UISkillSelectionSlot::new_object();
			m_SlotSet[i]->OnCreate(this);
		}
	}
}

void UITabProfileAbility::InitScroll()
{
	SkillManager * skillMng = g_pGameContext->getSkillManager();

	UINT8 _class = skillMng->getCurrentClass(GetCurrentAvatar(), GetCurrentTab());
	INT32 _SlotCount = skillMng->getMaxSkillCountByClass(_class);
	INT32 SliderMax = _SlotCount - MAX_SKILL_SLOT;

	m_pScrollBar->SetEnable( TRUE);
	m_pScrollBar->setPageSize(1);
	m_pScrollBar->setScrollRange( 0, SliderMax);
}

/*virtual*/ void UITabProfileAbility::_InitializeAtLoad(i3UIScene * pScene)
{
	CreateSelectSlot();
	m_pImageDummy = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameSkill");
	I3ASSERT( m_pImageDummy);
	m_pScrollBar = (i3UIScrollBar*) pScene->FindChildByName( "i3UIScrollBar");
	m_pScrollBar->setTickSize( 1);

	VEC3D vPos, vParentPos;
	VEC2D vSize;
	i3Vector::Zero( &vPos);
	i3Vector::Copy( &vParentPos, m_pImageDummy->getAccumulatedPos());

	m_SlotSet[0]->SetEnable( TRUE);
	i3Vector::Copy( &vSize, m_SlotSet[0]->GetSize());
	m_SlotSet[0]->SetEnable( FALSE);

	i3Vector::Add( &vParentPos, 3.f, 3.f, 0.f);

	for(INT32 i = 0; i < MAX_SKILL_SLOT; i++)
	{
		I3ASSERT( m_SlotSet[i] != NULL);

		m_SlotSet[i]->CalculatePos( m_SlotSet[i]->GetOriginalPos(), 0, i, &vParentPos, &vSize, 0.0f, 10.0f);
	}
}

/*virtual*/ void UITabProfileAbility::OnCreate( i3GameNode * pParent)
{
	UISkillMenu::OnCreate( pParent);
	AddScene( "UIRe/Scene/Main/PointBlankRe_Profile_Skill_Select.i3UIe", UITabProfileAbility_Glue);
	//CreateSelectSlot();
}

/*virtual*/ void UITabProfileAbility::OnMenuExit( void)
{
	UISkillMenu::OnMenuExit();

	for( INT32 i=0; i < MAX_SKILL_SLOT; i++)
	{
		if( m_SlotSet[i] )
		{
			m_SlotSet[i]->Destroy();
			m_SlotSet[i] = NULL;
		}
	}
}

/*virtual*/ void UITabProfileAbility::OnMenuStart()
{	
	m_StartIdx = 0;
	m_CurrentSlot = 0;

	for(INT32 i=0; i<MAX_SKILL_SLOT; i++)
		m_SlotSet[i]->EnableSlot( 0, i, NULL, NULL);
	for(INT32 i = 0; i < MAX_SKILL_SLOT; i++)
	{
		if(m_SlotSet[i] != NULL)
		{
			m_SlotSet[i]->SetVisible( TRUE);
		}
	}

	InitScroll();
	UpdateSlotList();
	LoadUserInfo();
	LoadSkillSlotInfo();
	g_pFramework->SetFocus(m_pScrollBar);
}

void UITabProfileAbility::SetInputDisable(bool bDisable)
{
	UISkillMenu::SetInputDisable(bDisable);
	for(INT32 i = 0; i < MAX_SKILL_SLOT; i++)
	{
		if(m_SlotSet[i] != NULL)
		{
			m_SlotSet[i]->SetInputDisable(bDisable);
		}
	}
}

void UITabProfileAbility::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData )
{
	switch(event)
	{
	case EVENT_SKILL_SLOT_LEARN:
		switch(arg)
		{
		case EVENT_ERROR_SUCCESS:
			//GameUI::MsgBox( MSG_TYPE_GAME_QUERY, "슬롯 획득에 성공하였습니다.");
			UpdateSlotList();
			LoadUserInfo();
			LoadSkillSlotInfo();
			break;

		case EVENT_ERROR_PBV15_SPECIALITY_SET_USER				:	// 주특기 셋팅 실패 - 유저가 없다.
			break;
		case EVENT_ERROR_PBV15_SPECIALITY_SET_CHARA_SLOT		:	// 주특기 셋팅 실패 - 잘못된 캐릭터 슬롯 번호 
			break;
		case EVENT_ERROR_PBV15_SPECIALITY_SET_TYPE_CLASS		:	// 주특기 셋팅 실패 - 셋팅 요청한 주특기 타입 또는 클래스가 잘못 되었다.
			break;
		case EVENT_ERROR_PBV15_SPECIALITY_SET_DIFF_TYPE_CLASS	:	// 주특기 셋팅 실패 - 클라/서버 주특기 정보(타입,클래스)가 다르다.
			break;
		case EVENT_ERROR_PBV15_SPECIALITY_SET_SKILL_TREE		:	// 주특기 셋팅 실패 - 잘못된 스킬트리 정보를 보냈다. 
			break;
		case EVENT_ERROR_PBV15_SPECIALITY_SET_SEARCH_RANK_POINT	:	// 주특기 셋팅 실패 - 스킬 구입시 체크하는 랭크 및 차감 플레이 타임 포인트 검색 실패.
			break;
		case EVENT_ERROR_PBV15_SPECIALITY_SET_RANK				:	// 주특기 셋팅 실패 - 스킬 구입시 자신의 랭크가 제한 랭크 보다 작다.
			break;
		case EVENT_ERROR_PBV15_SPECIALITY_SET_POINT				:	// 주특기 셋팅 실패 - 스킬 구입시 자신의 플레이 타임 포인트가 차감 플레이 타임 포인트 보다 적다.
			break;
		default:
			break;											// 알수없는 이유
		}
		break;
	}
}

#endif