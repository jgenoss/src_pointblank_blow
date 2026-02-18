#include "pch.h"
#include "UIFloatReadyRoomMissionInfo_V11.h"

#include "MissionCard/MCard.h"
#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCardSet.h"
#include "MissionCard/MCardSetInfo.h"
#include "MissionCard/MCardQuest.h"

I3_CLASS_INSTANCE( UIFloatReadyRoomMissionInfo_V11);

UIFloatReadyRoomMissionInfo_V11::UIFloatReadyRoomMissionInfo_V11()
{
	//m_AlignedPos = UIFLOATING_POS_LEFTTOP;

#if defined( I3_DEBUG)
	SetName( "ReadyRoomMissionInfo");
#endif
}

UIFloatReadyRoomMissionInfo_V11::~UIFloatReadyRoomMissionInfo_V11()
{
}

void UIFloatReadyRoomMissionInfo_V11::_UpdateMissionInfo( void)
{
	INT32 i;
	INT32 nRealSlot = 0;//ºó Äù½ºÆ®´Â ³Ñ¾î°¡µµ·Ï
	LuaState * L;

	MCard* pCard = MCardUtil::GetSelectedMCard();

	if( pCard)
	{
		MCardSet* pCurSet = MCardUtil::GetSelectedMCardSet();
		const MCardSetInfo* pSetInfo = pCurSet->GetMCardSetInfo();

		L = _CallLuaFunction( "SetCardInfo");
		if( L != nullptr)
		{
			i3Lua::PushStringUTF16To8( L, pSetInfo->wstrTitle);
			_EndLuaFunction( L, 1);
		}

		_SetCardImage(); 

		for( i = 0; i < MAX_CHALLENGE_COUNT; i++)
		{
			MCardQuest* pQuest = pCard->GetMCardQuest(i);

			if (pQuest->IsActive() == false )
				continue;

			nRealSlot++;
		}

		L = _CallLuaFunction( "ClearCardList");
		i3Lua::PushInteger(L, nRealSlot);
		_EndLuaFunction( L, 1);

		nRealSlot = 0;
		
		i3::rc_wstring wstrDesc;

		for( i = 0 ; i < MAX_CHALLENGE_COUNT ; ++i )
		{
			MCardQuest* pQuest = pCard->GetMCardQuest(i);

			if (pQuest->IsActive() == false )
				continue;

			wstrDesc = pQuest->GetCurrHUDDesc();

			L = _CallLuaFunction( "SetCardSlot");
			if( L != nullptr)
			{
				i3Lua::PushInteger( L, nRealSlot);
				i3Lua::PushStringUTF16To8( L, wstrDesc);
				_EndLuaFunction( L, 2);
			}

			nRealSlot++;
		}

	}
	else
	{
		L = _CallLuaFunction("SetCardOnOff");
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 1);
	}

}

void UIFloatReadyRoomMissionInfo_V11::_SetCardImage()
{
	MCardSet* pCardSet = MCardUtil::GetSelectedMCardSet();
	INT32 cardSetType = 0;
	INT32 cardIndex = -1;

	i3::rc_wstring wstrTemplateName;
	
	if (pCardSet)
	{
		wstrTemplateName = pCardSet->GetMCardSetInfo()->wstrUITemplateName;
		cardSetType = pCardSet->GetMCardSetInfo()->setID;

		MCard* pCard = pCardSet->GetSelectedMCard();
		if (pCard)
			cardIndex = pCard->GetMyIndex();
	}

	i3::stack_string strTemplateName;
	i3::utf16_to_mb(wstrTemplateName, strTemplateName);

	i3UITemplate_ButtonImageSet * pSetTemplate = (i3UITemplate_ButtonImageSet*) i3UI::FindTemplateByName( strTemplateName.c_str());
	if( pSetTemplate != nullptr)
	{
		if( m_pCardImg->getTemplate() != pSetTemplate)
		{
			m_pCardImg->SetTemplate( pSetTemplate);

			if(cardSetType == 0)
			{
				m_pCardImg->SetShapeIdx(-1);
			}
			else
			{
				m_pCardImg->SetShapeIdx(cardIndex);
			}
		}
		else if(cardSetType == QUEST_CARDSET_TYPE_TUTORIAL)
			m_pCardImg->SetShapeIdx(cardIndex);

	}
	LuaState * L = _CallLuaFunction("SetCardOnOff");
	i3Lua::PushBoolean(L, true);
	_EndLuaFunction(L, 1);
}

/*virtual*/ void UIFloatReadyRoomMissionInfo_V11::BindScene( i3UIScene * pScene)
{
	I3ASSERT( pScene != nullptr);

	m_pCardImg = (i3UIButtonImageSet*) pScene->FindChildByName( "cardimage");
	I3ASSERT( m_pCardImg != nullptr);
}

/*virtual*/ void UIFloatReadyRoomMissionInfo_V11::UnBindScene( void)
{
	m_pCardImg = nullptr;
}

/*virtual*/ bool UIFloatReadyRoomMissionInfo_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UISubBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_UpdateMissionInfo();

	return true;
}


