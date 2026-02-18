#include "pch.h"

#include "UITabProfileSkill.h"
#include "UIMainFrame.h"
#include "UIPhaseProfile.h"
#include "UICharaInfoMgr.h"
#include "CommunityContext.h"
#include "UIUtil.h"
#include "UIMath.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"

static UITabProfileSkill* GetThis( void)
{
	UIPhaseProfile* pProfile = static_cast<UIPhaseProfile*>(GameUI::GetPopup( UPW_PROFILE));
	if( pProfile != nullptr)
		return static_cast<UITabProfileSkill*>(pProfile->GetTab( UIPhaseProfile::TAB_SKILL ));
	return nullptr;
}

extern "C" {
	//케릭터 리스트박스 이벤트
	int PhaseProfileSkill_ChangeChara( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1 );
		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SetLuaCharaSelect(idx);
		return 0;
	}
	//카테고리 클릭이벤트
	int PhaseProfileSkill_CategoryClick( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1 );
		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SetCurrentCategory(idx);
		return 0;
	}
	//슬롯 스크롤 이벤트
	int PhaseProfileSkill_SkillScrool( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SetCurrentScroll(idx);
		return 0;
	}
	int PhaseProfileSkill_SlotWheel( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SlotWheel(idx);
		return 0;
	}
	int PhaseProfileSkill_SelectSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SlotNewCheck(idx);
		return 0;
	}
	int PhaseProfileSkill_ApplySlot( LuaState * L)
	{
		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->ApplySlot();
		return 0;
	}
	int PhaseProfileSkill_ApplySlot_DoubleClick( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->ApplySlot_DoubleClick(idx);
		return 0;
	}
	int PhaseProfileSkill_CancelSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->CancelSlot(idx);
		return 0;
	}
	int PhaseProfileSkill_SelectedLevel( LuaState * L)
	{
		INT32 level		= i3Lua::GetIntegerArg( L, 1 );

		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SelectedLevel(level);

		return 0;
	}
	int PhaseProfileSkill_SelectClass( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SelectClass(idx);
		return 0;
	}
	int PhaseProfileSkill_ApplyClass( LuaState * L)
	{
		UITabProfileSkill * pTab = GetThis();
		if( pTab != nullptr)
			pTab->ApplyClass();
		return 0;
	}
}

LuaFuncReg UITabProfileSkill_Glue[] =
{
	{"ChangeChara",		PhaseProfileSkill_ChangeChara	},
	{"CategoryClick",	PhaseProfileSkill_CategoryClick	},
	{"SkillScrool",		PhaseProfileSkill_SkillScrool	},
	{"SlotWheel",		PhaseProfileSkill_SlotWheel	},
	{"SelectSlot",		PhaseProfileSkill_SelectSlot	},
	{"ApplySlot",		PhaseProfileSkill_ApplySlot		},
	{"ApplySlot_DoubleClick", PhaseProfileSkill_ApplySlot_DoubleClick },
	{"CancelSlot",		PhaseProfileSkill_CancelSlot	},
	{"SelectedLevel",	PhaseProfileSkill_SelectedLevel	},
	{"SelectClass",		PhaseProfileSkill_SelectClass	},
	{"ApplyClass",		PhaseProfileSkill_ApplyClass	},
	{nullptr, nullptr}
};


I3_CLASS_INSTANCE( UITabProfileSkill);//, UITabProfileBase );

UITabProfileSkill::UITabProfileSkill()
{
	m_iRank = 0;
	m_CurrentAvatarSlot = 0;
	m_iCurrentCategory = 0;
	m_iCurrentIndex = 0;
	m_iCurrentScroll = 0;
	m_iCharaCount = 0;
	m_pSlotScrollbar = nullptr;
	m_pLevelImageSet = nullptr;
	m_pLevelProgressBar = nullptr;
	m_pLevelProgressLine = nullptr;
	m_pCurLevelButton = nullptr;
	m_biscancel = false;
	m_iCharaClass = CHARA_CLASS_ASSAULT;
	m_iCurSlotMaxLevel = 10;
	m_fMaxLevel = 0.f;
	m_fCurLevel = 0.f;
	m_fMotifyLevel = 0.f;
	m_fProgressTimer = 0.f;
	m_fSpeedRate = 1.0f;
	m_iOriginLevel = 0;
	m_iLevelButtonCount = 10;

	for(INT32 i = 0; i < CATEGORYLIST_MAX; i++)
		m_iFocusSlot[i] = -1;
}

UITabProfileSkill::~UITabProfileSkill()
{
	m_SlotChara.clear();
	m_SlotSkill[0].clear();
	m_SlotSkill[1].clear();
	m_SlotSkill[2].clear();
}

void	UITabProfileSkill::OnCreate( i3GameNode * pParent )
{
	UITabProfileBase::OnCreate( pParent );
	AddScene( "Scene/Main/PointBlankRe_Profile_Skill.i3UIs", UITabProfileSkill_Glue );
}

void	UITabProfileSkill::_InitializeAtLoad( i3UIScene * pScene)
{
	UITabProfileBase::_InitializeAtLoad( pScene);
	m_pSlotScrollbar	= (i3UIScrollBar*) pScene->FindChildByName( "i3UIScrollBar" );
	I3ASSERT( m_pSlotScrollbar);

	m_pLevelImageSet = (i3UIButtonImageSet *) pScene->FindChildByName( "SkillLevelImage");
	m_pLevelProgressBar = (i3UIProgressBar*) pScene->FindChildByName("SkillLevelProgress");
	m_pLevelProgressLine = (i3UIImageBox*) pScene->FindChildByName("i3UIImageBox5");
	m_pCurLevelButton = (i3UIButton*) pScene->FindChildByName("i3UIButton11");

	//m_FlowEditBox.SetFlowEditBox(static_cast<i3UIEditBox*>(pScene->FindChildByName("text_stat_Exp")), 2.0f, 0.25f);
}

void	UITabProfileSkill::_ClearAtUnload( i3UIScene * pScene)
{
	UITabProfileBase::_ClearAtUnload( pScene);
	m_pLevelImageSet = nullptr;
	m_pLevelProgressBar = nullptr;
	m_pLevelProgressLine = nullptr;
	m_pCurLevelButton = nullptr;
	//m_FlowEditBox.Reset();
}

