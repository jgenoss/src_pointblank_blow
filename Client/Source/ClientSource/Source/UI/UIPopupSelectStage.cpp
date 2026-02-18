#include "pch.h"
#include "UIPopupSelectStage.h"

#include "UIMainFrame.h"
#include "UIPopupCreateRoom.h"
#include "UIPopupSettingRoom.h"
#include "UISlotStage.h"
#include "UIUtil.h"

#include "../EquipShapeManager.h"

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

static UIPopupSelectStage * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIPopupSelectStage *) pFrame->GetPopup( UPW_SELECTSTAGE);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPopupSelectStage_ClickOK( LuaState * L)
	{
		GetThis()->SelectOK();
		
		return 0;
	}

	int UIPopupSelectStage_ClickCancel( LuaState * L)
	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
		I3ASSERT( pFrame);
		pFrame->ClosePopup( UPW_SELECTSTAGE);

		return 0;
	}

	int UIPopupSelectState_Scroll( LuaState * L)
	{
		GetThis()->Scroll();
		return 0;
	}

	int UIPopupSelectState_ClickRandomMap( LuaState * L)
	{
		GetThis()->ClickRandomMap();
		return 0;
	}
}

LuaFuncReg UIPopupSelectStage_Glue[] = {
	{"ClickOK",					UIPopupSelectStage_ClickOK},
	{"ClickCancel",				UIPopupSelectStage_ClickCancel},
	{"Scroll",					UIPopupSelectState_Scroll},
	{"ClickRandomMap",			UIPopupSelectState_ClickRandomMap},
	{NULL,						NULL}
};

I3_CLASS_INSTANCE( UIPopupSelectStage, UIPopupSubBase);

UIPopupSelectStage::UIPopupSelectStage()
{
	m_StartIdx = 0;

	m_pScrollBar = NULL;
	i3mem::FillZero( m_pSlot, sizeof( m_pSlot));

	m_bRandomMap = false;
}

UIPopupSelectStage::~UIPopupSelectStage()
{
	INT32 i;

	for( i = 0; i < MAPSLOT_COUNT; i++)
	{
		if( m_pSlot[ i])
		{
			m_pSlot[ i]->Destroy();
		}
	}
}

/*virtual*/ void UIPopupSelectStage::_InitializeAtLoad( i3UIScene * pScene)
{
	INT32 i;

	UIPopupSubBase::_InitializeAtLoad( pScene);

	m_pScrollBar = (i3UIScrollBar*) pScene->FindChildByName( "i3UIScrollBar_LCRSS");
	I3ASSERT( m_pScrollBar);
	m_pScrollBar->setTickSize( 1);

	m_pImageDummy = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd3_LCRSS");
	I3ASSERT( m_pImageDummy);

	LuaState * L = _CallLuaFunction("SetCheckRandomMap");

	m_bRandomMap = GetSettingRoom()->getRandomMap();
	
	if(L != NULL)
	{
		i3Lua::PushInteger(L , m_bRandomMap ? 1 : 0);
		_EndLuaFunction( L, 1);
	}

	m_SelectRandomMapList.Clear();

	if(m_bRandomMap)
	{
		for(i = 0 ; i < g_pGameContext->GetRandomMapIndexCount() ; i++)
		{
			INT32 MapIndex = g_pGameContext->GetRandomMapIndex(i);	
			m_SelectRandomMapList.Add((void*)MapIndex);
		}
	}

	VEC3D vPos, vParentPos;
	VEC2D vSize;
	i3Vector::Zero( &vPos);
	
	i3Vector::Copy( &vParentPos, m_pImageDummy->getAccumulatedPos());

	m_pSlot[0]->SetEnable( TRUE);
	i3Vector::Copy( &vSize, m_pSlot[0]->GetSize());
	m_pSlot[0]->SetEnable( FALSE);

	VEC2D * pFrameSize = m_pImageDummy->getSize();
	i3Vector::Scale( &vSize, pFrameSize, 0.33f);
//	i3Vector::Sub( &vSize, &vDiv, &vSize);
	
	i3Vector::Add( &vParentPos, 3.f, 3.f, 0.f);

	for( i = 0; i < MAPSLOT_COUNT; i++)
	{
		I3ASSERT( m_pSlot[ i] != NULL);

		m_pSlot[i]->CalculatePos( m_pSlot[i]->GetOriginalPos(), i%3, i/3, &vParentPos, &vSize);
	}

	
}

/*virtual*/ void UIPopupSelectStage::_ClearAtUnload( i3UIScene * pScene)
{
	m_pScrollBar = NULL;

	UIPopupSubBase::_ClearAtUnload( pScene);
}

UIPopupSettingRoom * UIPopupSelectStage::_GetSettingRoomPopup( void)
{
	I3ASSERT( m_pParent != NULL);
	return (UIPopupSettingRoom*) m_pParent;
}

void UIPopupSelectStage::_ClearMapInfo( void)
{
	m_StageCount = 0;
}

/*virtual*/ void UIPopupSelectStage::_OnClose( void)
{
	GetMainFrame()->ClosePopup( UPW_SELECTSTAGE);
}

