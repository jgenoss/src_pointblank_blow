#include "pch.h"
#include "UIPopupSelectMode.h"

#include "UIMainFrame.h"
#include "UIPopupCreateRoom.h"
#include "UIUtil.h"

#include "../GameGUI.h"

struct UIMapMode {
	INT32			_ArrangeID;	// UI에 설정된 종류별 ID
	USM_TYPE		_Type;
	UI_MODE_TYPE	_Mode;
	STAGE_TYPE		_StageType;
	STAGE_SLOT_MODE	_SlotMode;
	STAGE_OPTION_TYPE _Option;
};

static UIPopupSettingRoom * GetSettingRoom( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	UIPOPUPWIN PopupWin = UPW_SETTINGROOM;
	if(pFrame->GetCurrentPhaseType() == UIPHASE_LOBBY)
		PopupWin = UPW_CREATEROOM;

	UIPopupBase * pPopup = pFrame->GetPopup( PopupWin);
	I3ASSERT( pPopup);
	I3ASSERT( pPopup->IsTypeOf( UIPopupSettingRoom::GetClassMeta()));

	return (UIPopupSettingRoom*) pPopup;
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupSelectMode_SelectMode( LuaState * L)
	{
		INT32 mode = i3Lua::GetIntegerArg( L, 1);
		INT32 slot = i3Lua::GetIntegerArg( L, 2);
		// set mode
		UIPopupSelectMode * pSelector = (UIPopupSelectMode*) g_pFramework->GetUIMainframe()->GetPopup( UPW_SELECTMODE);
		pSelector->SelectMode( (USM_TYPE) mode, slot);
		return 0;
	}

	int UIPopupSelectMode_Scroll( LuaState * L)
	{
		UIPopupSelectMode * pSelector = (UIPopupSelectMode*) g_pFramework->GetUIMainframe()->GetPopup( UPW_SELECTMODE);
		pSelector->Scroll();
		return 0;
	}

	int UIPopupSelectMode_Cancel( LuaState * L)
	{
		g_pFramework->GetUIMainframe()->ClosePopup( UPW_SELECTMODE);
		return 0;
	}
}

LuaFuncReg UIPopupSelectMode_Glue[] = {
	{ "SelectMode",			UIPopupSelectMode_SelectMode},
	{ "Scroll",				UIPopupSelectMode_Scroll	},
	{ "Cancel",				UIPopupSelectMode_Cancel	},
	{NULL,					NULL}
};

//------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIPopupSelectMode, UIPopupSubBase);

UIPopupSelectMode::UIPopupSelectMode()
{
	m_StartSpecialIndex = 0;
	m_CurrentSpecialIndex = 0;
}

UIPopupSelectMode::~UIPopupSelectMode()
{
	__ReleaseModeList();
}