bool	UITabProfileSkill::OnEntranceStart( void * pArg1, void * pArg2 )
{
	//2014-08-21 캐릭터 정보에서 지워진 슬롯의 아이디를 가질 경우가 있어 수정 //ENG_김대영.
	m_CurrentAvatarSlot =  CCharaInfoContext::i()->GetMainCharaIdx();

	// Skill 시작 타임에 제한이 풀려있으면 Level 제한을 풉니다.
	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_RESTRICT_SKILL) == false)
		g_pSkillSystemValueSet->RestrictSkill();

	m_iOriginLevel = 0;

	if( false == UITabProfileBase::OnEntranceStart( pArg1, pArg2 ) )
	{
		return false;
	}

	char strTemplate[MAX_STRING_COUNT] = {0,};
	for(INT32 i = 0; i < MAX_SKILLSLOT; i++)
	{
		i3::snprintf( strTemplate, MAX_STRING_COUNT, "chara%d_Img",i); 
		m_apImgSetCtrl[i] = (i3UIButtonImageSet*) GetScene(0)->FindChildByName( strTemplate);
	}
	m_SelectImgSetCtrl = (i3UIButtonImageSet*) GetScene(0)->FindChildByName( "i3UIButtonImageSet1");

	m_fMaxLevel = m_pLevelImageSet->getSize()->x;
	InitLevelProgressBar();

	//기본정보를 세팅합니다.
	LuaState * L = _CallLuaFunction( "__init");
	if( L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}

	L = _CallLuaFunction("InitSkillLevel");
	if (L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}
	_SetSkillSlotData();
	SetCurrentCategory(0);
	SelectSlot(0);

	return true;
}

void	UITabProfileSkill::OnEntranceEnd()
{
	UITabProfileBase::OnEntranceEnd();

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}

bool	UITabProfileSkill::OnExitStart()
{

	if( false == UITabProfileBase::OnExitStart() )
	{
		return false;
	}

	// 스킬 정보창에 새로운 정보를 이 창을 닫을 때 한 번 해줍니다.
	// 그래야 실시간으로 변동을 확인 할 수 있습니다. 2014-12-22(jinsik.kim)
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SKILL_EQUIP_CHANGE);

	return true;
}

void	UITabProfileSkill::SetInputDisable(bool bDisable)
{
	UITabProfileBase::SetInputDisable(bDisable);

}

void	UITabProfileSkill::OnUpdate( REAL32 rDeltaSeconds)
{
	UITabProfileBase::OnUpdate(rDeltaSeconds);

	LevelProgressBarUpdate(rDeltaSeconds);

	//m_FlowEditBox.OnUpdate(rDeltaSeconds);
}

void	UITabProfileSkill::LevelProgressBarUpdate( REAL32 rDeltaSeconds)
{
	m_fProgressTimer += rDeltaSeconds;

	if (m_fProgressTimer > 0.01f)
	{
		if (i3Math::abs(m_fCurLevel - m_fMotifyLevel) < m_fSpeedRate)
		{
			m_fCurLevel = m_fMotifyLevel;
		}

		if (m_fMotifyLevel > m_fCurLevel)
		{
			m_fCurLevel += m_fSpeedRate;
		}
		else if (m_fMotifyLevel < m_fCurLevel)
		{
			m_fCurLevel -= m_fSpeedRate;
		}

		REAL32 value = 0.f;

		if (m_pLevelImageSet != nullptr)
		{
			i3::pack::RECT rect;
			VEC2D* size = m_pLevelImageSet->getSize();
			VEC3D* linePos = m_pLevelProgressLine->getPos();
			size->x = m_fCurLevel;
			m_pLevelImageSet->getShape(m_pLevelImageSet->GetShapeIdx())->setCoordW(m_fCurLevel);
			m_pLevelImageSet->setSize(size);
			m_pLevelImageSet->getRect(&rect);
			linePos->x = rect.right - 23.1f;

			m_pLevelProgressLine->setPos(linePos);

			if( m_iCurSlotMaxLevel == 3)
				value = size->x/(m_fMaxLevel-40.0f);
			else
				value = size->x/m_fMaxLevel;
		}

		if (m_pLevelProgressBar != nullptr)
		{
			m_pLevelProgressBar->setProgress(value);
		}

		m_fProgressTimer = 0;
	}

}

void	UITabProfileSkill::InitLevelProgressBar()
{
	VEC2D* size = m_pLevelImageSet->getSize();
	size->x = 0.f;
	m_pLevelImageSet->getShape(m_pLevelImageSet->GetShapeIdx())->setCoordW(0.f);
	m_pLevelImageSet->setSize(size);
	m_pLevelProgressBar->setProgress(0.f);
}

void	UITabProfileSkill::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	switch(event)
	{
	case EVENT_SKILL_SLOT_LEARN:
		if( arg == EVENT_ERROR_SUCCESS)
		{
			if(m_biscancel == true)
			{
				m_biscancel = false;
			}

			CHARA_INFO* charainfo = const_cast<CHARA_INFO*>(CCharaInfoContext::i()->GetCharaInfo(m_CurrentAvatarSlot));

			*charainfo->getSkillInfo() = m_Classtype;
		}
		else
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SKILL_ERROR_EQUIPFAIL"));


		_SetSkillActivate();
		_SetSkillSlotData();
		break;
	case EVENT_SKILL_CLASS_SET:
		GetMainFrame()->CloseAwayMessageBox();
		if( arg == EVENT_ERROR_SUCCESS)
		{
			CHARA_INFO* charainfo = const_cast<CHARA_INFO*>(CCharaInfoContext::i()->GetCharaInfo(m_CurrentAvatarSlot));
		
			*charainfo->getSkillInfo() = m_Classtype;
			
			CCharaInfoContext::i()->SetAllCharaData();			
			SetLuaApplyClass(false);
			SetLuaCharaInfo();
			_SetSkillSlotData();
			SelectSlot(0);
		}
		else
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SKILL_ERROR_CLASSSFAIL"));

	default:
		break;
	}
}

bool	UITabProfileSkill::RequestChangeTab()
{
	return UITabProfileBase::RequestChangeTab();
}

bool	UITabProfileSkill::RequestExitTab()
{
	return true;
}

//UTIL 
void	UITabProfileSkill::_SetSkillSlotData()
{
	//스킬 리스트를 컨테이너에 저장
	SetAllSlotData();
	
	//스킬 리스트에서 장착스킬 활성화
	_SetSkillActivate();

	//캐릭터 슬롯 UI세팅
	SetLuaCharaInfo();

	//장착스킬 UI 세팅
	SetLuaSkillApplyInfo();

	//스킬 UI 세팅
	if( m_Classtype.m_PrimarySkill == CHARA_CLASS_NONE )
	{
		SelectClass(0);
		SetLuaApplyClass(true);
	}
	else
	{
		SetLuaApplyClass(false);	
		SetLuaSkillSlotInfo();
	}

	return;
}