/*virtual*/ void UIPopupSelectStage::OnCreate( i3GameNode * pParent)
{
	INT32 i;

	UIPopupSubBase::OnCreate( pParent);

	// Load Scene
	AddScene( "UIRe/PointBlankRe_Lobby_CreatRoom_StageSelect.i3UIe", UIPopupSelectStage_Glue);

	for( i = 0; i < MAPSLOT_COUNT; i++)
	{
		m_pSlot[ i] = UISlotStage::NewObject();
		m_pSlot[ i]->OnCreate( this);
	}
}

/*virtual*/ bool UIPopupSelectStage::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupSubBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	m_StageID = *(STAGE_ID*)pArg1;
	m_StageOption = *(STAGE_OPTION_TYPE*) pArg2;

	return true;
}

/*virtual*/ void UIPopupSelectStage::OnEntranceEnd( void)
{
	UIPopupSubBase::OnEntranceEnd();

	InitMapList( getStageType( m_StageID), m_StageOption);
}


/*virtual*/ bool UIPopupSelectStage::OnExitStart( void)
{
	if( UIPopupSubBase::OnExitStart() == false)
		return false;

	INT32 i;

	for( i = 0; i < MAPSLOT_COUNT; i++)
		m_pSlot[ i]->SetVisible( FALSE);

	return true;
}

/*virtual*/ void UIPopupSelectStage::OnExitEnd( void)
{
	UIPopupSubBase::OnExitEnd();

	INT32 i;

	for( i = 0; i < MAPSLOT_COUNT; i++)
		m_pSlot[ i]->SetEnable( FALSE);
}

/*virtual*/ bool UIPopupSelectStage::OnCloseButton( void)
{
	if( this->isEnable())
	{
		_OnClose();
		return false;
	}

	return true;
}

/*virtual*/ void UIPopupSelectStage::OnWheel( i3UIControl * pControl, INT32 zDelta)
{
	if( pControl != NULL &&
		pControl != m_pScrollBar)
	{
		if( zDelta != 0)
		{
			POINT point;

			point.x = 0;
			point.y = 0;

			m_pScrollBar->OnMouseWheel( 0, (short)zDelta, point);

			Scroll();
		}
	}
}

void UIPopupSelectStage::Scroll( void)
{
	m_StartIdx = m_pScrollBar->getScrollPos() * MAPSTAGE_HORIZEN_COUNT;

	UpdateMapList();
}

void UIPopupSelectStage::InitMapList( STAGE_TYPE type, STAGE_OPTION_TYPE option)
{
	INT32 i;
	MAPINFO * pInfo;

	_ClearMapInfo();

	i3List MapList;

	if (g_pGameContext->IsStageMaskTraining())		option = STAGE_OPTION_AI;
	else if (g_pGameContext->IsKnuckleRoom())		option = STAGE_OPTION_KNUCKLE;
	else if (g_pGameContext->IsSniperRoom())		option = STAGE_OPTION_SNIPER;
	else if( g_pGameContext->IsShotGunRoom())		option = STAGE_OPTION_SHOTGUN;
	
	GameUI::GetEnableMapList( &MapList, (STAGE_ID) m_StageID, option);	

	for( i = 0; i < MapList.GetCount(); ++i )
	{
		STAGE_ID stageID = (STAGE_ID)(INT32)MapList.Items[ i];

		pInfo = (MAPINFO*) getStageMapInfo( m_StageCount);

		getStageDisplayName( pInfo->_szName, stageID);

		pInfo->_ImageIdx = g_pFramework->GetMapShapeIdx( stageID);

		if( TESTBIT(getNewMarkStageTable( getStageOrd( stageID)) , GOODS_MARK_NEW))
			pInfo->_bNew = true;
		else
			pInfo->_bNew = false;

		pInfo->_StageID = stageID;

		m_StageCount++;
		I3_BOUNDCHK( m_StageCount, STAGE_ORD_MAX);
	}

	if( m_StageCount > MAPSLOT_COUNT)
	{
		m_bScroll = true;
		m_pScrollBar->SetEnable( TRUE);
		m_pScrollBar->setPageSize(3);
		m_pScrollBar->setScrollRange( 0, (m_StageCount+2)/3);
	}
	else
	{
		m_bScroll = false;
		m_pScrollBar->SetEnable( FALSE);
	}

	m_StartIdx = 0;

	LuaState * L = _CallLuaFunction( "SetRandomMapCount");
	i3Lua::PushInteger(L , m_SelectRandomMapList.GetCount());
	i3Lua::PushInteger(L , m_StageCount);
	_EndLuaFunction( L, 2);

	UpdateMapList();
}


