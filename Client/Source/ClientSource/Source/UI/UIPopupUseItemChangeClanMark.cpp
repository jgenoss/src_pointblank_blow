#include "pch.h"
#include "UIPopupUseItemChangeClanMark.h"

#include "UIMainFrame.h"
#include "UIUtil.h"

#include "UISignal.h"


extern "C" {
	int UIPopupUseItemChangeClanMark_ChangeBGPrev( LuaState * L)
	{
		UIPopupUseItemChangeClanMark * pThis = static_cast<UIPopupUseItemChangeClanMark*>(GameUI::GetPopup( UPW_CHANGE_CLAN_MARK));
		if( pThis != nullptr)
			pThis->ChangePrevBGImage();
		return 0;
	}

	int UIPopupUseItemChangeClanMark_ChangeBGNext( LuaState * L)
	{
		UIPopupUseItemChangeClanMark * pThis = static_cast<UIPopupUseItemChangeClanMark*>(GameUI::GetPopup( UPW_CHANGE_CLAN_MARK));
		if( pThis != nullptr)
			pThis->ChangeNextBGImage();
		return 0;
	}

	int UIPopupUseItemChangeClanMark_ChangeShapePrev( LuaState * L)
	{
		UIPopupUseItemChangeClanMark * pThis = static_cast<UIPopupUseItemChangeClanMark*>(GameUI::GetPopup( UPW_CHANGE_CLAN_MARK));
		if( pThis != nullptr)
			pThis->ChangePrevShapeImage();
		return 0;
	}

	int UIPopupUseItemChangeClanMark_ChangeShapeNext( LuaState * L)
	{
		UIPopupUseItemChangeClanMark * pThis = static_cast<UIPopupUseItemChangeClanMark*>(GameUI::GetPopup( UPW_CHANGE_CLAN_MARK));
		if( pThis != nullptr)
			pThis->ChangeNextShapeImage();
		return 0;
	}

	int UIPopupUseItemChagneClanMark_ChangeBackImageColor( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPopupUseItemChangeClanMark * pThis = static_cast<UIPopupUseItemChangeClanMark*>(GameUI::GetPopup( UPW_CHANGE_CLAN_MARK));
		if( pThis != nullptr)
			pThis->ChangeBackImageColor( idx + 1);
		return 0;
	}

	int UIPopupUseItemChangeClanMark_ChangeShapeColor( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPopupUseItemChangeClanMark * pThis = static_cast<UIPopupUseItemChangeClanMark*>(GameUI::GetPopup( UPW_CHANGE_CLAN_MARK));
		if( pThis != nullptr)
			pThis->ChangeShapeColor( idx + 1);
		return 0;
	}

	int UIPopupUseItemChangeClnaMark_DuplicateMark( LuaState * L)
	{
		UIPopupUseItemChangeClanMark * pThis = static_cast<UIPopupUseItemChangeClanMark*>(GameUI::GetPopup( UPW_CHANGE_CLAN_MARK));
		if( pThis != nullptr)
			pThis->CheckDuplicateMark();
		return 0;
	}
}

LuaFuncReg UIPopupUseItemChangeClanMark_Glue[] = {
	{"ClickBGLeft",				UIPopupUseItemChangeClanMark_ChangeBGPrev},
	{"ClickBGRight",			UIPopupUseItemChangeClanMark_ChangeBGNext},
	{"ClickShapeLeft",			UIPopupUseItemChangeClanMark_ChangeShapePrev},
	{"ClickShapeRight",			UIPopupUseItemChangeClanMark_ChangeShapeNext},
	{"ClickBGColor",			UIPopupUseItemChagneClanMark_ChangeBackImageColor},
	{"ClickShapeColor",			UIPopupUseItemChangeClanMark_ChangeShapeColor},
	{"ClickDuplicate",			UIPopupUseItemChangeClnaMark_DuplicateMark},
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIPopupUseItemChangeClanMark);//, UIPopupUseItemBase);

UIPopupUseItemChangeClanMark::UIPopupUseItemChangeClanMark()
{
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;

	i3mem::FillZero(m_pTexClanMarkSelect, sizeof(m_pTexClanMarkSelect)); 
	i3mem::FillZero(m_pClanMarkIcon, sizeof(m_pClanMarkIcon)); 

	m_pFinalMark = 0;
	m_pTexClanMarkFinal = nullptr;

	m_nBackImageIdx = 1;
	m_nShapeImageIdx = 1;
	m_nBackColorIdx = 1;
	m_nShapeColorIdx = 1;

	m_bWaitingForCheck = false;

	RegisterGameEventFunc( EVENT_CLAN_DUPLICATE_MARK,		&UIPopupUseItemChangeClanMark::ReceiveGameEvent_Clan_Duplicate_Mark);
	RegisterGameEventFunc( EVENT_CLAN_CHANGE_MARK,			&UIPopupUseItemChangeClanMark::ReceiveGameEvent_Clan_Change_Mark);
}

