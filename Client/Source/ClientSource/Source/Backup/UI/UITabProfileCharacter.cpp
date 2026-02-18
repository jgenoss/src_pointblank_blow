#include "pch.h"
#if defined(SKILLSYSTEM)
#include "UITabProfileCharacter.h"
#include "UIMainFrame.h"
#include "UIPhaseProfile.h"
#include "UITabProfileSkill.h"
#include "UIMovieSetCtrl.h"
#include "CommunityContext.h"
#include "UIUtil.h"


static UITabProfileCharacter * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	UIPopupBase* pSubPhase = pFrame->GetPopup(UPW_PROFILE);
	UITabProfileBase* pTab = static_cast<UIPhaseProfile*>(pSubPhase)->GetCurrentTab();
	I3ASSERT( i3::same_of<UITabProfileSkill*>(pTab)); //->IsExactTypeOf( UITabProfileSkill::static_meta()));
	UISkillMenu * pScene = static_cast<UITabProfileSkill*>(pTab)->GetCurrentScene();
	return static_cast<UITabProfileCharacter*>(pScene);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UITabProfile_SelectCharacter( LuaState * L)
	{
		GetThis()->PushSetButton();
		return 0;
	}
	int UITabProfile_Scroll( LuaState * L)
	{
		GetThis()->UpdateScroll();
		return 0;
	}
}

LuaFuncReg UITabProfileCharacter_Glue[] =
{
	{ "SelectCharacter",	UITabProfile_SelectCharacter},
	{ "UpdateScroll",		UITabProfile_Scroll			},
	{NULL,					NULL},
};

I3_CLASS_INSTANCE( UITabProfileCharacter);//, UISkillMenu);

UITabProfileCharacter::UITabProfileCharacter()
{
	m_CurrentSlot = -1;
	m_pScrollBar = NULL;

	for( INT32 i=0; i < MAX_CHARA_SLOT; i++)
	{
		m_SlotSet[i] = NULL;
	}
}

UITabProfileCharacter::~UITabProfileCharacter()
{
	for( INT32 i=0; i < MAX_CHARA_SLOT; i++)
	{
		if( m_SlotSet[i] )
		{
			m_SlotSet[i]->Destroy();
			m_SlotSet[i] = NULL;
		}
	}	
}

/*virtual*/ void UITabProfileCharacter::OnCreate( i3GameNode * pParent)
{
	UISkillMenu::OnCreate( pParent);
	// Load Scene
	AddScene( "UIRe/Scene/Main/PointBlankRe_Profile_Skill_Chara.i3UIe", UITabProfileCharacter_Glue);
}

/*virtual*/ void UITabProfileCharacter::SetInputDisable(bool bDisable)
{
	UISkillMenu::SetInputDisable(bDisable);
	for( INT32 i=0; i < MAX_CHARA_SLOT; i++)
	{
		if( m_SlotSet[i] )
			m_SlotSet[i]->SetInputDisable(bDisable);
	}	
}

/*virtual*/ void UITabProfileCharacter::OnMenuStart()
{
	I3TRACE("Profile Chara MenuStart.\n");
	m_CurrentSlot = 0;
	m_StartIdx = 0;
	INT32 i32CharaCount = g_pFramework->GetCharaInfoContext()->GetHasCharacterCount();

	m_pScrollBar->SetEnable(TRUE);
	m_pScrollBar->setTickSize( 1);
	m_pScrollBar->setPageSize(MAX_CHARA_SLOT);
	m_pScrollBar->setScrollRange( 0, i32CharaCount);
	//m_pScrollBar->setScrollPos(0);

	//for(INT32 i = 0; i < MAX_CHARA_SLOT; i++)
	//{
	//	if( m_SlotSet[i] != NULL)
	//		m_SlotSet[i]->SetVisible(TRUE);
	//}

	InitState();
	UpdateSlotList();
	SelectSlot(0, false);

	static_cast<UIPhaseProfile *>(GetMainFrame()->GetPopup(UPW_PROFILE))->setEscape(true);
}

