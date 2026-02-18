#include "pch.h"

#if legacy_gui_scene
#include "UIPopup_Clan_ChangeMark_Old.h"

#include "UIMainFrame.h"
#include "UIUtil.h"

#include "GuiNotifyReceiver.h"
#include "ClanMark.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( UIPopup_Clan_ChangeMark_Old);//, UIPopupClanBase);

UIPopup_Clan_ChangeMark_Old::UIPopup_Clan_ChangeMark_Old(void)
{
	m_pGui = nullptr;
	m_pCaption = nullptr;

	m_pSelectStatic = nullptr;
	m_pBackColorStatic = nullptr;
	m_pShapeColorStatic = nullptr;

	m_pButtonOk = nullptr;
	m_pButtonCancel = nullptr;
	m_pButtonOverlappingCheck = nullptr;

	m_pClanMarkConfirmImage = nullptr;

	i3mem::FillZero(m_pClanMarkPrev, sizeof(m_pClanMarkPrev));
	i3mem::FillZero(m_pClanMarkNext, sizeof(m_pClanMarkNext));


	for(INT32 i=0; i<CHANGE_CLANMARK_COMBINATION_MAX; i++)
		for(INT32 j=0; j<CHANGE_CLANMARK_SHAPE_SLOT_MAX; j++)
			m_pClanMarkSelectImage[i][j] = nullptr;

	for(INT32 i=0; i<CHANGE_CLANMARK_COLOR_SLOT_MAX; i++)
	{
		m_pBackColorBox[i] = nullptr;
		m_pShapeColorBox[i] = nullptr;
	}

	for(INT32 i=0; i<CHANGE_CLANMARK_COMBINATION_MAX; i++)
	{
		for(INT32 j=0; j<CHANGE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			m_pTexClanMarkSelect[i][j] = nullptr;
		}
	}

	m_pTexClanMarkFinal = nullptr;

	m_nBackImageIdx = 1;
	m_nShapeImageIdx = 1;
	m_nBackColorIdx = 1;
	m_nShapeColorIdx = 1;

	m_bWaitingForCheck = false;
}

UIPopup_Clan_ChangeMark_Old::~UIPopup_Clan_ChangeMark_Old(void)
{
	_CleanupTexture();
}

void UIPopup_Clan_ChangeMark_Old::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_CLAN_B_POPUP_CHANGE_CLANMARK_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnOK();
		break;
	case GCI_CLAN_B_POPUP_CHANGE_CLANMARK_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCancel();
		break;
	case GCI_CLAN_B_POPUP_CHANGE_CLANMARK_OVERLAPPING_CHECK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnOverlappingCheck();
		break;
	case GCI_CLAN_B_POPUP_CHANGE_CLANMARK_GROUP_PREV1:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnPrevBackImage();
		break;
	case GCI_CLAN_B_POPUP_CHANGE_CLANMARK_GROUP_PREV2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnPrevShapeImage( );
		break;
	case GCI_CLAN_B_POPUP_CHANGE_CLANMARK_GROUP_NEXT1:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNextBackImage( );
		break;
	case GCI_CLAN_B_POPUP_CHANGE_CLANMARK_GROUP_NEXT2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNextShapeImage( );
		break;
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR1:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR2:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR3:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR4:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR5:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR6:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR7:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR8:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR9:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnChangeBackColor( pNotify->m_nID - GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR1 + 1);
		break;
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR1:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR2:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR3:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR4:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR5:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR6:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR7:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR8:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR9:
	case GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnChangeShapeColor( pNotify->m_nID - GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR1 + 1);
		break;
	}
}