#include "UserInfoContext.h"
// 서버에서 받은 스킬정보 세팅및 PEF에서 창착가능한스킬슬롯 세팅
void	UITabProfileSkill::SetAllSlotData(void)
{
	//캐릭터정보
	m_iCharaCount = CCharaInfoContext::i()->GetHasCharacterCount();				//캐릭터 카운터

	//스킬정보
	const CHARA_INFO*  charainfo = CCharaInfoContext::i()->GetCharaInfo(m_CurrentAvatarSlot);
	if( charainfo)
	{
		m_Classtype = charainfo->m_CharaSkillInfo;	//스킬 정보 받아오기
	}

	NCHARA_SKILL * pSkillSet = nullptr;
	INT32 iskillsize[3];
	INT32 _level = 0;
	REAL32 rPlayTime = 0.f;

	//유저 정보 계급등
	USER_INFO_BASIC infoBasic;											//유저 정보 계급 경험치등
	UserInfoContext::i()->GetMyUserInfoBasic( &infoBasic );

	//캐릭터 기본 정보 저장합니다. 
	m_SlotChara.clear();
	for(INT32 i = 0; i< m_iCharaCount; i++)
	{
		CHARASLOTDATA charadata;
		charainfo = CCharaInfoContext::i()->GetCharaInfo(i);

		charadata.wstrName = charainfo->m_CharaBasicInfo.m_strCharaName;

		// vv3 - check
		charadata.iCharaType = GameUI::GetCharaImageIndex( charainfo->getPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));
		m_SlotChara.push_back(charadata);

	}

	//데이터 기본 정보 저장합니다,
	charainfo = CCharaInfoContext::i()->GetCharaInfo(m_CurrentAvatarSlot);
	if( charainfo)
	{
		rPlayTime = charainfo->m_ui32SkillRelTime / (REAL32)3600;			//플레이시간
		m_iRank = infoBasic.m_ui32Rank;													//계급
	}
	

	iskillsize[0] = MAX_COMMON_SKILL;
	iskillsize[1] = MAX_MAIN_SKILL;
	iskillsize[2] = MAX_ASSIST_SKILL;

	for(INT i = 0; i < CATEGORYLIST_MAX; i++)
	{
		m_SlotSkill[i].clear();
		for( INT32 j = 0; j < iskillsize[i]; j++)
		{
			SKILLSLOTDATA tempData;
			tempData.rPlayTime = rPlayTime;
			pSkillSet = g_pSkillSystemValueSet->getSkillsystemSet( (SKILL_CATEGORYLIST)i, j);

			if(pSkillSet == nullptr)
				continue;
			
			//상승조건
			if( pSkillSet->getApplyCategory() == CATEGORYLIST_UNKNOWN)
				continue;

			if(i == CATEGORYLIST_MAIN)
			{
				if(pSkillSet->isWeaponType( m_Classtype.m_PrimarySkill) == false)
					continue;
			}

			tempData.iCurrentRank = m_iRank;
			tempData.iCurrentExp = infoBasic.m_ui32Exp;
			tempData.NextType = pSkillSet->getApplyRequisiteType();
			if( tempData.NextType == REQUISTE_CLASS)
			{
				_level =pSkillSet->getApplyRequisiteLevel(m_iRank);
			}
			else
			{
				_level =pSkillSet->getApplyRequisiteLevel(static_cast<INT32>(tempData.rPlayTime));
			}

			tempData.SkillType = pSkillSet->getApplySkill();
			tempData.iSkillIndex = j;
			tempData.iLevel = _level;
			tempData.iMaxLevel = _level;
			tempData.SkillState = SKILL_STATE_OFF;
			tempData.bFocus = false;

			
			const CCharaInfoContext::STCLevel * temp = CCharaInfoContext::i()->GetCheckSkillLevel(m_CurrentAvatarSlot);
			INT32 _size = m_SlotSkill[i].size();
			const INT32 *ilevel = &temp->SlotLevel[i].at(_size);
			if( *ilevel < tempData.iMaxLevel)
			{
				tempData.bNew = true;
			}


			if(_level < 0 )
			{
				tempData.iNextRank = pSkillSet->getApplyRequisiteValue(1);
				tempData.iNextPlaytime = static_cast<REAL32>(pSkillSet->getApplyRequisiteValue(1));
				tempData.iCurrentPlaytime =0;
			}
			else if(_level < 10 )
			{
				tempData.iNextRank = pSkillSet->getApplyRequisiteValue(_level+1);
				tempData.iNextPlaytime = static_cast<REAL32>(pSkillSet->getApplyRequisiteValue(_level+1));
				tempData.iCurrentPlaytime = static_cast<REAL32>(pSkillSet->getApplyRequisiteValue(_level));
			}
			else
			{
				tempData.iNextPlaytime = -1;
				tempData.iNextRank = -1;
				tempData.iCurrentPlaytime =0;
			}
			
			if(_level > -1)
				tempData.SkillState = SKILL_STATE_ON;

			if((m_iFocusSlot[i] > -1) && (m_iFocusSlot[i] == j ))
				tempData.bFocus = true;

			m_SlotSkill[i].push_back(tempData);

		}
	}
	
}

//스킬의 장착가능 레벨별 스킬을 활성화
void	UITabProfileSkill::_SetSkillActivate(void)
{
	i3::vector<SKILLSLOTDATA>::iterator itr_Info;
	INT32 iSkillSize = 0;
	INT32 iskillIndex[3];

	iskillIndex[0] = (INT32)m_Classtype.m_CommonSkill;
	iskillIndex[1] = (INT32)m_Classtype.m_MainSkill;
	iskillIndex[2] = (INT32)m_Classtype.m_AssistSkill;

	for(INT32 j = 0; j < CATEGORYLIST_MAX; j++)
	{
		itr_Info = m_SlotSkill[j].begin();
		iSkillSize = m_SlotSkill[j].size();

		for(INT32 i = 0; i < iSkillSize; i++)
		{
			if( itr_Info->iSkillIndex ==iskillIndex[j])
			{
				switch(j)
				{
					case	0:
						if( m_Classtype.m_ui8CommonSkillLv < 1)
							continue;
						itr_Info->iLevel = m_Classtype.m_ui8CommonSkillLv;
						break;
					case	1:
						if( m_Classtype.m_ui8MainSkillLv < 1)
							continue;
						itr_Info->iLevel = m_Classtype.m_ui8MainSkillLv;
						break;
					case	2:
						if( m_Classtype.m_ui8AssistSkillLv < 1)
							continue;
						itr_Info->iLevel = m_Classtype.m_ui8AssistSkillLv;
						break;
				}

				itr_Info->bApply = true;
				itr_Info->SkillState = SKILL_STATE_SELECT;
				m_SelectSkill[j] = *itr_Info;
			}
			else
			{
				itr_Info->bApply = false;
				itr_Info++;
			}
		}
	}

}