UIPopupUseItemChangeClanMark::~UIPopupUseItemChangeClanMark()
{
}

/*virtual*/ void UIPopupUseItemChangeClanMark::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupUseItemBase::_InitializeAtLoad( pScene);

	INT32 i;
	char szName[ MAX_PATH];

	// ImageEx를 찾자.!!
	for( i = 0; i < UI_CHANGE_CLANMARK_SHAPE_SLOT_MAX; i++)
	{
		i3::snprintf( szName, sizeof( szName), "bgIcon%d", i);
		m_pClanMarkIcon[0][i] = (i3UIImageBoxEx*) pScene->FindChildByName( szName);
		I3ASSERT( m_pClanMarkIcon[0][i]);

		i3::snprintf( szName, sizeof( szName), "shapeIcon%d", i);
		m_pClanMarkIcon[1][i] = (i3UIImageBoxEx*) pScene->FindChildByName( szName);
		I3ASSERT( m_pClanMarkIcon[1][i]);
	}

	m_pFinalMark = (i3UIImageBoxEx*) pScene->FindChildByName( "finalMark");
	I3ASSERT( m_pFinalMark);
}

/*virtual*/ LuaState * UIPopupUseItemChangeClanMark::_SetItemInfo( void)
{
	return UIPopupUseItemBase::_SetItemInfo();
}

void UIPopupUseItemChangeClanMark::_CleanupTexture( void)
{
	for(INT32 i = 0; i < MAX_CLAN_MARK_LAYER; i++)
	{
		for(INT32 j = 0; j < UI_CHANGE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			I3_SAFE_RELEASE( m_pTexClanMarkSelect[i][j]);
		}
	}

	I3_SAFE_RELEASE( m_pTexClanMarkFinal);
}