bool UIPopup_Clan_ChangeMark_Old::OnLinkControl(const i3::vector_set<STRCINFO*>* plist, i3GameNode * pControl)
{
	LinkControlEx(*plist,  "ChangeClanMark", m_pGui, GCI_UNKNOWN);
	{
		LinkControlEx(*plist,  "changeClanMarkPopupCaption", m_pCaption, GCI_UNKNOWN);

		LinkControlEx(*plist,  "changeClanMarkSelectStatic", m_pSelectStatic, GCI_UNKNOWN);
		LinkControlEx(*plist,  "changeClanMarkBackColorStatic", m_pBackColorStatic, GCI_UNKNOWN);
		LinkControlEx(*plist,  "changeClanMarkShapeColorStatic", m_pShapeColorStatic, GCI_UNKNOWN);

		LinkControlEx(*plist,  "changeClanMarkOk", m_pButtonOk, GCI_CLAN_B_POPUP_CHANGE_CLANMARK_OK);
		LinkControlEx(*plist,  "changeClanMarkCancel", m_pButtonCancel, GCI_CLAN_B_POPUP_CHANGE_CLANMARK_CANCEL);
		LinkControlEx(*plist,  "changeClanMarkOverlappingCheck", m_pButtonOverlappingCheck, GCI_CLAN_B_POPUP_CHANGE_CLANMARK_OVERLAPPING_CHECK);

		LinkControlEx(*plist,  "changeClanMarkConfirmImage", m_pClanMarkConfirmImage, GCI_UNKNOWN);

		char temp[256] = "";
		for(INT32 i=0; i<2; i++)
		{
			sprintf_s( temp, "changeClanMarkPrev%d", i+1);
			LinkControlEx(*plist,  temp, m_pClanMarkPrev[i], GCI_CLAN_B_POPUP_CHANGE_CLANMARK_GROUP_PREV1 + i);

			sprintf_s( temp, "changeClanMarkNext%d", i+1);
			LinkControlEx(*plist,  temp, m_pClanMarkNext[i], GCI_CLAN_B_POPUP_CHANGE_CLANMARK_GROUP_NEXT1 + i);

			for(INT32 j=0; j<5; j++)
			{
				sprintf_s( temp, "changeClanMarkSelectImage%d%d", i+1, j+1);
				LinkControlEx(*plist,  temp, m_pClanMarkSelectImage[i][j], GCI_UNKNOWN);
			}
		}

		LinkControlEx(*plist,  "changeClanMarkBackColorStatic", m_pBackColorStatic, GCI_UNKNOWN);
		LinkControlEx(*plist,  "changeClanMarkBackColorSelected", m_pBackColorSelected, GCI_UNKNOWN);
		for(INT32 i=0; i<10; i++)
		{
			sprintf_s(temp, "changeClanMarkBackColorBox%d", i+1);
			LinkControlEx(*plist,  temp, m_pBackColorBox[i], GCI_CLAN_S_POPUP_CHANGE_CLANMARK_BACK_COLOR1 + i);
		}

		LinkControlEx(*plist,  "changeClanMarkShapeColorStatic", m_pShapeColorStatic, GCI_UNKNOWN);
		LinkControlEx(*plist,  "changeClanMarkShapeColorSelected", m_pShapeColorSelected, GCI_UNKNOWN);
		for(INT32 i=0; i<10; i++)
		{
			sprintf_s(temp, "changeClanMarkShapeColorBox%d", i+1);
			LinkControlEx(*plist,  temp, m_pShapeColorBox[i], GCI_CLAN_S_POPUP_CHANGE_CLANMARK_SHAPE_COLOR1 + i);
		}
	}

	return false;
}