//LUA 관련 유틸함수 
//케릭터 선택이벤트
void	UITabProfileSkill::SetLuaCharaSelect(INT32 idx)
{

	m_CurrentAvatarSlot = idx;
	SetCurrentCategory(0);

}
//캐릭터 관련 LUA 세팅
void	UITabProfileSkill::SetLuaCharaInfo(void)
{
	i3::stack_wstring wstrCharname;
	i3::vector<CHARASLOTDATA>::iterator itr_Info  = m_SlotChara.begin();

	for(UINT32 i = 0; i < m_SlotChara.size(); i++)
	{
		i3::generic_string_cat(wstrCharname, itr_Info->wstrName);
		i3::generic_string_cat(wstrCharname, L";");
		itr_Info++;
	}

	LuaState * L = _CallLuaFunction( "SetDataChara");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L, m_CurrentAvatarSlot);
		i3Lua::PushInteger(L, m_SlotChara[m_CurrentAvatarSlot].iCharaType);
		i3Lua::PushStringUTF16To8(L, wstrCharname);
		i3Lua::PushInteger(L, m_Classtype.m_PrimarySkill);

		CharaInfoMgr* chara_mgr = CharaInfoMgr::instance();
		const ITEM_INFO & item_info = chara_mgr->find(m_CurrentAvatarSlot, item_def::SCTG_WEAPON, item_def::WT_PRIMARY);
		CWeaponInfo* pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( item_info.m_TItemID );

		i3::stack_wstring stack_wstr;

		// Damage
		INT32 damage = 0;
		if( pWeaponInfo != nullptr)
			damage = pWeaponInfo->GetDamage();

		i3::itoa( damage, stack_wstr);

		i3Lua::PushStringUTF16To8(L, stack_wstr);

		REAL32 ability_value[ EQUIP::eABILITY_COUNT ];
		memset(ability_value, 0, sizeof(REAL32) * EQUIP::eABILITY_COUNT ); 
		for(size_t i = item_def::PT_HEADGEAR; i<item_def::PT_MAX; ++i)
		{
			const ITEM_INFO & info = chara_mgr->find(m_CurrentAvatarSlot, item_def::SCTG_PARTS, i);
			if( info.m_TItemID == 0 ) continue;

			const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo(info.m_TItemID);
			if(pInfo == nullptr)
				continue;
			for(size_t j = EQUIP::eABILITY_DAMAGE; j<EQUIP::eABILITY_COUNT; ++j)
			{
				ability_value[j] += pInfo->GetAbilityValue( (EQUIP::eABILITY)j );
			}
		}

		REAL32 defence = ability_value[EQUIP::eABILITY_DEFENSE_BODY] + ability_value[EQUIP::eABILITY_DEFENSE_ARM] + ability_value[EQUIP::eABILITY_DEFENSE_LEG];
		defence = (defence == 0.f) ? 100.f : (defence * 100.f) + 100.f;

		i3::ftoa( defence, stack_wstr, 1);

		i3Lua::PushStringUTF16To8(L, stack_wstr);

		// vv3 - check
		const CHARA_PARTS_INFO* parts = CCharaInfoContext::i()->GetCharaPartsInfo( m_CurrentAvatarSlot );
		T_ItemID chara_id = parts->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

		const CCharaInfo * pCharInfo = g_pCharaInfoDataBase->GetCharaDBInfo( chara_id);

		//movement
		REAL32 movement = ( pCharInfo->GetMoveSpeed(MST_RUN, MDT_FRONT) + ability_value[EQUIP::eABILITY_MOVESPEED_FRONT]);
		if( pWeaponInfo != nullptr)
			movement = movement * pWeaponInfo->GetMoveNormalStand() * 0.1f;

		i3::ftoa( movement, stack_wstr, 1);

		i3Lua::PushStringUTF16To8(L, stack_wstr);

		_EndLuaFunction(L, 7);
	}
}

//스킬 CLASS LUA 세팅
void	UITabProfileSkill::SetLuaApplyClass(bool bEnable)
{
	LuaState * L = _CallLuaFunction( "SetClassApply");
	if( L != nullptr)
	{
		i3Lua::PushBoolean(L, bEnable);
		_EndLuaFunction(L, 1);
	}
}

//스킬슬롯의 데이타 LUA 세팅
void	UITabProfileSkill::SetLuaSkillSlotInfo(void)
{
	i3::vector<SKILLSLOTDATA>::iterator itr_Info; 

	char strTemplate[MAX_STRING_COUNT] = {0,};
	char strTemplate2[MAX_STRING_COUNT] = {0,};

	i3::snprintf( strTemplate, MAX_STRING_COUNT, "Skill_Icon%d",m_iCurrentCategory+1); 

	// 모든 슬롯을 열어준다
	for (UINT32 i = 0; i < MAX_SKILLSLOT; i++)
	{
		LuaState * L = _CallLuaFunction( "OpenSkillSlot");
		if( L != nullptr)
		{
			i3Lua::PushInteger(L, i+1);
			_EndLuaFunction(L, 1);
		}
	}

	for(UINT32 i = 0; i < MAX_SKILLSLOT; i++)
	{
		if((m_SlotSkill[m_iCurrentCategory].size() <= i))
		{
			LuaState * L = _CallLuaFunction( "CloseSkillSlot");
			if( L != nullptr)
			{
				i3Lua::PushInteger(L, i+1);
				_EndLuaFunction(L, 1);
			}
			continue;
		}

		i3::stack_wstring wstrLevelList;

		i3::rc_wstring wstrSkillInfo;
		REAL32 rAlpha = 0.f;

		itr_Info  = m_SlotSkill[m_iCurrentCategory].begin()+m_iCurrentScroll+i;

		((i3UIControl *)m_apImgSetCtrl[i])->SetTemplateByName(strTemplate);
	
		switch(itr_Info->SkillState)
		{
		case SKILL_STATE_SELECT:
		case SKILL_STATE_ON:
			rAlpha = 1.f;
			break;
		case SKILL_STATE_OFF:
			rAlpha = 0.5f;
			break;
		}

		COLOR* color = m_apImgSetCtrl[i]->getColor();
		color->a = (UINT8)(255 * rAlpha);
		m_apImgSetCtrl[i]->setColor( color );

		if( itr_Info->SkillState != SKILL_STATE_OFF)
		{
			for(INT32 j = 1; j <= itr_Info->iMaxLevel; j++)
			{
				strTemplate2[0]= 0;
				i3::snprintf(strTemplate2, MAX_STRING_COUNT, "STR_SKILL_LEVEL%d", j);
				i3::generic_string_cat(wstrLevelList, GAME_RCSTRING(strTemplate2));
				i3::generic_string_cat(wstrLevelList, L";");
			}
		}

		i3::rc_wstring wstrSkillName;
		GetSkillNameTypeStr(m_iCurrentCategory, itr_Info->iSkillIndex, wstrSkillName);

		INT32 nSkillLevel = itr_Info->iLevel;
		if(nSkillLevel < 0)
			nSkillLevel = 0;

		i3::snprintf(strTemplate2, MAX_STRING_COUNT, "Lv. %d\n", nSkillLevel);
		i3::generic_string_cat(wstrSkillInfo, GAME_RCSTRING(strTemplate2));
		i3::generic_string_cat(wstrSkillInfo, wstrSkillName);

		LuaState * L = _CallLuaFunction( "SetDataSlot");
		if( L != nullptr)
		{
			i3Lua::PushInteger(L, i+1);
			i3Lua::PushInteger(L, itr_Info->SkillState);			//스킬 상태값
			i3Lua::PushInteger(L, itr_Info->iSkillIndex);			//스킬인덱스(타입)
			i3Lua::PushStringUTF16To8(L, wstrSkillInfo);				//스킬 설명 String
			i3Lua::PushStringUTF16To8(L, wstrLevelList);				//레벨 리스트 String

			i3Lua::PushBoolean(L, itr_Info->bApply);				//스킬인덱스(타입)
			i3Lua::PushInteger(L, itr_Info->iLevel-1);				//스킬 레벨(타입)
			i3Lua::PushBoolean(L, itr_Info->bFocus);				//스킬인덱스(타입)
			i3Lua::PushBoolean(L, itr_Info->bNew);					//스킬 뉴 상태
			_EndLuaFunction(L, 9);
		}
	}

	bool iTabNew[3];
	
	for(INT32 i = 0; i < CATEGORYLIST_MAX; i++)
	{
		iTabNew[i] = false;
		for( UINT32 j=0; j < m_SlotSkill[i].size(); j++)
		{
			if( m_SlotSkill[i].at(j).bNew == true)
			{
				iTabNew[i] = true;
			}
		}
		
		LuaState * L = _CallLuaFunction( "SetTabNew");
		if( L != nullptr)
		{
			i3Lua::PushInteger(L, i+1);
			i3Lua::PushBoolean(L, iTabNew[i]);
			_EndLuaFunction(L, 2);
		}
	}
	

}