void UIPopupUseItemChangeClanMark::_ClanMarkUserSelectImageUpdate( CLAN_MARK_LAYER_TYPE layerType)
{
	INT32 i;
	INT32 nImageIdx = 1;
	INT32 nColorIdx = 1;
	INT32 nImageMax = 1;

	//	관련된 클랜 마크 조각 슬롯 이미지 업데이트
	for( i = 0; i < UI_CHANGE_CLANMARK_SHAPE_SLOT_MAX; i++)
	{
		switch( layerType)
		{
		case CLAN_MARK_LAYER_FRAME :			//	배경 마크
			nImageIdx = m_nBackImageIdx;
			nColorIdx = m_nBackColorIdx;
			nImageMax = CClanMarkManager::GetMarkBackImageCount();

			I3_BOUNDCHK( nImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
			I3_BOUNDCHK( nColorIdx, CClanMarkManager::GetMarkColorCount()+1);
			break;
		case CLAN_MARK_LAYER_CONTENT :	//	모양 마크	
			nImageIdx = m_nShapeImageIdx;
			nColorIdx = m_nShapeColorIdx;
			nImageMax = CClanMarkManager::GetMarkShapeImageCount();

			I3_BOUNDCHK( nImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
			I3_BOUNDCHK( nColorIdx, CClanMarkManager::GetMarkColorCount()+1);
			break;
		}

		//	가운데 3번째(idx 0) 슬롯을 중심으로 좌우 슬롯은 무한 루프 되도록 인덱스를 정한다.
		switch( i)
		{
		case 0:		nImageIdx -= 2;		if( nImageIdx <= 0)	nImageIdx += nImageMax;		break;
		case 1:		nImageIdx -= 1;		if( nImageIdx <= 0)	nImageIdx += nImageMax;		break;
		case 2:		nImageIdx += 0;		break;
		case 3:		nImageIdx += 1;		if( nImageIdx > nImageMax)	nImageIdx -= nImageMax;		break;
		case 4:		nImageIdx += 2;		if( nImageIdx > nImageMax)	nImageIdx -= nImageMax;		break;
	//	default:	continue;
		}

		switch( layerType)
		{
		case CLAN_MARK_LAYER_FRAME :			//	배경 마크
			I3_BOUNDCHK( nImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
			I3_BOUNDCHK( nColorIdx, CClanMarkManager::GetMarkColorCount()+1);
			break;

		case CLAN_MARK_LAYER_CONTENT :	//	모양 마크
			I3_BOUNDCHK( nImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
			I3_BOUNDCHK( nColorIdx, CClanMarkManager::GetMarkColorCount()+1);
			break;
		}

		//	하나의 슬롯의 마크 조각 이미지를 변경합니다.
		_ChangeSelectedImage( layerType, i, nImageIdx, nColorIdx); 
	}

	//	클랜 마크 조각 이미지가 변경됬으므로 최종 조합 마크 이미지도 함께 변경
	_ClanMarkUserFinalImage();
}

void UIPopupUseItemChangeClanMark::_ChangeSelectedImage( CLAN_MARK_LAYER_TYPE LayerType, INT32 nSlotIdx, INT32 nImageIdx, INT32 nColorIdx)
{
	I3_BOUNDCHK( LayerType, MAX_CLAN_MARK_LAYER);
	I3_BOUNDCHK( nSlotIdx, UI_CHANGE_CLANMARK_SHAPE_SLOT_MAX);

	i3Texture * pTex = m_pTexClanMarkSelect[LayerType][nSlotIdx];
	I3ASSERT(pTex);

	if( LayerType == CLAN_MARK_LAYER_FRAME)			//	배경 마크
	{
		::MakeClanMarkShapeTex( pTex, CLAN_MARK_MEDIUM, CLAN_MARK_LAYER_FRAME, nImageIdx, nColorIdx);
	}
	else if( LayerType == CLAN_MARK_LAYER_CONTENT)	//	모양 마크
	{
		::MakeClanMarkShapeTex( pTex, CLAN_MARK_MEDIUM, CLAN_MARK_LAYER_CONTENT, nImageIdx, nColorIdx);
	}
}

void UIPopupUseItemChangeClanMark::_ClanMarkUserFinalImage( void)
{
	::MakeClanMarkTex( m_pTexClanMarkFinal, CLAN_MARK_MEDIUM, m_nBackImageIdx, m_nShapeImageIdx, m_nBackColorIdx, m_nShapeColorIdx);
}

void UIPopupUseItemChangeClanMark::_SetOKButtonActivateState( bool bActivate)
{
	LuaState * L = _CallLuaFunction( "SetOKButtonActivateState");
	if( L != nullptr)
	{
		i3Lua::PushBoolean( L, bActivate);
		_EndLuaFunction( L, 1);
	}
}

/*virtual*/ void UIPopupUseItemChangeClanMark::OnCreate( i3GameNode * pParent)
{
	UIPopupUseItemBase::OnCreate( pParent);
	
	AddScene("Scene/Popup/PBRe_PopUp_ClanMark_Change.i3UIs", UIPopupUseItemChangeClanMark_Glue);
}

/*virtual*/ void UIPopupUseItemChangeClanMark::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	UIPopupUseItemBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

/*virtual*/ bool UIPopupUseItemChangeClanMark::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	INT32 i, j;

	if( UIPopupUseItemBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	UINT32 nOldFlag = MAKE_CLANMARK_FLAG( 1, 1, 1, 1);
	if( pArg2 != nullptr)
		nOldFlag = *static_cast<UINT32*>(pArg2);

	//	현재 클랜 마크 값을 가져온다.
	m_nBackImageIdx = CLANMARK_FRAME_IMAGE_IDX( nOldFlag);
	m_nShapeImageIdx = CLANMARK_CONTENT_IMAGE_IDX( nOldFlag);
	m_nBackColorIdx = CLANMARK_FRAME_COLOR_IDX( nOldFlag);
	m_nShapeColorIdx = CLANMARK_CONTENT_COLOR_IDX( nOldFlag);

	//	default 1 base
	if( m_nBackImageIdx <= 0)	m_nBackImageIdx = 1;
	if( m_nShapeImageIdx <= 0)	m_nShapeImageIdx = 1;
	if( m_nBackColorIdx <= 0)	m_nBackColorIdx  = 1;
	if( m_nShapeColorIdx <= 0)	m_nShapeColorIdx = 1;

	_CleanupTexture();

	//	선택용 클랜 마크 조합 이미지를 미리 생성
	for( i=0; i<MAX_CLAN_MARK_LAYER; i++)
	{
		for( j=0; j<UI_CHANGE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			//	텍스처 생성
			m_pTexClanMarkSelect[i][j] = i3Texture::new_object(); 
			I3ASSERT( m_pTexClanMarkSelect[i][j] != nullptr);
#if defined( I3_DEBUG)
			m_pTexClanMarkSelect[i][j]->SetName( "ClanMarkLayer");
#endif
			m_pTexClanMarkSelect[i][j]->Create(64, 64, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);

			I3ASSERT( m_pClanMarkIcon[i][j] != nullptr);
			m_pClanMarkIcon[i][j]->AddTexture( m_pTexClanMarkSelect[i][j], CLAN_MARK_MID_PIXEL, CLAN_MARK_MID_PIXEL);
			m_pClanMarkIcon[i][j]->CreateBaseSceneObjects();
			m_pClanMarkIcon[i][j]->SetCurrentImage(0);
		}
	}
	
	// 조합 완료 마크 텍스처 생성
	REAL32 rSize = CLAN_MARK_MID_PIXEL;

	m_pTexClanMarkFinal = i3Texture::new_object(); 
	I3ASSERT( m_pTexClanMarkFinal != nullptr);
#if defined( I3_DEBUG)
	m_pTexClanMarkFinal->SetName( "ClanMarkMerged");
#endif
	m_pTexClanMarkFinal->Create( 64, 64, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);

	m_pFinalMark->AddTexture( m_pTexClanMarkFinal, rSize, rSize);
	m_pFinalMark->CreateBaseSceneObjects();
	m_pFinalMark->SetCurrentImage(0);
	m_pFinalMark->SetShapeSize( rSize, rSize);

	// 중심점 다시 잡기
	i3UIControl * pParent = (i3UIControl*) m_pFinalMark->getParent();
	REAL32 w = pParent->getWidth();
	REAL32 h = pParent->getHeight();

	REAL32 newPosX = (w - rSize) * 0.5f;
	REAL32 newPosY = (h - rSize) * 0.5f;

	m_pFinalMark->setPos( newPosX, newPosY);

	ChangeBackImageColor( m_nBackColorIdx);
	ChangeShapeColor( m_nShapeColorIdx);

	for( i = 0; i < MAX_CLAN_MARK_LAYER; i++)
	{
		_ClanMarkUserSelectImageUpdate( (CLAN_MARK_LAYER_TYPE) i);
	}

	m_bWaitingForCheck = false;

	_SetOKButtonActivateState( false);

	return true;
}

/*virtual*/ void UIPopupUseItemChangeClanMark::OnEntranceEnd( void)
{
	UIPopupUseItemBase::OnEntranceEnd();

	INT32 i, j;
	for( i = 0; i < MAX_CLAN_MARK_LAYER; i++)
	{
		for( j = 0; j < UI_CHANGE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			i3UIImageBoxEx * pImgBox = m_pClanMarkIcon[ i][ j];
			I3ASSERT( pImgBox != nullptr);
			pImgBox->EnableCtrl( true);
		}
	}
	
	//m_pFinalMark->SetShapeSize( m_pFinalMark->getWidth(), m_pFinalMark->getHeight());
	m_pFinalMark->EnableCtrl( true);
}

/*virtual*/ bool UIPopupUseItemChangeClanMark::OnExitStart( void)
{
	if( UIPopupUseItemBase::OnExitStart() == false)
		return false;

	_CleanupTexture();

	i3mem::FillZero( m_pClanMarkIcon, sizeof(m_pClanMarkIcon) );
	m_pFinalMark = nullptr;

	return true;
}


/*virtual*/ bool UIPopupUseItemChangeClanMark::OnOKButton( void)
{
	bool is_ok_btn_activate = false;

	LuaState * L = _CallLuaFunction("IsOKButtonActivate");
	if( L != nullptr )
		is_ok_btn_activate = _ReturnLuaBoolean(L , 0);

	if( is_ok_btn_activate == false )
	{
		CheckDuplicateMark();
		return false;
	}

	I3_BOUNDCHK( m_nBackImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
	I3_BOUNDCHK( m_nShapeImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
	I3_BOUNDCHK( m_nBackColorIdx, CClanMarkManager::GetMarkColorCount()+1);
	I3_BOUNDCHK( m_nShapeColorIdx, CClanMarkManager::GetMarkColorCount()+1);

	UINT32 nFlag = MAKE_CLANMARK_FLAG( m_nBackImageIdx, m_nShapeImageIdx, m_nBackColorIdx, m_nShapeColorIdx);

	I3ASSERT( g_pFramework != nullptr);
	g_pFramework->GetClanContext()->ProcessChangeClanMark( m_AuthWareDBIndex, nFlag);

	return true;
}

void UIPopupUseItemChangeClanMark::ChangeBackImageColor( INT32 colorIdx)
{
	if( colorIdx < CClanMarkManager::GetMarkColorCount()+1 &&
		m_nBackColorIdx != colorIdx )
	{
		m_nBackColorIdx = colorIdx;

		_ClanMarkUserSelectImageUpdate();

		_SetOKButtonActivateState( false);
	}
}

void UIPopupUseItemChangeClanMark::ChangeShapeColor( INT32 colorIdx)
{
	if( colorIdx < CClanMarkManager::GetMarkColorCount()+1 &&
		m_nShapeColorIdx != colorIdx )
	{
		m_nShapeColorIdx = colorIdx;

		_ClanMarkUserSelectImageUpdate( CLAN_MARK_LAYER_CONTENT);

		_SetOKButtonActivateState( false);
	}
}

void UIPopupUseItemChangeClanMark::ChangePrevBGImage( void)
{
	m_nBackImageIdx--;

	if( m_nBackImageIdx <= 0)
		m_nBackImageIdx = CClanMarkManager::GetMarkBackImageCount();

	//	이전 마크 배경 이미지로 업데이트
	_ClanMarkUserSelectImageUpdate();

	_SetOKButtonActivateState( false);
}

void UIPopupUseItemChangeClanMark::ChangeNextBGImage( void)
{
	m_nBackImageIdx++;

	if( m_nBackImageIdx > CClanMarkManager::GetMarkBackImageCount())
		m_nBackImageIdx = 1;

	//	다음 마크 배경 이미지로 업데이트
	_ClanMarkUserSelectImageUpdate();

	_SetOKButtonActivateState( false);
}


void UIPopupUseItemChangeClanMark::ChangePrevShapeImage( void)
{
	m_nShapeImageIdx--;

	if( m_nShapeImageIdx <= 0)
		m_nShapeImageIdx = CClanMarkManager::GetMarkShapeImageCount();

	//	이전 마크 모양 이미지로 업데이트
	_ClanMarkUserSelectImageUpdate( CLAN_MARK_LAYER_CONTENT);

	_SetOKButtonActivateState( false);
}

void UIPopupUseItemChangeClanMark::ChangeNextShapeImage( void)
{
	m_nShapeImageIdx++;

	if( m_nShapeImageIdx > CClanMarkManager::GetMarkShapeImageCount())
		m_nShapeImageIdx = 1;

	//	다음 마크 모양 이미지로 업데이트
	_ClanMarkUserSelectImageUpdate( CLAN_MARK_LAYER_CONTENT);

	_SetOKButtonActivateState( false);
}

void UIPopupUseItemChangeClanMark::CheckDuplicateMark( void)
{
	m_bWaitingForCheck = false;
	//	1 base (0 is default)
	if( m_bWaitingForCheck == false)
	{
		I3_BOUNDCHK( m_nBackImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
		I3_BOUNDCHK( m_nShapeImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
		I3_BOUNDCHK( m_nBackColorIdx, CClanMarkManager::GetMarkColorCount()+1);
		I3_BOUNDCHK( m_nShapeColorIdx, CClanMarkManager::GetMarkColorCount()+1);

		UINT32 nFlag = MAKE_CLANMARK_FLAG( m_nBackImageIdx, m_nShapeImageIdx, m_nBackColorIdx, m_nShapeColorIdx);

		I3ASSERT( g_pFramework != nullptr);
		g_pFramework->GetClanContext()->SendOverlappingClanMark(nFlag);

		m_bWaitingForCheck = true;
	}
}

void UIPopupUseItemChangeClanMark::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPopupUseItemChangeClanMark::ReceiveGameEvent_Clan_Duplicate_Mark( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_POSSIBLE_MARK"),	//	"사용 가능한 클랜 마크입니다."
						&GAME_RCSTRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK"));							//	"클랜 마크 변경"

		_SetOKButtonActivateState( true);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_IMPOSSIBLE_MARK"),	//	"사용할 수 없는 클랜 마크입니다."
					&GAME_RCSTRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK"));								//	"클랜 마크 변경"

		_SetOKButtonActivateState( false);
	}

	m_bWaitingForCheck = false;
}

void UIPopupUseItemChangeClanMark::ReceiveGameEvent_Clan_Change_Mark( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
	//	UI::call_cb( "update_slot9", (void*) true, (void*) true); //상점/인벤 슬롯 상태 변경
		UISignal::UpdateSlot9(true, true);	//상점/인벤 슬롯 상태 변경

		//GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMPLETE_CHANGE"),	//	"클랜 마크 변경을 완료하였습니다."
		//	&GAME_RCSTRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK"));										//	"클랜 마크 변경"

		// 팝업창을 닫는다.
		GetMainFrame()->ClosePopup( UPW_CHANGE_CLAN_MARK);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_IMPOSSIBLE_MARK"),	//	"사용할 수 없는 클랜 마크입니다."
			&GAME_RCSTRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK"));										//	"클랜 마크 변경"

		_SetOKButtonActivateState( false);
	}
}
