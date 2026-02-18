#include "pch.h"
#if defined(SKILLSYSTEM)
#include "SkillManager.h"
#include "UIMainFrame.h"
#include "UITabProfileAbility.h"
#include "UITabProfileSkill.h"
#include "UIPopupLearnSkill.h"



I3_CLASS_INSTANCE( UIPopupLearnSkill);//, UIPopupBase);

static UIPopupLearnSkill * GetThis(void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupLearnSkill*) pFrame->GetPopup( UPW_SKILL_LEARN);
}
//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupLearnSkill_Confirm(LuaState * L)
	{
		GetThis()->OnOKButton();
		return 0;
	}
	int UIPopupLearnSkill_Cancel(LuaState * L)
	{
		GetThis()->OnCloseButton();
		return 0;
	}
}

LuaFuncReg UIPopupSkillLearn_Glue[] =
{
	{"Confirm",		UIPopupLearnSkill_Confirm	},
	{"Close",		UIPopupLearnSkill_Cancel	},
	{NULL,					NULL},
};

UIPopupLearnSkill::UIPopupLearnSkill()
{
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
}

UIPopupLearnSkill::~UIPopupLearnSkill()
{
	if( m_SlotSet)
		m_SlotSet->Destroy();
}

void UIPopupLearnSkill::InitState()
{
	UINT8 _Class = g_pGameContext->getSkillManager()->getCurrentClass(GetCurrentAvatar(), GetCurrentTab());
	m_SlotSet->SetSkillImage(m_SkillProperty._line);
	m_SlotSet->SetSkillText(m_SkillProperty._line);
	m_SlotSet->SetBG(FALSE);
	
	if( _Class > SKILL_SHOTGUN)
		m_SlotSet->SetCheckBoxEnable(UISkillSelectionSlot::SLOT_TYPE_PURPLE);
	else
		m_SlotSet->SetCheckBoxEnable((INT32)_Class);

	if(m_SkillProperty._direction == 1)
		m_SlotSet->SetArrowDirection(3,-1);
	else
		m_SlotSet->SetArrowDirection(-1,0);
	m_SlotSet->SetSelectionSlot(m_SkillProperty._direction);
}

void UIPopupLearnSkill::setNickText()
{
	char _text[MAX_STRING_COUNT] = {0,};
	CCharaInfoContext * CChara = g_pFramework->GetCharaInfoContext();
	SkillManager * Skill = g_pGameContext->getSkillManager();

	INT32 _CurrentClass = Skill->getCurrentClass(m_SkillProperty._currentAvater, m_SkillProperty._currentTab);
	sprintf_s(_text, "%s - %s", CChara->GetNickName(m_SkillProperty._currentAvater), Skill->getClassName(_CurrentClass));

	LuaState * L = _CallLuaFunction("setNickText");
	i3Lua::PushA2UTF8String(L, _text);
	i3Lua::PushA2UTF8String(L, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));
	i3Lua::PushA2UTF8String(L, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));
	_EndLuaFunction(L,3);
}

/*virtual*/ bool UIPopupLearnSkill::OnOKButton( void)
{
	PACKET_SET_SPECIALITY_REQ			skillSlotSet;
	skillSlotSet.m_ui8Slot = (UINT8)m_SkillProperty._currentAvater;
	skillSlotSet.m_ui8Type = (UINT8)m_SkillProperty._currentTab;
	skillSlotSet.m_ui8Class= (UINT8)m_SkillProperty._class;
	skillSlotSet.m_ui64SkillTree = m_SkillProperty._skilltree;
	g_pGameContext->getSkillManager()->setPacketSlot(&skillSlotSet);
	g_pGameContext->SetEvent(EVENT_SKILL_SLOT_LEARN);
	GetMainFrame()->TogglePopup(UPW_SKILL_LEARN);
	return UIPopupBase::OnOKButton();
}

/*virtual*/ bool UIPopupLearnSkill::OnCloseButton( void)
{
	GetMainFrame()->TogglePopup(UPW_SKILL_LEARN);
	return UIPopupBase::OnCloseButton();
}