//현재 장착중인 스킬 LUA 세팅
void	UITabProfileSkill::SetLuaSkillApplyInfo(void)
{
	INT32 iskillIndex[CATEGORYLIST_MAX];
	INT32 iskillLv[CATEGORYLIST_MAX];
	i3::rc_wstring wstr_TempStrSkill;
	i3::stack_wstring wstrSkillInfo;
	i3::stack_wstring wstrSkillLevel;
	char strTemplate2[MAX_STRING_COUNT];
	
	iskillIndex[0] = (INT32)m_Classtype.m_CommonSkill;
	iskillIndex[1] = (INT32)m_Classtype.m_MainSkill;
	iskillIndex[2] = (INT32)m_Classtype.m_AssistSkill;

	iskillLv[0] = (INT32)m_Classtype.m_ui8CommonSkillLv;
	iskillLv[1] = (INT32)m_Classtype.m_ui8MainSkillLv;
	iskillLv[2] = (INT32)m_Classtype.m_ui8AssistSkillLv;

	for(INT32 i = 0; i < CATEGORYLIST_MAX; i++)
	{
		wstrSkillInfo.clear();
		wstrSkillLevel.clear();
		if( iskillLv[i] < 1 )
			iskillIndex[i]= -1;
		GetSkillNameTypeStr(i, m_SelectSkill[i].iSkillIndex, wstr_TempStrSkill);
		i3::snprintf(strTemplate2, MAX_STRING_COUNT, "STR_SKILL_LEVEL%d", iskillLv[i]);
		i3::generic_string_cat(wstrSkillInfo, wstr_TempStrSkill);
		i3::generic_string_cat(wstrSkillInfo, L" + ");
		i3::generic_string_cat(wstrSkillInfo, GAME_RCSTRING(strTemplate2));

		i3::snprintf(strTemplate2, MAX_STRING_COUNT, "Lv. %d", iskillLv[i]);
		i3::generic_string_cat(wstrSkillLevel, GAME_RCSTRING(strTemplate2));

		LuaState * L = _CallLuaFunction( "SetApplyInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger(L, i+1);
			i3Lua::PushInteger(L, iskillIndex[i]);			//스킬 상태값
			i3Lua::PushStringUTF16To8(L, wstrSkillInfo);				//스킬 설명 String
			i3Lua::PushStringUTF16To8(L, wstrSkillLevel);
			_EndLuaFunction(L, 4);
		}
	}

}

void UITabProfileSkill::SetSkillDesc(INT32 idx, REAL32 & outVal)
{
	if( (size_t)idx >= m_SlotSkill[m_iCurrentCategory].size()) return;

	const i3::vector<SKILLSLOTDATA>::iterator iter = m_SlotSkill[m_iCurrentCategory].begin() + idx;

	i3::rc_wstring	wstrSkillInfo;
	i3::rc_wstring    wstrDesc;


	outVal = -1.0f;

//	GetSkillNameTypeStr(m_iCurrentCategory, iter->iSkillIndex, wstrSkillInfo);

	if( iter->NextType == REQUISTE_TIME)
	{
		if((REAL32)iter->iNextPlaytime > iter->rPlayTime)
			outVal = (REAL32)(iter->rPlayTime - iter->iCurrentPlaytime)/(REAL32)(iter->iNextPlaytime - iter->iCurrentPlaytime);

		if( outVal >= 0)
		{
			GetSkillTypeDescripStr(iter->iSkillIndex+1, wstrDesc);
			i3::sprintf( wstrSkillInfo, wstrDesc,iter->iNextPlaytime); 
		}
		else
		{
			GetSkillTypeDescripStr(iter->iSkillIndex+1, wstrDesc, true);
			wstrSkillInfo = wstrDesc; 
		}
	}
	else
	{
		if((REAL32)iter->iNextRank > iter->iCurrentRank)
		{
			INT32 rank = UserInfoContext::i()->GetMaxExpByRank(iter->iNextRank);
			if( rank > 0)
				outVal = (REAL32)iter->iCurrentExp / (REAL32)rank;
		}

		if( outVal >= 0 )
		{
			GetSkillTypeDescripStr(iter->iSkillIndex+1, wstrDesc);
			i3::sprintf( wstrSkillInfo, wstrDesc, GetCharaRank(iter->iNextRank)); 
		}
		else
		{
			GetSkillTypeDescripStr(iter->iSkillIndex+1, wstrDesc, true);
			wstrSkillInfo = wstrDesc;
		}
	}

	// 스킬 템플릿 교체
	char strTemplate[MAX_STRING_COUNT] = {0,};
	i3::snprintf( strTemplate, MAX_STRING_COUNT, "Skill_Icon%d", m_iCurrentCategory+1); 
	((i3UIControl *)m_SelectImgSetCtrl)->SetTemplateByName(strTemplate);

	LuaState* L = _CallLuaFunction("SetSkillDescription");
	if (L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrSkillInfo);
		_EndLuaFunction(L, 1);
	}

	// EditBox Text 설정 및 Flow 설정
	//{
	//	m_FlowEditBox.SetFlowTextData(wstrSkillInfo);
	//	m_FlowEditBox.PlayEditTextFlow(TFD_VERTICAL, true);
	//}
}

