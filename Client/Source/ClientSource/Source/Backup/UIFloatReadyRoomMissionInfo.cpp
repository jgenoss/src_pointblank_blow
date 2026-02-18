#include "pch.h"
#include "UIFloatReadyRoomMissionInfo.h"

#include "../QuestTextDictionary.h"

I3_CLASS_INSTANCE( UIFloatReadyRoomMissionInfo);//, UISubBase);

UIFloatReadyRoomMissionInfo::UIFloatReadyRoomMissionInfo()
{
	//m_AlignedPos = UIFLOATING_POS_LEFTTOP;

#if defined( I3_DEBUG)
	SetName( "ReadyRoomMissionInfo");
#endif
}

UIFloatReadyRoomMissionInfo::~UIFloatReadyRoomMissionInfo()
{
}

void UIFloatReadyRoomMissionInfo::_UpdateMissionInfo( void)
{
#if defined( ENABLE_MISSION_CARD)
	INT32 i;
	INT32 nRealSlot = 0;//ºó Äù½ºÆ®´Â ³Ñ¾î°¡µµ·Ï
	LuaState * L;

	CQuestManager * pManager = g_pGameContext->getQuestManager(true);
	if( pManager == NULL)
		return;
	
	if( pManager->isQuestActivated())
	{
		char	strTemp[MAX_STRING_COUNT];

		CCardSet * pCardSet = pManager->_getActiveCardSet();
		if( pCardSet == NULL)
			return;

		CCardInfo * pCardInfo = pCardSet->getActiveCard();
		if( pCardInfo == NULL)
			return;

		g_pQuestTextDictionary->GetCardSetNameString( strTemp, MAX_STRING_COUNT, pCardSet->GetID());

		L = _CallLuaFunction( "SetCardInfo");
		if( L != NULL)
		{
			i3Lua::PushA2UTF8String( L, strTemp);
			_EndLuaFunction( L, 1);
		}

		_SetCardImage( pCardSet->GetID(), pCardInfo->GetIndx());

		for( i = 0; i < MAX_CHALLENGE_COUNT; i++)
		{
			CQuestBase * pQuest = pManager->getActiveQuest( i);
			if ( pQuest == NULL )
				continue;

			nRealSlot++;
		}

		L = _CallLuaFunction( "ClearCardList");
		if( L != NULL)
		{
			i3Lua::PushInteger( L, nRealSlot);
			_EndLuaFunction( L, 1);
		}

		nRealSlot = 0;
		for( i = 0 ; i < MAX_CHALLENGE_COUNT ; ++i )
		{
			CQuestBase * pQuest = pManager->getActiveQuest( i);
			if ( pQuest == NULL )
				continue;

			pQuest->OnQueryHUDDesc( strTemp, sizeof( strTemp));

			INT32 CheckedBoxState = 0;

			switch( pQuest->OnQueryState())
			{
			case QST_NOT_STARTED:	CheckedBoxState = 0;	break;
			case QST_DOING:			CheckedBoxState = 0;	break;
			case QST_COMPLETED:		CheckedBoxState = 1;	break;
			}

			L = _CallLuaFunction( "SetCardSlot");
			if( L != NULL)
			{
				i3Lua::PushInteger( L, nRealSlot);
				i3Lua::PushA2UTF8String( L, strTemp);
				i3Lua::PushInteger( L, CheckedBoxState);
				_EndLuaFunction( L, 3);
			}

			nRealSlot++;
		}

		L = _CallLuaFunction( "UpdateCardSlot");
		if( L != NULL)
		{
			_EndLuaFunction( L, 0); 
		}

	}
	else
	{
		LuaState * L = _CallLuaFunction("SetCardOnOff");
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 1);
	}
#endif
}

void UIFloatReadyRoomMissionInfo::_SetCardImage( CARDSET_ID id, INT32 idx)
{
	CQuestManager * pQuestManager = g_pGameContext->getQuestManager();
	if( pQuestManager == NULL)
		return;

	const char *TemplateName = pQuestManager->GetTemplateName( (ENUM_QUEST_CARDSET_TYPE)id );

	i3UITemplate_ButtonImageSet * pSetTemplate = (i3UITemplate_ButtonImageSet*) i3UI::FindTemplateByName( TemplateName);
	if( pSetTemplate != NULL)
	{
		if( m_pCardImg->getTemplate() != pSetTemplate)
		{
			m_pCardImg->SetTemplate( pSetTemplate);
			if(id == 0)
			{
				m_pCardImg->SetShapeIdx(-1);
			}
			else
			{
				m_pCardImg->SetShapeIdx(idx);
			}
		}
		else if(id == QUEST_CARDSET_TYPE_TUTORIAL)
			m_pCardImg->SetShapeIdx(idx);

	}
	LuaState * L = _CallLuaFunction("SetCardOnOff");
	i3Lua::PushBoolean(L, true);
	_EndLuaFunction(L, 1);
}

/*virtual*/ void UIFloatReadyRoomMissionInfo::BindScene( i3UIScene * pScene)
{
	I3ASSERT( pScene != NULL);

	m_pCardImg = (i3UIButtonImageSet*) pScene->FindChildByName( "cardimage");
	I3ASSERT( m_pCardImg != NULL);
}

/*virtual*/ void UIFloatReadyRoomMissionInfo::UnBindScene( void)
{
	m_pCardImg = NULL;
}

/*virtual*/ bool UIFloatReadyRoomMissionInfo::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UISubBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_UpdateMissionInfo();

	return true;
}