void UIPopupSelectStage::UpdateMapList( void)
{
	INT32 i;
	INT32 idx;

	for( i = 0; i < MAPSLOT_COUNT; i++)
	{
		idx = m_StartIdx + i;

		m_pSlot[ i]->EnableSlot( i%3, i/3, NULL, NULL);

		if( idx < m_StageCount)
		{
			MAPINFO * pInfo = getStageMapInfo( idx);

			// X-Mas Mark
			bool bXMasEnable = false;

			if(TESTBIT(getNewMarkStageTable( getStageOrd( pInfo->_StageID)) , GOODS_MARK_XMAS))
				bXMasEnable = true;
		
			m_pSlot[ i]->SetInfo( pInfo->_szName, pInfo->_ImageIdx, pInfo->_bNew, bXMasEnable);
			

			if( pInfo->_StageID == m_StageID)
			{
				m_pSlot[ i]->OnSelect();
				m_CurrentSlot = i;
			}
		}
		else
		{
			m_pSlot[ i]->SetEmptySlot();
		}
	}

	if(m_bRandomMap)
	{
		for( i = 0; i < MAPSLOT_COUNT; i++)
		{
			if(m_pSlot[i]->IsEmpty())
				continue;

			idx = m_StartIdx + i;
			INT32 ord = getStageOrd(GetStageID(idx));
			
			if( m_SelectRandomMapList.IndexOf((void*)ord) >= 0)
				m_pSlot[ i]->OnSelect();
			else
				m_pSlot[ i]->OnUnselect();

		}
	}
}

INT32 UIPopupSelectStage::FindSlot( LuaState * L)
{
	INT32 i;

	for( i = 0; i < MAPSLOT_COUNT; i++)
	{
		if( m_pSlot[i])
		{
			i3UIScene * pScene = m_pSlot[i]->GetScene(0);
			I3ASSERT( pScene);

			if( pScene->getLuaContext() == L)
				return i;
		}
	}

	return -1;
}

void UIPopupSelectStage::SelectSlot( INT32 slot)
{
	if(slot == -1)
		return;

	if(m_pSlot[slot]->IsEmpty())
		return;


	INT32 idx;

	if(m_bRandomMap)
	{
		idx = slot + m_StartIdx;
		INT32 ord = getStageOrd(GetStageID(idx));

		if(m_pSlot[slot]->IsSelect())
		{
			m_pSlot[ slot]->OnUnselect();
			m_SelectRandomMapList.Remove((void*)ord);
		}
		else
		{
			m_pSlot[ slot]->OnSelect();
			m_SelectRandomMapList.Add((void*)ord);
		}

		LuaState * L = _CallLuaFunction( "SetRandomMapCount");
		i3Lua::PushInteger(L , m_SelectRandomMapList.GetCount());
		i3Lua::PushInteger(L , m_StageCount);
		_EndLuaFunction( L, 2);

		return;
	}

	if( m_CurrentSlot != -1)
	{
		m_pSlot[ m_CurrentSlot]->OnUnselect();
	}

	m_pSlot[ slot]->OnSelect();

	m_CurrentSlot = slot;

	idx = m_CurrentSlot + m_StartIdx;
	m_StageID = GetStageID(idx);
}

void UIPopupSelectStage::SelectOK( void)
{
	if(m_bRandomMap && m_SelectRandomMapList.GetCount() < 1)
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_STRING("STR_POPUP_RANDOMMAP_CHECK_COUNT"));
		return;
	}

	g_pGameContext->ClearRandomMapIndexList();

	for(INT32 idx = 0 ; idx < m_SelectRandomMapList.GetCount() ; idx ++)
	{
		INT32 CheckMap = (INT32)m_SelectRandomMapList.GetItem(idx);
		g_pGameContext->AddRandomMapIndex(CheckMap);
	}

	GetSettingRoom()->setRandomMap(m_bRandomMap);

	GetSettingRoom()->SelectStageOK(m_StageID);
}

void UIPopupSelectStage::ClickRandomMap( void)
{
	LuaState * L = _CallLuaFunction( "isRandomMap");
	m_bRandomMap = !_ReturnLuaBoolean( L, 0);
	
	m_SelectRandomMapList.Clear();

	if(!m_bRandomMap)
	{
		for(INT32 idx = 0 ; idx < MAPSLOT_COUNT ; idx++)
		{
			INT32 MapIndex = m_StartIdx + idx ;

			if(GetStageID(MapIndex) == m_StageID)
			{
				m_pSlot[idx]->OnSelect();
			}
			else
				m_pSlot[idx]->OnUnselect();
		}
	}
	else
	{
		for(INT32 idx = 0 ; idx < m_StageCount ; idx++)
		{
			STAGE_ID StageID = GetStageID(idx);
			INT32 ord = getStageOrd(StageID);
			m_SelectRandomMapList.Add((void*)ord);
		}
	}

	L = _CallLuaFunction( "SetRandomMapCount");
	i3Lua::PushInteger(L , m_SelectRandomMapList.GetCount());
	i3Lua::PushInteger(L , m_StageCount);
	_EndLuaFunction( L, 2);

	UpdateMapList();
}

void	UIPopupSelectStage::SetInputDisable( bool bDisable)
{

	for (INT32 i = 0 ; i < MAPSLOT_COUNT ; ++i )
	{
		if (m_pSlot[i])
			m_pSlot[i]->SetInputDisable(bDisable);
	}

	UIPopupSubBase::SetInputDisable(bDisable);

}
