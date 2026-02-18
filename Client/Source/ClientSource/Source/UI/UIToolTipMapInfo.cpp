#include "pch.h"
#include "UIToolTipMapInfo.h"

#include "UIUtil.h"

#include "../TextSet.h"

I3_CLASS_INSTANCE( UIToolTipMapInfo, UIToolTipBase);

UIToolTipMapInfo::UIToolTipMapInfo()
{
}

UIToolTipMapInfo::~UIToolTipMapInfo()
{
}

/*virtual*/ void UIToolTipMapInfo::OnCreate( i3GameNode * pParent)
{
	UIToolTipBase::OnCreate( pParent);

	// Load Scene
	AddScene( "UIRe/PointBlankRe_Lobby_MapInfo_ToolTip.i3UIe");
}

/*virtual*/ void UIToolTipMapInfo::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	UIToolTipBase::OnEntranceStart( pArg1, pArg2);

	SetRoomInfo( static_cast<ROOM_INFO_BASIC*> (pArg1));
}

void UIToolTipMapInfo::SetRoomInfo( ROOM_INFO_BASIC * pInfo)
{
	// Map Image
	INT32 MapIamage = g_pFramework->GetMapShapeIdx( (STAGE_ID) pInfo->_StageID);

	// Mode
	std::string strModeName;
	BOOL bAllowChangeMode = FALSE;
	STAGE_TYPE stageType = GET_STAGETYPE( pInfo->_StageID);
	GetStageModeName( stageType, strModeName, bAllowChangeMode);

	// ½Â¸® Á¶°Ç
	char strVictoryCondition[ MAX_STRING_COUNT];
	if( g_pGameContext->IsStageMaskTraining())
	{
		// ÈÆ·ÃÀü Àû±º ¼ö
		sprintf( strVictoryCondition, "%d", g_pGameContext->getEnemyAiCount());
	}
	else
	{
		// ½Â¸®Á¶°Ç
		i3String::Copy( strVictoryCondition, TextSet::VictoryCondition(stageType, gameType));
	}

	LuaState * L = _CallLuaFunction( "SetInfo");
	i3Lua::PushInteger( L, MapImage);
	i3Lua::PushA2UTF8String( L, strModeName.c_str());
	i3Lua::PushA2UTF8String( L, strVictoryCondition);
	_EndLuaFunction( L, 13);
}