void UIPopupSelectMode::__AddMode( USM_TYPE type, STAGE_TYPE stageType, STAGE_OPTION_TYPE option /*= STAGE_OPTION_NONE*/,
								  STAGE_SLOT_MODE slot /*= STAGE_SLOT_MODE_NORMAL*/)
{
	UIMapMode * pPossibleMode = (UIMapMode*) i3MemAlloc( sizeof( UIMapMode));
	I3ASSERT( pPossibleMode);

	pPossibleMode->_Type = type;

	pPossibleMode->_StageType = stageType;
	pPossibleMode->_Option = option;
	pPossibleMode->_SlotMode = slot;

	switch( stageType)
	{
	case STAGE_TYPE_DEATHMATCH :	pPossibleMode->_Mode = UMT_DEATHMATCH;	break;
	case STAGE_TYPE_BOMB :			pPossibleMode->_Mode = UMT_BOMBMISSION;	break;
	case STAGE_TYPE_DESTROY :		pPossibleMode->_Mode = UMT_DESTROY;		break;
	case STAGE_TYPE_ANNIHILATION :	pPossibleMode->_Mode = UMT_ANNIHILATION; break;
	case STAGE_TYPE_DEFENCE :		pPossibleMode->_Mode = UMT_DEFENCE;		break;
	case STAGE_TYPE_SUDDEN_DEATH :	pPossibleMode->_Mode = UMT_SUDDENDEATH;	break;
	case STAGE_TYPE_ESCAPE :		pPossibleMode->_Mode = UMT_ESCAPE;		break;
	case STAGE_TYPE_HEAD_HUNTER :	pPossibleMode->_Mode = UMT_HEADHUNTER;	break;
	case STAGE_TYPE_HEAD_KILLER :	pPossibleMode->_Mode = UMT_HEADHUNTER;	break;
	case STAGE_TYPE_TUTORIAL :		pPossibleMode->_Mode = UMT_TUTORIAL;	break;
	case STAGE_TYPE_DOMINATION :	pPossibleMode->_Mode = UMT_DOMINATION;	break;
	default :	I3FATAL( "invalid stageType %d!!!", stageType);	break;
	}

	switch( option)
	{
	case STAGE_OPTION_NONE :		break;
	case STAGE_OPTION_AI :			pPossibleMode->_Mode = UMT_AI;		break;
	case STAGE_OPTION_RPG :			break;
	case STAGE_OPTION_SNIPER :		break;
	case STAGE_OPTION_ANNIHILATION : pPossibleMode->_Mode = UMT_ANNIHILATION;	break;
	case STAGE_OPTION_SUDDENDEATH : pPossibleMode->_Mode = UMT_SUDDENDEATH;	break;
	case STAGE_OPTION_HEADHUNTER :
	case STAGE_OPTION_HEADKILLER :	pPossibleMode->_Mode = UMT_HEADHUNTER;	break;
	case STAGE_OPTION_SHOTGUN :		break;
	case STAGE_OPTION_KNUCKLE :		break;
	case STAGE_OPTION_TUTORIAL : pPossibleMode->_Mode = UMT_TUTORIAL; break;
	default :			I3FATAL( "not support option!!!! %d", option);
	}

	m_ModeList.Add( pPossibleMode);

	switch( type)
	{
	case USM_TYPE_NORMAL :	m_NormalList.Add( pPossibleMode);	break;
	case USM_TYPE_MISSION : m_MissionList.Add( pPossibleMode);	break;
	case USM_TYPE_DINO :	m_DinoList.Add( pPossibleMode);		break;
	case USM_TYPE_SPECIAL : m_SpecialList.Add( pPossibleMode);	break;
	default :				I3FATAL( "invalid type !!");		break;
	}
}

void UIPopupSelectMode::__ReleaseModeList( void)
{
	INT32 i;

	for( i = 0; i < m_ModeList.GetCount(); i++)
	{
		UIMapMode * pMode = (UIMapMode*) m_ModeList.Items[ i];
		I3MEM_SAFE_FREE( pMode);
	}

	m_ModeList.Clear();

	m_NormalList.Clear();
	m_MissionList.Clear();
	m_DinoList.Clear();
	m_SpecialList.Clear();
}

void UIPopupSelectMode::_SetModeInfo( void)
{
	LuaState * L;
	INT32 i;

	__ReleaseModeList();
	
	_FindPossibleMode();

	// Normal mode
	L = _CallLuaFunction( "SetNormalCount");
	if( L != NULL)
	{
		i3Lua::PushInteger( L, m_NormalList.GetCount());
		_EndLuaFunction( L, 1);
	}

	
	std::string strMode;

	for( i = 0; i < m_NormalList.GetCount(); i++)
	{
		UIMapMode * pMode = (UIMapMode*) m_NormalList.Items[ i];

		GameUI::GetStageModeName( strMode, pMode->_StageType, pMode->_Option);

		L = _CallLuaFunction( "SetNormalMode");
		if( L != NULL)
		{
			i3Lua::PushInteger( L, i);
			i3Lua::PushA2UTF8String( L, strMode.c_str());
			i3Lua::PushColor( L, GameUI::GetStageModeColor( pMode->_StageType, pMode->_Option));
			_EndLuaFunction( L, 6);
		}
	}

	// Mission Mode
	L = _CallLuaFunction( "SetMissionCount");
	if( L != NULL)
	{
		i3Lua::PushInteger( L, m_MissionList.GetCount());
		_EndLuaFunction( L, 1);
	}

	for( i = 0; i < m_MissionList.GetCount(); i++)
	{
		UIMapMode * pMode = (UIMapMode*) m_MissionList.Items[ i];

		GameUI::GetStageModeName( strMode, pMode->_StageType, pMode->_Option);

		L = _CallLuaFunction( "SetMissionMode");
		if( L != NULL)
		{
			i3Lua::PushInteger( L, i);
			i3Lua::PushA2UTF8String( L, strMode.c_str());
			i3Lua::PushColor( L, GameUI::GetStageModeColor( pMode->_StageType, pMode->_Option));
			_EndLuaFunction( L, 6);
		}
	}

	// Dino Mode
	L = _CallLuaFunction( "SetDinoCount");
	if( L != NULL)
	{
		i3Lua::PushInteger( L, m_DinoList.GetCount());
		_EndLuaFunction( L, 1);
	}

	for( i = 0; i < m_DinoList.GetCount(); i++)
	{
		UIMapMode * pMode = (UIMapMode*) m_DinoList.Items[ i];

		GameUI::GetStageModeName( strMode, pMode->_StageType, pMode->_Option);

		L = _CallLuaFunction( "SetDinoMode");
		if( L != NULL)
		{
			i3Lua::PushInteger( L, i);
			i3Lua::PushA2UTF8String( L, strMode.c_str());
			i3Lua::PushColor( L, GameUI::GetStageModeColor( pMode->_StageType, pMode->_Option));
			_EndLuaFunction( L, 6);
		}
	}

	_UpdateSpecialMode();
	
}