//스킬 슬롯의 선택후 상세정보 LUA 세팅
void UITabProfileSkill::SlotNewCheck(INT32 slot)
{
	i3::vector<SKILLSLOTDATA>::iterator itr_Info; 
	INT32 iSelectSlot = m_iCurrentScroll+slot;

	itr_Info = m_SlotSkill[m_iCurrentCategory].begin()+iSelectSlot;

	CCharaInfoContext::STCLevel * temp = CCharaInfoContext::i()->SetCheckSkillLevel(m_CurrentAvatarSlot);
	INT32 *ilevel = &temp->SlotLevel[m_iCurrentCategory].at(iSelectSlot);
	if( itr_Info->bNew == true)
	{
		itr_Info->bNew = false;
		*ilevel = itr_Info->iMaxLevel;
	}
	SelectSlot(slot);
}

void UITabProfileSkill::ApplySlot_DoubleClick(INT32 slot)
{

	INT32 iSelectSlot = m_iCurrentScroll + slot;

	if (m_iFocusSlot[m_iCurrentCategory] == iSelectSlot)
		ApplySlot();
}

//스킬 슬롯의 선택후 상세정보 LUA 세팅
void UITabProfileSkill::SelectSlot(INT32 slot)
{
	i3::vector<SKILLSLOTDATA>::iterator itr_Info; 
	INT32 iSelectSlot = m_iCurrentScroll+slot;
	
	if( (size_t)iSelectSlot >= m_SlotSkill[m_iCurrentCategory].size()) return;
	
	itr_Info = m_SlotSkill[m_iCurrentCategory].begin()+iSelectSlot;
	m_iFocusSlot[m_iCurrentCategory] = iSelectSlot;

	for(INT32 i = 0; i< (INT32)m_SlotSkill[m_iCurrentCategory].size(); i++)
	{
		itr_Info = m_SlotSkill[m_iCurrentCategory].begin()+i;
		itr_Info->bFocus = false;
	}

	itr_Info = m_SlotSkill[m_iCurrentCategory].begin()+iSelectSlot;
	itr_Info->bFocus = true;

	// 공통, 주력 카테고리에 있는 스킬은 MAX 레벨이 10
	if (m_iCurrentCategory != CATEGORYLIST_ASSIST)
		m_iCurSlotMaxLevel = 10;
	// 보조 카테고리에 있는 스킬 중 경험치 증가, 포인트 증가 스킬은 MAX 레벨이 3
	// 나머지 보조 스킬은 MAX 레벨이 5이다
	else if (itr_Info->SkillType != SKILLLIST_EXP && itr_Info->SkillType != SKILLLIST_POINT)
		m_iCurSlotMaxLevel = 5;
	else
	{
		m_iCurSlotMaxLevel = 3;
		m_iLevelButtonCount = 9;
	}

	_SetCurLevelButtonPos(m_iCurSlotMaxLevel, (itr_Info->iLevel * m_iLevelButtonCount / m_iCurSlotMaxLevel));
	m_iOriginLevel = itr_Info->iLevel * m_iLevelButtonCount / m_iCurSlotMaxLevel;
	m_iLevelButtonCount = 10;

	if (m_iOriginLevel < 0)
		m_iOriginLevel = 0;

	ChangeLevel(itr_Info->iLevel);

	//장착스킬 UI 세팅
	SetLuaSkillApplyInfo();
	//캐릭터 UI 세팅
	SetLuaCharaInfo();
	//스킬 UI 세팅
	SetLuaSkillSlotInfo();

	i3::stack_wstring wstrTempExp;
	REAL32 rExpRate = -1.0f;

	// 스킬의 Description 설정
	SetSkillDesc(iSelectSlot, rExpRate);
	
	i3::sprintf( wstrTempExp, L"%d%%",(INT32)(rExpRate*100));

	bool bApplyClass = true;
	if( m_Classtype.m_PrimarySkill == CHARA_CLASS_NONE )
		bApplyClass = false;

	LuaState* L = UILua::CallLuaFunction( GetScene(0), "SetInfo");
	if(L != nullptr)
	{
		i3Lua::PushInteger(L, itr_Info->iSkillIndex);
		i3Lua::PushNumber(L, rExpRate);
		i3Lua::PushStringUTF16To8( L, wstrTempExp);
		i3Lua::PushInteger(L, m_iCurrentCategory+1);
		i3Lua::PushBoolean(L, bApplyClass);
		UILua::EndLuaFunction(L, 5);
	}	

	L = _CallLuaFunction("SetLevelButton");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, m_iCurSlotMaxLevel);
		i3Lua::PushInteger(L, itr_Info->iMaxLevel);
		i3Lua::PushInteger(L, itr_Info->iLevel);
		_EndLuaFunction(L, 3);
	}
}

//클레스미선택시 클레스 선택 이벤트
void UITabProfileSkill::SelectClass(INT32 _index)
{

	i3::rc_wstring wstrClassname;

	m_iCharaClass = (UINT8)(_index&0xFF)+1;
	GetWeaponTypeStr( (CHARA_CLASS)m_iCharaClass, wstrClassname);

	i3::stack_wstring wstrTemplate;
	i3::sprintf( wstrTemplate, GAME_RCSTRING("STR_TBL_CLASS_MAINTEXT2"), wstrClassname, wstrClassname); 

	LuaState* L = UILua::CallLuaFunction( GetScene(0), "ChangeClass");
	if(L != nullptr)
	{
		i3Lua::PushInteger(L, (UINT8)(_index&0xFF)+1);
		i3Lua::PushStringUTF16To8( L, wstrTemplate);
		UILua::EndLuaFunction(L, 2);
	}
}

void UITabProfileSkill::_SetCurLevelButtonPos(INT32 leveltype, INT32 curlevel)
{
	if ( curlevel < 0)
		curlevel = 0;

	curlevel = curlevel - m_iOriginLevel;

	VEC3D* buttonPos = m_pCurLevelButton->getPos();
	buttonPos->x = buttonPos->x + (40.f*curlevel);
	m_pCurLevelButton->setPos(buttonPos);
}

//카테고리이동 버튼클릭 이벤트
void UITabProfileSkill::SetCurrentCategory(INT32 idx)
{
	I3ASSERT_RETURN(m_pSlotScrollbar);

	i3UIManager* pMgr = i3UI::getManager();

	if( pMgr )
		pMgr->SetFocusControl( m_pSlotScrollbar );
	
	if( idx <0 || idx >= CATEGORYLIST_MAX) return;

	m_iCurrentCategory = idx; 
	// 루아 스크롤 세팅
	//REAL32 rMaxPage = (REAL32)(m_SlotSkill[m_iCurrentCategory].size()/2) + 0.5f;		// 이게 뭐하자는건지 조금 모르겠지만..일단 커버리티 지적대로 해본다..
	REAL32 rMaxPage = (REAL32)(m_SlotSkill[m_iCurrentCategory].size()) * 0.5f + 0.5f;	// 2016.07.19.수빈.
	INT32 iMaxPage = (INT32)rMaxPage;

	if (m_SlotSkill[m_iCurrentCategory].size() > MAX_SKILLSLOT)
	{
		iMaxPage = m_SlotSkill[m_iCurrentCategory].size() - 4;
	}

	if( iMaxPage > 0 && m_SlotSkill[m_iCurrentCategory].size() > MAX_SKILLSLOT)
	{
		m_pSlotScrollbar->setScrollRange(0, iMaxPage);
		m_pSlotScrollbar->EnableCtrl(true);
	}
	else
	{
		m_pSlotScrollbar->setScrollRange(0, 0);
		m_pSlotScrollbar->setScrollPos(0);
		m_pSlotScrollbar->EnableCtrl(false);
	}
	
	m_iCurrentScroll = m_pSlotScrollbar->getScrollPos();

	if (m_iCurrentScroll < 0)
		m_iCurrentScroll = 0;

	_SetSkillSlotData();
	SelectSlot(0);

}