void UIPopup_Clan_ChangeMark_Old::OnInitControl(void)
{
	INIT_WIN_CAPTION( m_pCaption, GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_CHANGE_CLAN_MARK"));	//	"클랜 마크 변경"

	INIT_NORMAL_CAPTION(m_pSelectStatic, GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SELECT_MARK"));	//"클랜 표시 SELECT

	INIT_CAPTION_BUTTON(m_pButtonOk, GAME_RCSTRING("STBL_IDX_CLAN_BUTTON_OK"));			//	"확인"
	INIT_CAPTION_BUTTON(m_pButtonCancel, GAME_RCSTRING("STBL_IDX_CLAN_BUTTON_CANCEL"));	//	"취소
	INIT_CAPTION_BUTTON(m_pButtonOverlappingCheck, GAME_RCSTRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));	//	"중복검사"

	INIT_STATIC_CAPTION(m_pBackColorStatic, GAME_RCSTRING("STBL_IDX_CLAN_BACK_COLOR"));		//	"바탕 색상"
	INIT_STATIC_CAPTION(m_pShapeColorStatic, GAME_RCSTRING("STBL_IDX_CLAN_SHAPE_COLOR"));		//	"모양 색상"

	for(INT32 i=0; i<CHANGE_CLANMARK_COMBINATION_MAX; i++)
	{
		INIT_BUTTON_EX( m_pClanMarkPrev[i]);
		INIT_BUTTON_EX( m_pClanMarkNext[i]);
	}

	for(INT32 i=0; i<CHANGE_CLANMARK_COLOR_SLOT_MAX; i++)
	{
		m_pBackColorBox[i]->setInputDisable(false);
		m_pBackColorBox[i]->SetAllShapeEnable(false);
		m_pBackColorBox[i]->SetShapeEnable(i, true);

		m_pShapeColorBox[i]->setInputDisable(false);
		m_pShapeColorBox[i]->SetAllShapeEnable(false);
		m_pShapeColorBox[i]->SetShapeEnable(i, true);
	}

	m_pGui->SetEnable(false);
}

//	pArg1 = 아이템 인증 flag,  pArg2 = 현재 클랜마크 flag
bool UIPopup_Clan_ChangeMark_Old::OnEntranceStart(void * pArg1, void * pArg2)
{
	if( pArg1 != nullptr)
	{
		m_nAuthFlag = *static_cast<INT64*>(pArg1);
	}
	else
	{
		//	"아이템을 사용할 수 없습니다.\n잘못된 아이템입니다."
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_WRONG_ITEM"));

		return false;
	}	

	if( UIPopupClanBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	UINT32 nOldFlag = MAKE_CLANMARK_FLAG( 1, 1, 1, 1);
	if( pArg2 != nullptr)
	{
		nOldFlag = *static_cast<UINT32*>(pArg2);
	}

	m_pGui->SetEnable(true);

	// _OnPopupClanCreate()안에서 GameUI::MsgBox( MSG_TYPE_GAME_OK, )호출 후 return 시,
	// 배경 GUI의 입력을 다시 풀어야하기때문에 먼저 막습니다.
	m_pGui->setInputDisable(false);

	_InitPopup(nOldFlag);

	return true;
}

bool UIPopup_Clan_ChangeMark_Old::OnExitStart()
{
	if( UIPopupClanBase::OnExitStart() == false)
		return false;

	m_pGui->SetEnable(false);

	_CleanupTexture();

	return true;
}

void UIPopup_Clan_ChangeMark_Old::OnUpdate(REAL32 rDeltaSeconds)
{
}

void UIPopup_Clan_ChangeMark_Old::OnPreGuiNotify(void)
{
}

bool UIPopup_Clan_ChangeMark_Old::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	return UIPopupClanBase::OnEvent(event, pObj, param2, code);
}

void UIPopup_Clan_ChangeMark_Old::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	switch( evt)
	{
	case EVENT_CLAN_DUPLICATE_MARK:
		{
			if( EV_SUCCESSED(arg))
			{
				//	"사용 가능한 클랜 마크입니다."
				//	"클랜 마크 변경"
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_POSSIBLE_MARK"),
					GAME_RCSTRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK")); 

				ACTIVATE_BUTTON(m_pButtonOk);
			}
			else
			{
				//	"사용할 수 없는 클랜 마크입니다."
				//	"클랜 마크 변경"
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_IMPOSSIBLE_MARK"), 
					GAME_RCSTRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK"));

				DEACTIVATE_BUTTON(m_pButtonOk);
			}

			m_bWaitingForCheck = false;
		}
		break;
	case EVENT_CLAN_CHANGE_MARK:
		{
			if( EV_SUCCESSED(arg))
			{
				//	"클랜 마크 변경을 완료하였습니다."
				//	"클랜 마크 변경"
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMPLETE_CHANGE"), GAME_RCSTRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK")); 

				//	배경 모델의 클랜 마크도 변경
				//((CStageReady*)m_pParent)->GetReadyBg()->CharaChangeClanMarkInReadyBg();

				OnExitKey();
			}
			else
			{
				//	"사용할 수 없는 클랜 마크입니다."
				//	"클랜 마크 변경"
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_IMPOSSIBLE_MARK"), GAME_RCSTRING("STBL_IDX_CLAN_CHANGE_CLAN_MARK"));

				DEACTIVATE_BUTTON(m_pButtonOk);
			}
		}
		break;
	}
}