/*virtual*/ void UIPopupSelectMode::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupSubBase::_InitializeAtLoad( pScene);

	m_pScrollBar = (i3UIScrollBar*) pScene->FindChildByName( "scrollBarSpecial");
	I3ASSERT( m_pScrollBar != NULL);
}

/*virtual*/ void UIPopupSelectMode::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupSubBase::_ClearAtUnload( pScene);

	m_pScrollBar = NULL;
}

/*virtual*/ void UIPopupSelectMode::_OnClose( void)
{
	GetMainFrame()->ClosePopup( UPW_SELECTMODE);
}

void UIPopupSelectMode::_FindPossibleMode( void)
{
	bool bLobby = true;			// 로비에서만 선택 가능한 모드 분류
	bool bNormalMode = true;	// 일반전일때만 선택 가능한 모드 분류

	// 
	if( g_pFramework->GetUIMainframe()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
	{
		bLobby = false;

		g_pGameContext->IsShotGunRoom() ? bNormalMode = false : bNormalMode = true;
	}
	if(bNormalMode == true)
	{
		// 일반모드
		{
			// 단체전
			if( IsAvailableStageMode( STAGE_TYPE_DEATHMATCH))
			{
				__AddMode( USM_TYPE_NORMAL, STAGE_TYPE_DEATHMATCH);

				//섬멸전은 단체전의 맵을 공유해서 사용하기 때문에 여기서 추가
				__AddMode( USM_TYPE_NORMAL, STAGE_TYPE_ANNIHILATION, STAGE_OPTION_ANNIHILATION);
			}
			// 4vs4
			// 4vs4맵을 사용할 수 있으면 true
			//__AddMode( USM_TYPE_NORMAL, UMT_DEATHMATCH, ULM_NONE, STAGE_SLOT_MODE_4VS4);
				

			// 현재 페이지가 로비에서만 설정 가능한 맵
			if(bLobby == true)
			{
				// AI 챌린지
				if( IsSpecialMode( STAGE_OPTION_AI))
					__AddMode( USM_TYPE_NORMAL, STAGE_TYPE_DEATHMATCH, STAGE_OPTION_AI);
			}

			// 헤드헌터
			if( IsSpecialMode( STAGE_OPTION_HEADHUNTER))
				__AddMode( USM_TYPE_NORMAL, STAGE_TYPE_HEAD_HUNTER);
		}

		// 임무모드
		{
			if( IsAvailableStageMode( STAGE_TYPE_ANNIHILATION))
				__AddMode( USM_TYPE_MISSION, STAGE_TYPE_ANNIHILATION);

			if( IsAvailableStageMode( STAGE_TYPE_DESTROY))
				__AddMode( USM_TYPE_MISSION, STAGE_TYPE_DESTROY);

			if( IsAvailableStageMode( STAGE_TYPE_BOMB))
				__AddMode( USM_TYPE_MISSION, STAGE_TYPE_BOMB);

			if( IsAvailableStageMode( STAGE_TYPE_DEFENCE))
				__AddMode( USM_TYPE_MISSION, STAGE_TYPE_DEFENCE);
		}

		// 공룡모드
		{
			// 로비에서만 설정 가능한 맵
			if(bLobby == true)
			{
				if( IsAvailableStageMode( STAGE_TYPE_ESCAPE))
					__AddMode( USM_TYPE_DINO, STAGE_TYPE_ESCAPE);

				/// [제압 미션]모드 추가.
				//if( IsAvailableStageMode( STAGE_TYPE_DOMINATION))
				if( LocaleValue::Enable( "EnableDomination"))
				{
					__AddMode( USM_TYPE_DINO, STAGE_TYPE_DOMINATION);
				}
			}
		}
	}

	// 특수전
	{
		if(bLobby == true || bNormalMode == false)
		{
			if( IsSpecialMode( STAGE_OPTION_SHOTGUN))
			{
				__AddMode( USM_TYPE_SPECIAL, STAGE_TYPE_DEATHMATCH, STAGE_OPTION_SHOTGUN);
				__AddMode( USM_TYPE_SPECIAL, STAGE_TYPE_ANNIHILATION, STAGE_OPTION_SHOTGUN);
				__AddMode( USM_TYPE_SPECIAL, STAGE_TYPE_BOMB, STAGE_OPTION_SHOTGUN);
				__AddMode( USM_TYPE_SPECIAL, STAGE_TYPE_DESTROY, STAGE_OPTION_SHOTGUN);
				__AddMode( USM_TYPE_SPECIAL, STAGE_TYPE_DEFENCE, STAGE_OPTION_SHOTGUN);
			}
		}

		/*if( IsSpecialMode( STAGE_OPTION_RPG))
		__AddMode( USM_TYPE_SPECIAL, STAGE_TYPE_DEATHMATCH, STAGE_OPTION_RPG);*/

		if(bLobby == true)
		{
			if( IsSpecialMode( STAGE_OPTION_SNIPER))
				__AddMode( USM_TYPE_SPECIAL, STAGE_TYPE_DEATHMATCH, STAGE_OPTION_SNIPER);

			if( IsSpecialMode( STAGE_OPTION_KNUCKLE))
				__AddMode( USM_TYPE_SPECIAL, STAGE_TYPE_DEATHMATCH, STAGE_OPTION_KNUCKLE);
		}

		_EnableScrollBar( (m_SpecialList.GetCount() > MAX_UI_SPECIAL_COUNT)?true:false);
		m_pScrollBar->setScrollPos( 0);
	}
}

void UIPopupSelectMode::_UpdateSpecialMode( void)
{
	INT32 i;
	LuaState * L;
	std::string strMode;

	for( i = 0; i < MAX_UI_SPECIAL_COUNT; i++)
	{
		INT32 idx = m_StartSpecialIndex + i;

		if( idx >= m_SpecialList.GetCount())
		{
			L = _CallLuaFunction( "SetSpecialEnable");
			if( L != NULL)
			{
				i3Lua::PushInteger( L, i);
				i3Lua::PushBoolean( L, FALSE);
				_EndLuaFunction( L, 2);
			}
		}
		else
		{
			L = _CallLuaFunction( "SetSpecialEnable");
			if( L != NULL)
			{
				i3Lua::PushInteger( L, i);
				i3Lua::PushBoolean( L, TRUE);
				_EndLuaFunction( L, 2);
			}

			UIMapMode * pMode = (UIMapMode*) m_SpecialList.Items[ idx];

			GameUI::GetStageModeName( strMode, pMode->_StageType, pMode->_Option);

			L = _CallLuaFunction( "SetSpecialMode");
			if( L != NULL)
			{
				i3Lua::PushInteger( L, i);
				i3Lua::PushA2UTF8String( L, strMode.c_str());
				i3Lua::PushColor( L, GameUI::GetStageModeColor( pMode->_StageType, pMode->_Option));
				_EndLuaFunction( L, 6);
			}
		}
	}
}

void UIPopupSelectMode::_EnableScrollBar( bool bEnable)
{
	if( m_SpecialList.GetCount() > MAX_UI_SPECIAL_COUNT)
	{
		m_pScrollBar->EnableCtrl( true);
		m_pScrollBar->setPageSize(1);
		m_pScrollBar->setTickSize(1); 

		INT32 SliderMax = (m_SpecialList.GetCount() + 2) / 3;

		SliderMax = (SliderMax > 2 ) ? SliderMax - 1 : 1;

		m_pScrollBar->setScrollRange( 0, SliderMax);
	}
	else
	{
		m_pScrollBar->EnableCtrl( false);
	}
}

void UIPopupSelectMode::_SetInitUI(void)
{
	LuaState * L = _CallLuaFunction("InitUI");
#if defined(LOCALE_CHINA)
	i3Lua::PushBoolean(L, false);
#else
	i3Lua::PushBoolean(L, true);
#endif
	_EndLuaFunction(L, 1);
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIPopupSelectMode::OnCreate( i3GameNode * pParent)
{
	UIPopupSubBase::OnCreate( pParent);

	// Load Scene
	AddScene( "UIRe/PointBlankRe_Lobby_CreatRoom_ModeSelect.i3UIe", UIPopupSelectMode_Glue);
}


/*virtual*/ bool UIPopupSelectMode::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupSubBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	m_StartSpecialIndex = 0;
	m_CurrentSpecialIndex = 0;

	return true;
}

/*virtual*/ void UIPopupSelectMode::OnEntranceEnd( void)
{
	UIPopupSubBase::OnEntranceEnd();
	// 임시로 OK버튼 Disable 및 Cancel 버튼 위치 조정합니다.
	// OK버튼이 템플릿에서 삭제 될 경우 해당 코드를 제거해야합니다.
	LuaState * L = _CallLuaFunction("SetOKCancelButtonInfo");
	_EndLuaFunction(L , 0);
	_SetModeInfo();
	_SetInitUI();

	i3UI::setDisableUIInput(false);	
}


/*virtual*/ bool UIPopupSelectMode::OnCloseButton( void)
{
	if( this->isEnable())
	{
		_OnClose();
		return false;
	}

	return true;
}

/*virtual*/ void UIPopupSelectMode::OnWheel( i3UIControl * pControl, INT32 zDelta)
{
	if( zDelta != 0)
	{
		// 다른 윈도우 또는 신에서의 이벤트를 받아서 처리하게 된다.
		// 이 경우 스크롤바에 이벤트를 주자!!
		if( m_pScrollBar->isEnable())
		{
			m_pScrollBar->OnNotify( pControl, I3UI_EVT_MOUSEWHEEL, zDelta, 0);
			Scroll();
		}
	}
}


void UIPopupSelectMode::SelectMode( USM_TYPE type, INT32 slot)
{
	UIMapMode * pMode = NULL;

	switch( type)
	{
	case USM_TYPE_NORMAL :
		I3_BOUNDCHK( slot, m_NormalList.GetCount());
		pMode = (UIMapMode*) m_NormalList.Items[ slot];
		break;

	case USM_TYPE_MISSION :
		I3_BOUNDCHK( slot, m_MissionList.GetCount());
		pMode = (UIMapMode*) m_MissionList.Items[ slot];
		break;

	case USM_TYPE_DINO :
		I3_BOUNDCHK( slot, m_DinoList.GetCount());
		pMode = (UIMapMode*) m_DinoList.Items[ slot];
		break;

	case USM_TYPE_SPECIAL :
		I3_BOUNDCHK( slot + m_StartSpecialIndex, m_SpecialList.GetCount());
		pMode = (UIMapMode*) m_SpecialList.Items[ slot + m_StartSpecialIndex];
		break;
	}

	UI_LIMIT_MODE limit = ULM_NONE;

	switch( pMode->_Option)
	{
	case STAGE_OPTION_NONE :
	case STAGE_OPTION_AI :			break;
	case STAGE_OPTION_RPG :			limit = ULM_ROCKET; break;
	case STAGE_OPTION_SNIPER :		limit = ULM_SNIPER; break;
	case STAGE_OPTION_ANNIHILATION :
	case STAGE_OPTION_SUDDENDEATH :
	case STAGE_OPTION_HEADHUNTER :
	case STAGE_OPTION_HEADKILLER :	break;
	case STAGE_OPTION_SHOTGUN :		limit = ULM_SHOTGUN; break;
	case STAGE_OPTION_KNUCKLE :		limit = ULM_KNUCKLE; break;
	case STAGE_OPTION_TUTORIAL :	break;
	default : I3FATAL( "invali stage option!!!%d", pMode->_Option); break;
	}
	
	I3ASSERT( pMode != NULL);
	GetSettingRoom()->SelectMode( pMode->_Mode, limit);
}

void UIPopupSelectMode::Scroll( void)
{
	m_StartSpecialIndex = m_pScrollBar->getScrollPos() * 3;
	_UpdateSpecialMode();
}