/*virtual*/ void UITabProfileCharacter::OnMenuExit( void)
{
	I3TRACE("Profile Chara MenuExit.\n");
	UISkillMenu::OnMenuExit();

	for( INT32 i=0; i < MAX_CHARA_SLOT; i++)
	{
		if( m_SlotSet[i] )
		{
			m_SlotSet[i]->Destroy();
			m_SlotSet[i] = NULL;
		}
	}
}

/*virtual*/ void UITabProfileCharacter::_InitializeAtLoad(i3UIScene * pScene)
{
	CreateCharaSlot();
	m_pImageDummy = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameLeft");
	I3ASSERT( m_pImageDummy);
	m_pScrollBar = (i3UIScrollBar*) pScene->FindChildByName( "i3UIScrollBar");

	VEC3D vPos, vParentPos;
	VEC2D vSize;
	i3Vector::Zero( &vPos);
	i3Vector::Copy( &vParentPos, m_pImageDummy->getAccumulatedPos());

	m_SlotSet[0]->SetEnable( TRUE);
	i3Vector::Copy( &vSize, m_SlotSet[0]->GetSize());
	m_SlotSet[0]->SetEnable( FALSE);

	i3Vector::Add( &vParentPos, 3.f, 3.f, 0.f);

	for(INT32 i = 0; i < MAX_CHARA_SLOT; i++)
	{
		I3ASSERT( m_SlotSet[i] != NULL);

		m_SlotSet[i]->CalculatePos( m_SlotSet[i]->GetOriginalPos(), 0, i, &vParentPos, &vSize);
	}
}

void UITabProfileCharacter::UpdateSlotList(void)
{
	INT32 idx = 0;
	if(m_pScrollBar == NULL)
		return;

	INT32 sPos = m_pScrollBar->getScrollPos();
	if(sPos < 0)
		sPos = 0;

	CCharaInfoContext * _Chara = g_pFramework->GetCharaInfoContext();

	for(INT32 i = 0; i < MAX_CHARA_SLOT; i++)
	{
		idx = sPos + i;
		m_SlotSet[i]->EnableSlot( 0, i, NULL, NULL);
		if(idx >= _Chara->GetHasCharacterCount())
		{
			m_SlotSet[i]->OnUnselect();
			m_SlotSet[i]->SetSlotState(2);	
			continue;
		}
		m_SlotSet[i]->SetSlotState(0);	
		m_SlotSet[i]->SetSlotInfo(idx);
		m_SlotSet[i]->OnUnselect();

		INT32 selectedIdx = m_CurrentSlot - sPos;
		if(selectedIdx >= 0 && selectedIdx < MAX_CHARA_SLOT)
		{
			if(m_SlotSet[selectedIdx]->_isEmptyLuaContext() == false)
				m_SlotSet[selectedIdx]->OnSelect();
		}
	}
}

void UITabProfileCharacter::PushSetButton()
{
	CCharaInfoContext * CChara = g_pFramework->GetCharaInfoContext();
	INT32 charaIdx = CChara->GetCharaInfo(m_CurrentSlot)->getBasicInfo()->m_ui8CharaSlotIdx;
	g_pGameContext->SetEvent(EVENT_SKILL_CHARA_SET, &charaIdx);
}

void UITabProfileCharacter::CreateCharaSlot()
{
	for( INT32 i=0; i < MAX_CHARA_SLOT; i++)
	{
		if( NULL == m_SlotSet[i] )
		{
			m_SlotSet[i] = UISkillCharacterSlot::new_object();
			m_SlotSet[i]->OnCreate(this);
		}		
	}	
}

void UITabProfileCharacter::SelectSlot(INT32 idx, BOOL act)
{
	CCharaInfoContext* _Chara = g_pFramework->GetCharaInfoContext();
	if( idx == -1 || idx >= _Chara->GetHasCharacterCount())
	{
		return;
	}
	for(INT32 i=0; i < MAX_CHARA_SLOT; i++)
		m_SlotSet[i]->OnUnselect();
	m_CurrentSlot = idx + m_pScrollBar->getScrollPos();
	if(m_CurrentSlot < 0)
		m_CurrentSlot = 0;
	m_SlotSet[idx]->OnSelect();
	SetDetailInfo(m_CurrentSlot);

	if(act)
		PushSetButton();
}