void UIPopup_Clan_ChangeMark_Old::OnExitKey(void)
{
	GetMainFrame()->ClosePopup(UPW_CLAN_MARKCHANGE_OLD);
}

void UIPopup_Clan_ChangeMark_Old::OnOK(void)
{
	I3_BOUNDCHK(m_nBackImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
	I3_BOUNDCHK(m_nShapeImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
	I3_BOUNDCHK(m_nBackColorIdx, CClanMarkManager::GetMarkColorCount()+1);
	I3_BOUNDCHK(m_nShapeColorIdx, CClanMarkManager::GetMarkColorCount()+1);

	UINT32 nFlag = MAKE_CLANMARK_FLAG( m_nBackImageIdx, m_nShapeImageIdx, m_nBackColorIdx, m_nShapeColorIdx);

	//g_pFramework->GetClanContext()->ProcessChangeClanMark( nFlag);
	g_pFramework->GetClanContext()->ProcessChangeClanMark( m_nAuthFlag, nFlag);
}

void UIPopup_Clan_ChangeMark_Old::OnCancel(void)
{
	OnExitKey();
}

void UIPopup_Clan_ChangeMark_Old::OnOverlappingCheck(void)
{
	//	1 base (0 is default)
	if( !m_bWaitingForCheck)
	{
		I3_BOUNDCHK(m_nBackImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
		I3_BOUNDCHK(m_nShapeImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
		I3_BOUNDCHK(m_nBackColorIdx, CClanMarkManager::GetMarkColorCount()+1);
		I3_BOUNDCHK(m_nShapeColorIdx, CClanMarkManager::GetMarkColorCount()+1);

		UINT32 nFlag = MAKE_CLANMARK_FLAG( m_nBackImageIdx, m_nShapeImageIdx, m_nBackColorIdx, m_nShapeColorIdx);

		g_pFramework->GetClanContext()->SendOverlappingClanMark(nFlag);

		m_bWaitingForCheck = true;
	}
}

void UIPopup_Clan_ChangeMark_Old::OnPrevBackImage(void)
{
	m_nBackImageIdx--;

	if( m_nBackImageIdx <= 0)							m_nBackImageIdx = CClanMarkManager::GetMarkBackImageCount();

	//	이전 마크 배경 이미지로 업데이트
	_ClanMarkUserSelectImageUpdate(0);

	DEACTIVATE_BUTTON(m_pButtonOk);
}

void UIPopup_Clan_ChangeMark_Old::OnNextBackImage(void)
{
	m_nBackImageIdx++;

	if( m_nBackImageIdx > CClanMarkManager::GetMarkBackImageCount())		m_nBackImageIdx = 1;

	//	다음 마크 배경 이미지로 업데이트
	_ClanMarkUserSelectImageUpdate(0);

	DEACTIVATE_BUTTON(m_pButtonOk);
}

void UIPopup_Clan_ChangeMark_Old::OnPrevShapeImage(void)
{
	m_nShapeImageIdx--;

	if( m_nShapeImageIdx <= 0)							m_nShapeImageIdx = CClanMarkManager::GetMarkShapeImageCount();

	//	이전 마크 모양 이미지로 업데이트
	_ClanMarkUserSelectImageUpdate(1);

	DEACTIVATE_BUTTON(m_pButtonOk);
}

void UIPopup_Clan_ChangeMark_Old::OnNextShapeImage(void)
{
	m_nShapeImageIdx++;

	if( m_nShapeImageIdx > CClanMarkManager::GetMarkShapeImageCount())	m_nShapeImageIdx = 1;

	//	다음 마크 모양 이미지로 업데이트
	_ClanMarkUserSelectImageUpdate(1);

	DEACTIVATE_BUTTON(m_pButtonOk);
}

void UIPopup_Clan_ChangeMark_Old::OnChangeBackColor(INT32 nColorIdx)
{	//	배경 마크 색상 변경	(1 base)
	m_nBackColorIdx = nColorIdx;

	I3ASSERT( m_nBackColorIdx > 0);
	I3_BOUNDCHK( m_nBackColorIdx,CClanMarkManager::GetMarkColorCount()+1);

	INT32 idxArray = nColorIdx-1;

	INT32 selectedWidth = m_pBackColorSelected->getWidth();
	INT32 slotWidth = m_pBackColorBox[idxArray]->getWidth();
	INT32 offsetWidth = (selectedWidth - slotWidth) / 2;
	REAL32 slotPos = m_pBackColorBox[idxArray]->getPositionX();

	m_pBackColorSelected->setPositionX( slotPos - offsetWidth); 

	//PlaySound(GUI_SND_ITEM_SELECTING);

	_ClanMarkUserSelectImageUpdate(0);

	DEACTIVATE_BUTTON(m_pButtonOk);
}

void UIPopup_Clan_ChangeMark_Old::OnChangeShapeColor(INT32 nColorIdx)
{	//	모양 마크 색상 변경 (1 base)
	m_nShapeColorIdx = nColorIdx;

	I3ASSERT( m_nShapeColorIdx > 0);
	I3_BOUNDCHK( m_nShapeColorIdx,CClanMarkManager::GetMarkColorCount()+1);

	INT32 idxArray = nColorIdx-1;

	INT32 selectedWidth = m_pShapeColorSelected->getWidth();
	INT32 slotWidth = m_pShapeColorBox[idxArray]->getWidth();
	INT32 offsetWidth = (selectedWidth - slotWidth) / 2;
	REAL32 slotPos = m_pShapeColorBox[idxArray]->getPositionX();

	m_pShapeColorSelected->setPositionX( slotPos - offsetWidth); 

//	PlaySound(GUI_SND_ITEM_SELECTING);

	_ClanMarkUserSelectImageUpdate(1);

	DEACTIVATE_BUTTON(m_pButtonOk);
}

void UIPopup_Clan_ChangeMark_Old::SetInputDisable(bool bDisable)
{
	m_pGui->setInputDisable(bDisable);
}

bool UIPopup_Clan_ChangeMark_Old::GetInputDisable(void)
{
	return m_pGui->getInputDisable();
}

void UIPopup_Clan_ChangeMark_Old::_InitPopup(UINT32 nFlag)
{	
	//	현재 클랜 마크 값을 가져온다.
	m_nBackImageIdx = CLANMARK_FRAME_IMAGE_IDX( nFlag);
	m_nShapeImageIdx = CLANMARK_CONTENT_IMAGE_IDX( nFlag);
	m_nBackColorIdx = CLANMARK_FRAME_COLOR_IDX( nFlag);
	m_nShapeColorIdx = CLANMARK_CONTENT_COLOR_IDX( nFlag);

	//	default 1 base
	if( m_nBackImageIdx <= 0)	m_nBackImageIdx = 1;
	if( m_nShapeImageIdx <= 0)	m_nShapeImageIdx = 1;
	if( m_nBackColorIdx <= 0)	m_nBackColorIdx  = 1;
	if( m_nShapeColorIdx <= 0)	m_nShapeColorIdx = 1;

	_CleanupTexture();

	//	선택용 클랜 마크 조합 이미지를 미리 생성
	for(INT32 i=0; i<CHANGE_CLANMARK_COMBINATION_MAX; i++)
	{
		for(INT32 j=0; j<CHANGE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			//	텍스처 생성
			m_pTexClanMarkSelect[i][j] = i3Texture::new_object(); 
			I3ASSERT( m_pTexClanMarkSelect[i][j] != nullptr);

			m_pTexClanMarkSelect[i][j]->Create(64, 64, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);
		}
	}

	// 조합 완료 마크 텍스처 생성
	m_pTexClanMarkFinal = i3Texture::new_object(); 
	I3ASSERT( m_pTexClanMarkFinal != nullptr);
	m_pTexClanMarkFinal->Create(64, 64, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);

	DEACTIVATE_BUTTON(m_pButtonOk);
	ACTIVATE_BUTTON(m_pButtonOverlappingCheck);

	//	초기화
	OnChangeBackColor(m_nBackColorIdx);
	OnChangeShapeColor(m_nShapeColorIdx);

	for(INT32 i=0; i<CHANGE_CLANMARK_COMBINATION_MAX; i++)
	{
		ACTIVATE_BUTTON(m_pClanMarkPrev[i]);
		ACTIVATE_BUTTON(m_pClanMarkNext[i]);

		_ClanMarkUserSelectImageUpdate(i);
	}

	for(INT32 i=0; i<CHANGE_CLANMARK_COLOR_SLOT_MAX; i++)
	{
		m_pBackColorBox[i]->setInputDisable(false);
		m_pShapeColorBox[i]->setInputDisable(false);
	}

	m_bWaitingForCheck = false;
}

void UIPopup_Clan_ChangeMark_Old::_CleanupTexture(void)
{
	for(INT32 i=0; i<CHANGE_CLANMARK_COMBINATION_MAX; i++)
	{
		for(INT32 j=0; j<CHANGE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			I3_SAFE_RELEASE( m_pTexClanMarkSelect[i][j]);
		}
	}

	I3_SAFE_RELEASE(m_pTexClanMarkFinal);
}

void UIPopup_Clan_ChangeMark_Old::_ClanMarkUserSelectImageUpdate(INT32 LayerType)
{
	INT32 nImageIdx = 1;
	INT32 nColorIdx = 1;
	INT32 nImageMax = 1;

	//	관련된 클랜 마크 조각 슬롯 이미지 업데이트
	for(INT32 slot=0; slot<CHANGE_CLANMARK_SHAPE_SLOT_MAX; slot++)
	{		
		if( LayerType == 0)			//	배경 마크
		{
			nImageIdx = m_nBackImageIdx;
			nColorIdx = m_nBackColorIdx;
			nImageMax = CClanMarkManager::GetMarkBackImageCount();

			I3_BOUNDCHK( nImageIdx,CClanMarkManager::GetMarkBackImageCount()+1);
			I3_BOUNDCHK( nColorIdx,CClanMarkManager::GetMarkColorCount()+1);
		}
		else if( LayerType == 1)	//	모양 마크	
		{
			nImageIdx = m_nShapeImageIdx;
			nColorIdx = m_nShapeColorIdx;
			nImageMax = CClanMarkManager::GetMarkShapeImageCount();

			I3_BOUNDCHK( nImageIdx,CClanMarkManager::GetMarkShapeImageCount()+1);
			I3_BOUNDCHK( nColorIdx,CClanMarkManager::GetMarkColorCount()+1);
		}

		//	가운데 3번째(idx 0) 슬롯을 중심으로 좌우 슬롯은 무한 루프 되도록 인덱스를 정한다.
		switch( slot)
		{
		case 0:		nImageIdx -= 2;		if( nImageIdx <= 0)	nImageIdx += nImageMax;		break;
		case 1:		nImageIdx -= 1;		if( nImageIdx <= 0)	nImageIdx += nImageMax;		break;
		case 2:		nImageIdx += 0;		break;
		case 3:		nImageIdx += 1;		if( nImageIdx > nImageMax)	nImageIdx -= nImageMax;		break;
		case 4:		nImageIdx += 2;		if( nImageIdx > nImageMax)	nImageIdx -= nImageMax;		break;
		default:	continue;
		}

		if( LayerType == 0)			//	배경 마크
		{
			I3_BOUNDCHK( nImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
			I3_BOUNDCHK( nColorIdx, CClanMarkManager::GetMarkColorCount()+1);
		}
		else if( LayerType == 1)	//	모양 마크	
		{
			I3_BOUNDCHK( nImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
			I3_BOUNDCHK( nColorIdx, CClanMarkManager::GetMarkColorCount()+1);
		}

		//	하나의 슬롯의 마크 조각 이미지를 변경합니다.
		_ClanMarkUserSelectImage(LayerType, slot, nImageIdx, nColorIdx); 
	}

	//	클랜 마크 조각 이미지가 변경됬으므로 최종 조합 마크 이미지도 함께 변경
	_ClanMarkUserFinalImage();
}

void UIPopup_Clan_ChangeMark_Old::_ClanMarkUserSelectImage(INT32 LayerType, INT32 nSlotIdx, INT32 nImageIdx, INT32 nColorIdx)
{	//	하나의 슬롯의 마크 조각 이미지를 변경합니다.
	I3_BOUNDCHK( LayerType, CHANGE_CLANMARK_COMBINATION_MAX);
	I3_BOUNDCHK( nSlotIdx, CHANGE_CLANMARK_SHAPE_SLOT_MAX);

	i3Texture * pTex = m_pTexClanMarkSelect[LayerType][nSlotIdx];
	I3ASSERT(pTex);

	if( LayerType == 0)			//	배경 마크
	{
		::MakeClanMarkShapeTex(pTex, CLAN_MARK_MEDIUM, CLAN_MARK_LAYER_FRAME, nImageIdx, nColorIdx);
	}
	else if( LayerType == 1)	//	모양 마크
	{
		::MakeClanMarkShapeTex(pTex, CLAN_MARK_MEDIUM, CLAN_MARK_LAYER_CONTENT, nImageIdx, nColorIdx);
	}

	i3GuiControl * pControl = m_pClanMarkSelectImage[LayerType][nSlotIdx];
	i3GuiImage * pImage = pControl->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);
	I3ASSERT( pImage != nullptr);

	pImage->SetTexture( pTex, 0, 0, CLAN_MARK_MID_PIXEL, CLAN_MARK_MID_PIXEL);
	pControl->SetShapeEnable( GUI_SHAPE_OFFSET, true);
	pControl->UpdateRenderObjects();	//	Sprite를 강제 업데이트 하기위해...이거 없으면 GUI에서 텍스처 정보를 업데이트 안합니다. komet

}

void UIPopup_Clan_ChangeMark_Old::_ClanMarkUserFinalImage(void)
{	//	모든 조합이 완료된 최종 마크 이미지 업데이트
	i3Texture * pTex = m_pTexClanMarkFinal;
	I3ASSERT(pTex);

	::MakeClanMarkTex(pTex, CLAN_MARK_MEDIUM, m_nBackImageIdx, m_nShapeImageIdx, m_nBackColorIdx,m_nShapeColorIdx);

	i3GuiControl * pControl = m_pClanMarkConfirmImage;
	i3GuiImage * pImage = pControl->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);
	I3ASSERT( pImage != nullptr);

	pImage->SetTexture( pTex, 0, 0, CLAN_MARK_MID_PIXEL, CLAN_MARK_MID_PIXEL);
	pControl->SetShapeEnable( GUI_SHAPE_OFFSET, true);
	pControl->UpdateRenderObjects();	//	Sprite를 강제 업데이트 하기위해...이거 없으면 GUI에서 텍스처 정보를 업데이트 안합니다. komet
}
#endif