//스킬 슬롯의 스크롤 이벤트 
void UITabProfileSkill::SetCurrentScroll(INT32 idx)
{
	m_iCurrentScroll = idx;

	if (m_iCurrentScroll < 0)
		m_iCurrentScroll = 0;
	if (m_iCurrentScroll > 0 && m_iCurrentScroll < m_pSlotScrollbar->getTickSize())
	{
		m_iCurrentScroll = m_pSlotScrollbar->getTickSize();
	}

	//장착스킬 UI 세팅
	SetLuaSkillApplyInfo();

	//캐릭터 UI 세팅
	SetLuaCharaInfo();

	//스킬 UI 세팅
	SetLuaSkillSlotInfo();

}

//스킬 슬롯의 휠 이벤트
void UITabProfileSkill::SlotWheel( int zDelta )
{
	if( m_pSlotScrollbar )
	{
		int nPos = m_pSlotScrollbar->getScrollPos() - (int)(zDelta / 120);
		INT32 slot = m_SlotSkill[m_iCurrentCategory].size() - MAX_SKILLSLOT;
		nPos = MINMAX( 0, nPos,  slot);

		m_pSlotScrollbar->setScrollPos( nPos );
	}
}

//장착스킬 창에서의 스킬 해제 이벤트
void UITabProfileSkill::CancelSlot(INT32 slot)
{
	// 병과가 없을 경우 장착 해제 버튼들의 동작을 막습니다. - sanghun.han (2014. 12. 19) 
	if( m_Classtype.m_PrimarySkill == CHARA_CLASS_NONE )
		return;

	PACKET_SKILL_SET_REQ ClassReq;

	switch(slot)
	{
	case	0:
		m_Classtype.m_CommonSkill = COMMON_SKILL_NONE;
		m_Classtype.m_ui8CommonSkillLv = 0;
		break;
	case	1:
		m_Classtype.m_MainSkill = MAIN_SKILL_NONE;
		m_Classtype.m_ui8MainSkillLv = 0;
		break;
	case	2:
		m_Classtype.m_AssistSkill = ASSIST_SKILL_NONE;
		m_Classtype.m_ui8AssistSkillLv = 0;
		break;
	default:
		CHARA_CLASS PrimarySkill = m_Classtype.m_PrimarySkill;
		m_Classtype.Reset(); 
		m_Classtype.m_PrimarySkill = PrimarySkill;
		break;
	}

	GetSendData(&ClassReq);
	GameEventSender::i()->SetEvent(EVENT_SKILL_SLOT_LEARN, &ClassReq);

	m_biscancel = true;
}



//클레스 선택 클릭 이벤트
void UITabProfileSkill::ApplyClass(void)
{

	i3::rc_wstring wstrClassname;

	GetWeaponTypeStr( (CHARA_CLASS)m_iCharaClass, wstrClassname);

	i3::rc_wstring wstrTemplate;
	i3::sprintf( wstrTemplate, GAME_RCSTRING("STR_SKILL_CLASS_SELECT"), wstrClassname); 

	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrTemplate, nullptr, MAKE_CALLBACK(CbApplyClass), this);
}

void UITabProfileSkill::CbApplyClass(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		UITabProfileSkill* profileskill = (UITabProfileSkill*)pThis;

		PACKET_SKILL_CLASS_SET_REQ Classreq;

		CHARA_INFO* pCharaInfo = const_cast<CHARA_INFO*>(CCharaInfoContext::i()->GetCharaInfo(profileskill->GetCurrentAvatar()));
		Classreq.m_ui8Slot = pCharaInfo->getBasicInfo()->m_ui8CharaSlotIdx;
		Classreq.m_ui8Class = profileskill->m_iCharaClass;

		profileskill->m_Classtype.m_PrimarySkill = (CHARA_CLASS)profileskill->m_iCharaClass;

		GameEventSender::i()->SetEvent(EVENT_SKILL_CLASS_SET, &Classreq);
		g_pFramework->GetUIMainframe()->OpenAwayMessageBox(GAME_RCSTRING("STBL_IDX_MYINFO_REST"));/*장시간 움직임이 없으므로 5초 후 로비로 이동합니다.*/
	}
	else
	{
		UITabProfileSkill* profileskill = (UITabProfileSkill*)pThis;
		profileskill->SelectClass(0);
		profileskill->SetLuaApplyClass(true);
	}
}


//스킬 슬롯창의 스킬 장착버튼 이벤트
void UITabProfileSkill::ApplySlot()
{
	PACKET_SKILL_SET_REQ ClassReq;

	INT32 slot = m_iFocusSlot[m_iCurrentCategory];

	i3::vector<SKILLSLOTDATA>::iterator itr_Info; 
	itr_Info = m_SlotSkill[m_iCurrentCategory].begin()+slot;

	if( itr_Info->SkillState == SKILL_STATE_OFF )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SKILL_ERROR_EQUIPFAIL") );
		return;
	}

	i3::stack_wstring wstrSkillLevel;
	char strTemplate[MAX_STRING_COUNT];

	i3::snprintf(strTemplate, MAX_STRING_COUNT, "Lv.");
	i3::generic_string_cat(wstrSkillLevel, GAME_RCSTRING(strTemplate));

	switch(m_iCurrentCategory)
	{
	case	0:
		m_Classtype.m_CommonSkill = (COMMON_SKILL_TYPE)itr_Info->iSkillIndex;
		m_Classtype.m_ui8CommonSkillLv = (UINT8)(itr_Info->iLevel&0xFF);
		i3::snprintf(strTemplate, MAX_STRING_COUNT, " %d", m_Classtype.m_ui8CommonSkillLv);
		break;
	case	1:
		m_Classtype.m_MainSkill = (MAIN_SKILL_TYPE)itr_Info->iSkillIndex;
		m_Classtype.m_ui8MainSkillLv = (UINT8)(itr_Info->iLevel&0xFF);
		i3::snprintf(strTemplate, MAX_STRING_COUNT, " %d", m_Classtype.m_ui8MainSkillLv);
		break;
	case	2:
		m_Classtype.m_AssistSkill = (ASSIST_SKILL_TYPE)itr_Info->iSkillIndex;
		m_Classtype.m_ui8AssistSkillLv = (UINT8)(itr_Info->iLevel&0xFF);
		i3::snprintf(strTemplate, MAX_STRING_COUNT, " %d", m_Classtype.m_ui8AssistSkillLv);
		break;
	}

	i3::generic_string_cat(wstrSkillLevel, GAME_RCSTRING(strTemplate));

	LuaState* L = _CallLuaFunction("SetApplySkillLevel");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, m_iCurrentCategory + 1);
		i3Lua::PushStringUTF16To8(L, wstrSkillLevel);
		_EndLuaFunction(L, 2);
	}

	GetSendData(&ClassReq);
	GameEventSender::i()->SetEvent(EVENT_SKILL_SLOT_LEARN, &ClassReq);

	if( m_pSlotScrollbar )
	{
		i3UIManager* pMgr = i3UI::getManager();

		if( pMgr )
			pMgr->SetFocusControl( m_pSlotScrollbar );
	}
}