/*virtual*/ bool UIPopupLearnSkill::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	m_SkillProperty = *(SKILL_PROPERTY*)pArg1;

	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	m_SlotSet->EnableSlot( 0, 0, NULL, NULL);
	InitState();
	setNickText();

	//LoadAgreement();
	return true;
}

/*virtual*/ bool UIPopupLearnSkill::OnExitStart()
{
	if( UIPopupBase::OnExitStart() == false)
		return false;

	m_SlotSet->SetVisible(FALSE);
	return true;
}

/*virtual*/ void UIPopupLearnSkill::_InitializeAtLoad(i3UIScene * pScene)
{
	m_pImageDummy = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameSkill");

	VEC3D vPos, vParentPos;
	VEC2D vSize;
	i3Vector::Zero( &vPos);
	i3Vector::Copy( &vParentPos, m_pImageDummy->getAccumulatedPos());

	m_SlotSet->SetEnable( TRUE);
	i3Vector::Copy( &vSize, m_SlotSet->GetSize());
	m_SlotSet->SetEnable( FALSE);

	i3Vector::Add( &vParentPos, 3.f, 3.f, 0.f);

	I3ASSERT( m_SlotSet != NULL);
	m_SlotSet->CalculatePos( m_SlotSet->GetOriginalPos(), 0, 0, &vParentPos, &vSize, 0.0f, 10.0f);
}

/*virtual*/ void UIPopupLearnSkill::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene( "UIRe/Scene/PBRe_PopUp_Skill_Learn.i3UIe", UIPopupSkillLearn_Glue);
	m_SlotSet = UISkillSelectionSlot::new_object();
	m_SlotSet->OnCreate(this);
}

//void UIPopupLearnSkill::LoadAgreement()
//{
//	i3FileStream FileStream;
//	i3FileStream FileStream1;
//	static INT32 _StringCount = 0;
//
//	if( FALSE == FileStream.Open( "config\\china\\data1.txt", STREAM_READ | STREAM_SHAREREAD) )
//		return;
//	FileStream1.Create("config\\china\\outdata.txt",0,0);
//	if( FALSE == FileStream1.Open( "config\\china\\outdata.txt", STREAM_WRITE | STREAM_READ | STREAM_SHAREREAD) )
//	{
//		return;
//	}
//
//	WCHAR16 wchTemp;
//	//	유니코드 파일인지 검사
//	FileStream.Read( &wchTemp, sizeof( WCHAR16));
//
//	//	여기에 걸리면 스크립트 파일이 유니코드가 아닙니다. 반드시 유니코드 파일이어야 합니다. komet
//	if( !CHECK_UNICODE(wchTemp))
//	{
//		return;
//	}
//
//	WCHAR16	wcharBuff[256];
//	WCHAR16 AllText[2865][128];
//	INT32	nText[2865];
//	for(INT32 i=0; i<2865; i++)
//		nText[i] = -1;
//
//	_StringCount = 0;
//	for(INT32 i=0; i<2865; i++)
//	{
//		FileStream.ReadLineW(wcharBuff);
//		i3String::CopyW(AllText[_StringCount++], wcharBuff, 128);
//	}
//	INT32 count = 1;
//	for(INT32 i=0; i<2865; i++)
//	{
//		for(INT32 j=i+1; j<2865; j++)
//		{
//			if(i3String::CompareW(AllText[i], AllText[j]) == 0)
//			{
//				nText[i] = count;
//				nText[j] = count;
//			}
//			else
//			{
//				i = j-1;
//				break;
//			}
//		}
//		count++;
//	}
//
//	char outbuff[256];
//	for(INT32 i=0; i<2865; i++)
//	{
//		INT32 index = i3::integral_to_comma_str(nText[i], outbuff, 250);
//		outbuff[index] = '\n';
//		outbuff[index+1] = '\0';
//		FileStream1.WriteLine(outbuff);
//	}
//	
//
//	//INT32 iLen = wcslen((WCHAR16*)wszTexts);
//	//	유니코드 문자를 멀티바이트 문자로 변환
//	//WideCharToMultiByte( GetCodePage(), 0, (WCHAR16*)wszTexts,iLen, szTexts, uiTextsSize, NULL, NULL);
//	FileStream1.Close();
//	FileStream.Close();
//
//}

#endif