INT32 UITabProfileCharacter::FindSlot(LuaState * L)
{
	for( INT32 i = 0; i < MAX_CHARA_SLOT; i++)
	{
		if( m_SlotSet[i])
		{
			i3UIScene * pScene = m_SlotSet[i]->GetScene(0);
			I3ASSERT( pScene);
			if( pScene->getLuaContext() == L)
				return i;
		}
	}
	return -1;
}

void UITabProfileCharacter::InitText()
{
	LuaState * L = _CallLuaFunction("InitText");
	i3Lua::PushA2UTF8String(L, GAME_STRING("STR_SKILL_TAB_CHARA_LABEL2"));
	_EndLuaFunction(L, 1);
}

void UITabProfileCharacter::InitState()
{
	InitText();
	for(INT32 i=0; i<MAX_SKILL_TYPE; i++)
		DefaultSlot(i, FALSE);
}

void UITabProfileCharacter::SetCharaName(INT32 idx)
{
	CCharaInfoContext * CChara = g_pFramework->GetCharaInfoContext();
	char _Nick[MAX_STRING_COUNT];
	sprintf_s(_Nick, GAME_STRING("STR_SKILL_TAB_CHARA_LABEL1"), CChara->GetNickName(idx));

	LuaState * L = _CallLuaFunction("SetCharaName");
	i3Lua::PushA2UTF8String(L, _Nick);
	_EndLuaFunction(L,1);
}

void UITabProfileCharacter::SetDetailInfo(INT32 idx)
{
	SkillManager * skillMng = g_pGameContext->getSkillManager();

	char _skillName[MAX_STRING_COUNT] = {0,};
	char _skillTemp[MAX_STRING_COUNT] = {0,};
	char _playPoint[MAX_STRING_COUNT] = {0,};

	SetCharaName(idx);
	CCharaInfoContext * CChara = g_pFramework->GetCharaInfoContext();
	INT32 skillIdx = CChara->GetCharaInfo(idx)->getBasicInfo()->m_ui8CharaSlotIdx;

	for(INT32 i=0; i < MAX_SKILL_TYPE; i++)
	{
		if(skillMng->HasType(skillIdx,i) == FALSE)
		{
			DefaultSlot(i,FALSE);
			continue;
		}
		DefaultSlot(i,TRUE);

		INT32 _Img = skillMng->getSkillLastImageIndex(skillIdx,i);
		float _Pt = (float)(skillMng->getSkillPlayTime(skillIdx,i));
		UINT16 _Point = skillMng->getSkillPlayPoint(skillIdx,i);
		INT32 _Slot = skillMng->GetLastSlot(skillIdx, i);

		sprintf_s(_playPoint, "%d", _Point);

		if(_Slot >= 0)
			sprintf_s(_skillTemp, "%s%d", "No.", _Slot + 1);
		else
			sprintf_s(_skillTemp, "%s", GAME_STRING("STR_SKILL_TAB_CHARACTER_NO_SELECT"));
		sprintf_s(_skillName, "%s - %s", skillMng->getClassName(skillMng->getCurrentClass(skillIdx ,i)), _skillTemp);

		LuaState * L = _CallLuaFunction("SetDetailInfo");
		i3Lua::PushInteger(L, i);
		i3Lua::PushInteger(L, _Img);
		i3Lua::PushNumber(L, _Pt/3600);
		i3Lua::PushA2UTF8String(L, _playPoint);
		i3Lua::PushA2UTF8String(L, _skillName);
		
		_EndLuaFunction(L, 5);
	}
}

void UITabProfileCharacter::DefaultSlot(INT32 idx, BOOL Sig)
{
	LuaState * L = _CallLuaFunction("SetFrame");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushBoolean(L, Sig);
	_EndLuaFunction(L, 2);
}

void	UITabProfileCharacter::UpdateWheel( int nDelta )
{
	if( m_pScrollBar )
	{
		int nTerm	= 0 < nDelta ? -1 : 1;
		int nPos	= m_pScrollBar->getScrollPos() + nTerm * m_pScrollBar->getTickSize();
		m_pScrollBar->setScrollPos( nPos );
	}

	UpdateScroll();
}

void	UITabProfileCharacter::UpdateScroll()
{
	UpdateSlotList();
}

#endif