void UITabProfileSkill::SelectedLevel(INT32 level)
{
	i3::vector<SKILLSLOTDATA>::iterator itr_Info; 
	itr_Info = m_SlotSkill[m_iCurrentCategory].begin()+m_iFocusSlot[m_iCurrentCategory];

	_SetCurLevelButtonPos(m_iCurSlotMaxLevel, level);
	m_iOriginLevel = level;

	if ( m_iCurSlotMaxLevel == 5)
		level = level / 2;
	else if ( m_iCurSlotMaxLevel == 3)
		level = level / 3;

	LuaState * L = _CallLuaFunction("SetLevelButton");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, m_iCurSlotMaxLevel);
		i3Lua::PushInteger(L, itr_Info->iMaxLevel);
		i3Lua::PushInteger(L, level);
		_EndLuaFunction(L, 3);
	}

	itr_Info->iLevel = level;

	ChangeLevel(level);

	SetLuaSkillSlotInfo();
}

//슬롯창의 레벨 리스트 박스 이벤트
void UITabProfileSkill::ChangeLevel(INT32 level)
{
	if (level < 0)
		level = 0;

	// 레벨에 맞는 좌표값을 계산함
	if (m_iCurSlotMaxLevel == 3)
		m_fMotifyLevel = uiMath::interpolate(((REAL32)level)/m_iCurSlotMaxLevel, 0.f, m_fMaxLevel-40.0f);
	else
		m_fMotifyLevel = uiMath::interpolate(((REAL32)level)/m_iCurSlotMaxLevel, 0.f, m_fMaxLevel);

	m_fSpeedRate = i3Math::abs(m_fMotifyLevel - m_fCurLevel)/20.f;
	
	if( m_pSlotScrollbar )
	{
		i3UIManager* pMgr = i3UI::getManager();

		if( pMgr )
			pMgr->SetFocusControl( m_pSlotScrollbar );
	}

}

//UTIL 무기 및, 스킬 능력치 스트링 읽어오는 유틸
void UITabProfileSkill::GetWeaponTypeStr(CHARA_CLASS type, i3::rc_wstring& out_wstr)
{
	switch(type)
	{
	case CHARA_CLASS_ASSAULT:
		out_wstr = GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ASSAULT");
		break;
	case CHARA_CLASS_SMG:
		out_wstr = GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SMG");
		break;
	case CHARA_CLASS_SNIPER:
		out_wstr = GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SNIPER");
		break;
	case CHARA_CLASS_SHOTGUN:
		out_wstr = GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SHOTGUN");
		break;
	case CHARA_CLASS_MACHINE:
		out_wstr = GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_MG");
		break;
	default:
		out_wstr = GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_UNKNOWN");
	}
}

void UITabProfileSkill::GetSkillNameTypeStr(INT32 category, INT32 index, i3::rc_wstring& out)
{
	char strTemplate[MAX_STRING_COUNT] = {0,};
	INT32 tempidx = index + 1;

	switch(category)
	{
	case	0:
		i3::snprintf( strTemplate, MAX_STRING_COUNT, "STR_SKILL_COMMON_%d_NAME",tempidx); 
		break;
	case	1:
		i3::snprintf( strTemplate, MAX_STRING_COUNT, "STR_SKILL_MAIN_%d_NAME",tempidx); 
		break;
	case	2:
		i3::snprintf( strTemplate, MAX_STRING_COUNT, "STR_SKILL_ASSIST_%d_NAME",tempidx); 
		break;
	}
	
	out = GAME_RCSTRING(strTemplate);
}

void UITabProfileSkill::GetSkillTypeDescripStr(INT32 index, i3::rc_wstring& out_wstr, bool bMax)
{
	char strTemplate[MAX_STRING_COUNT] = {0,};

	if( bMax == false)
	{
		switch(m_iCurrentCategory)
		{
		case	0:
			i3::snprintf( strTemplate, MAX_STRING_COUNT, "STR_SKILL_COMMON_1_DESCRIP2"); 
			break;
		case	1:
			i3::snprintf( strTemplate, MAX_STRING_COUNT, "STR_SKILL_MAIN_%d_DESCRIP2",index); 
			break;
		case	2:
			i3::snprintf( strTemplate, MAX_STRING_COUNT, "STR_SKILL_COMMON_1_DESCRIP2"); 
			break;
		}
	}
	else
	{
		switch(m_iCurrentCategory)
		{
		case	0:
			i3::snprintf( strTemplate, MAX_STRING_COUNT, "STR_SKILL_COMMON_%d_MAXDESCRIP",index); 
			break;
		case	1:
			i3::snprintf( strTemplate, MAX_STRING_COUNT, "STR_SKILL_MAIN_%d_MAXDESCRIP",index); 
			break;
		case	2:
			i3::snprintf( strTemplate, MAX_STRING_COUNT, "STR_SKILL_ASSIST_%d_MAXDESCRIP",index); 
			break;
		}
	}

	out_wstr = GAME_RCSTRING(strTemplate);

}

//서버에 패킷 전송데이터 저장함수
void UITabProfileSkill::GetSendData(PACKET_SKILL_SET_REQ* _data)
{
	CHARA_INFO* charainfo  = const_cast<CHARA_INFO*>(CCharaInfoContext::i()->GetCharaInfo(m_CurrentAvatarSlot));
	UINT8 ServerCharaIndex = charainfo->getBasicInfo()->m_ui8CharaSlotIdx;
	_data->m_ui8SlotIdx  =	(UINT8)(ServerCharaIndex&0xFF);
	_data->m_CommonSkill =	m_Classtype.m_CommonSkill;
	_data->m_MainSkill =	m_Classtype.m_MainSkill;
	_data->m_AssistSkill =	m_Classtype.m_AssistSkill;
	_data->m_ui8MainLv =	m_Classtype.m_ui8MainSkillLv;
	_data->m_ui8AssistLv =	m_Classtype.m_ui8AssistSkillLv;
	_data->m_ui8CommonLv =	m_Classtype.m_ui8CommonSkillLv;
}

