#include "pch.h"
#include "GlobalVariables.h"
#include "PopupUseItem.h"

#if !defined( ENABLE_UI2)

enum ITEM_USE_METHOD
{
	USE_METHOD_UNKNOWN,

	USE_METHOD_DEFAULT_AUTH,		//DefaultAuth,
	USE_METHOD_DISGUISE_NICK,		//DisguiseNick,
	USE_METHOD_DISGUISE_RANK,		//DisguiseRank,
	USE_METHOD_COLOR_NICK,			//ColorNick,
	USE_METHOD_COLOR_CLAN,			//ColorClan,
	USE_METHOD_COLOR_CROSS_HAIR,	//ColorCrossHair,
	USE_METHOD_CHANGE_NICK,			//ChangeNick,
	USE_METHOD_CHANGE_CLAN_NAME,	//ChangeClanName,
	USE_METHOD_CHANGE_CLAN_MARK,	//ChangeClanMark,
	USE_METHOD_BATTLE_FIELD_MOVE,	//BattleFieldMove,

	MAX_USE_METHOD
};

enum ITEM_RESULT_METHOD
{
	RESULT_METHOD_UNKNOWN,

	RESULT_METHOD_COLOR_NICK,		//ColorNick,
	RESULT_METHOD_COLOR_CLAN,		//ColorClan,
	RESULT_METHOD_NICK,				//Nick,
	RESULT_METHOD_RANK,				//Rank,
	RESULT_METHOD_COLOR_CROSS_HAIR,	//ColorCrossHair
	RESULT_METHOD_CLAN_NAME,		//ClanName,
	RESULT_METHOD_CLAN_PERSON_PLUS,	//ClanPersonPlus
	RESULT_METHOD_GRAB_BAG,			//GrabBag,
	RESULT_METHOD_POINT,			//Point,

	MAX_RESULT_METHOD
};

ITEM_USE_METHOD		UseMethodStringToEnum(const char * pString)
{
	const char * Keywords[MAX_USE_METHOD] =
	{
		_T(""),
		_T("DefaultAuth"),
		_T("DisguiseNick"),
		_T("DisguiseRank"),
		_T("ColorNick"),
		_T("ColorClan"),
		_T("ColorCrossHair"),
		_T("ChangeNick"),
		_T("ChangeClanName"),
		_T("ChangeClanMark"),
		_T("BattleFieldMove"),
	};

	for (INT32 i = 1 ; i < MAX_USE_METHOD ; ++i )
	{
		if ( 0 == i3String::Compare(Keywords[i], pString) )
			return static_cast<ITEM_USE_METHOD>(i);
	}

	return USE_METHOD_UNKNOWN;
}

ITEM_RESULT_METHOD	ResultMethodStringToEnum(const char * pString)
{
	const char * Keywords[MAX_RESULT_METHOD] =
	{
		_T(""),
		_T("ColorNick"),
		_T("ColorClan"),
		_T("Nick"),
		_T("Rank"),
		_T("ColorCrossHair"),
		_T("ClanName"),
		_T("ClanPersonPlus"),
		_T("GrabBag"),
		_T("Point"),
	};

	for (INT32 i = 1 ; i < MAX_RESULT_METHOD ; ++i )
	{
		if ( 0 == i3String::Compare(Keywords[i], pString) )
			return static_cast<ITEM_RESULT_METHOD>(i);
	}

	return RESULT_METHOD_UNKNOWN;
}



struct UseCashItemMessage
{
	char * _pszResultStatic;	//	USE_ITEM_COMPLETE '사용 아이템'
	char * _pszResultValue;		//	USE_ITEM_COMPLETE '사용 결과'
	char * _pszResultMessage;	//	USE_ITEM_COMPLETE '안내 메시지'
};

I3_CLASS_INSTANCE(CPopupUseItem, CPopupBase);

CPopupUseItem::CPopupUseItem(void)
{
	m_ItemID = 0;
	m_AuthWareDBIndex = 0;
	m_Type = USE_ITEM_UNKNOWN;

	m_Style = USE_ITEM_STYLE_NORMAL;
	m_pStringFilter = NULL;
	m_pStringFilter2 = NULL;

	m_bChangeNameChecked = FALSE;

	m_pRoot = NULL;

	m_pQueryRoot = NULL;
	m_pQueryCaption = NULL;
	m_pQueryButtonOk = NULL;
	m_pQueryButtonCancel = NULL;
	m_pQueryItemImage = NULL;
	m_pQueryItemStatic = NULL;
	m_pQueryItemValue = NULL;
	m_pQueryAttrStatic = NULL;
	m_pQueryAttrValue = NULL;
	m_pQueryNotiveValue = NULL;
	m_pQueryStatic = NULL;

	m_pChangeNameRoot = NULL;
	m_pChangeNameCaption = NULL;
	m_pChangeNameButtonOk = NULL;
	m_pChangeNameButtonCancel = NULL;
	m_pChangeNameItemStatic = NULL;
	m_pChangeNameItemValue = NULL;
	m_pChangeNameBeforeStatic = NULL;
	m_pChangeNameBeforeValue = NULL;
	m_pChangeNameUserStatic = NULL;
	m_pChangeNameUserEdit = NULL;
	m_pChangeNameCheckStatic = NULL;
	m_pChangeNameCheck = NULL;
	m_pChangeNameNoticeValue = NULL;

	m_pChangeColorRoot = NULL;
	m_pChangeColorCaption = NULL;
	m_pChangeColorButtonOk = NULL;
	m_pChangeColorButtonCancel = NULL;
	m_pChangeColorItemStatic = NULL;
	m_pChangeColorItemValue = NULL;
	m_pChangeColorCurrentStatic = NULL;
	m_pChangeColorCurrentColorBox = NULL;
	m_pChangeColorSelectStatic = NULL;
	m_pChangeColorApplyStatic = NULL;
	m_pChangeColorApplyValue = NULL;
	m_pChangeColorNoticeValue = NULL;
	m_pChangeColorSelectColorBoxFrame = NULL;

	i3mem::FillZero(m_pChangeColorSelectColorBox, sizeof(m_pChangeColorSelectColorBox));


	m_pChangeCrosshairColorRoot = NULL;
	m_pChangeCrosshairColorCaption = NULL;
	m_pChangeCrosshairColorButtonOk = NULL;
	m_pChangeCrosshairColorButtonCancel = NULL;
	m_pChangeCrosshairColorItemStatic = NULL;
	m_pChangeCrosshairColorItemValue = NULL;
	m_pChangeCrosshairColorCurrentStatic = NULL;
	m_pChangeCrosshairColorCurrentColorBox = NULL;
	m_pChangeCrosshairColorSelectStatic = NULL;
	m_pChangeCrosshairColorNoticeValue = NULL;
	m_pChangeCrosshairColorSelectColorBoxFrame = NULL;
	m_pChangeCrosshairColorCrossImage = NULL;
	m_pChangeCrosshairColorCrossDotImage = NULL;
	m_pChangeCrosshairColorDotImage = NULL;
	m_pChangeCrosshairColorCircleImage = NULL;

	i3mem::FillZero(m_pChangeCrosshairColorSelectColorBox, sizeof(m_pChangeCrosshairColorSelectColorBox));

	m_pChangeRankRoot = NULL;
	m_pChangeRankCaption = NULL;
	m_pChangeRankButtonOk = NULL;
	m_pChangeRankButtonCancel = NULL;
	m_pChangeRankItemStatic = NULL;
	m_pChangeRankItemValue = NULL;
	m_pChangeRankCurrentStatic = NULL;
	m_pChangeRankCurrentValue = NULL;
	m_pChangeRankSelectStatic = NULL;
	m_pChangeRankSelectCombo = NULL;
	m_pChangeRankSelectButton = NULL;
	m_pChangeRankSelectList = NULL;
	m_pChangeRankNoticeValue = NULL;

	m_pCompleteRoot = NULL;
	m_pCompleteCaption = NULL;
	m_pCompleteButtonOk = NULL;
	m_pCompleteResultStatic = NULL;
	m_pCompleteResultValue = NULL;
	m_pCompleteNoticeValue = NULL;

	m_szChangeName[0] = 0;
	m_nSelected = 0;
}

CPopupUseItem::~CPopupUseItem(void)
{
	I3_SAFE_RELEASE(m_pStringFilter);
	I3_SAFE_RELEASE(m_pStringFilter2);
}

BOOL CPopupUseItem::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char strName[MAX_STRING_COUNT] = "";

	LinkControlEx(plist, "UseItem", m_pRoot, -1);

	//	아이템사용 문의 팝업
	{
		LinkControlEx(plist, "useItemQueryPopup", m_pQueryRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupCaption", m_pQueryCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupOk", m_pQueryButtonOk, GCI_USE_ITEM_B_QUERY_OK);
		LinkControlEx(plist, "useItemQueryPopupCancel", m_pQueryButtonCancel, GCI_USE_ITEM_B_QUERY_CANCEL);
		LinkControlEx(plist, "useItemQueryPopupItemImage", m_pQueryItemImage, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupItemStatic", m_pQueryItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupItemValue", m_pQueryItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupAttrStatic", m_pQueryAttrStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupAttrValue", m_pQueryAttrValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupNoticeValue", m_pQueryNotiveValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemQueryPopupStatic", m_pQueryStatic, GCI_UNKNOWN);
	}

	//	닉네임 변경
	{
		LinkControlEx(plist, "useItemChangeNamePopup", m_pChangeNameRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupCaption", m_pChangeNameCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupOk", m_pChangeNameButtonOk, GCI_USE_ITEM_B_USE_ITEM_OK);
		LinkControlEx(plist, "useItemChangeNamePopupCancel", m_pChangeNameButtonCancel, GCI_USE_ITEM_B_USE_ITEM_CANCEL);
		LinkControlEx(plist, "useItemChangeNamePopupItemStatic", m_pChangeNameItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupItemValue", m_pChangeNameItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupBeforeStatic", m_pChangeNameBeforeStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupBeforeValue", m_pChangeNameBeforeValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupUserStatic", m_pChangeNameUserStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupUserEdit", m_pChangeNameUserEdit, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupCheckStatic", m_pChangeNameCheckStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeNamePopupCheck", m_pChangeNameCheck, GCI_USE_ITEM_B_CHANGE_NAME_CHECK);
		LinkControlEx(plist, "useItemChangeNamePopupStatic", m_pChangeNameNoticeValue, GCI_UNKNOWN);
	}

	//	컬러 변경
	{
		LinkControlEx(plist, "useItemChangeColorPopup", m_pChangeColorRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupCaption", m_pChangeColorCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupOk", m_pChangeColorButtonOk, GCI_USE_ITEM_B_USE_ITEM_OK);
		LinkControlEx(plist, "useItemChangeColorPopupCancel", m_pChangeColorButtonCancel, GCI_USE_ITEM_B_USE_ITEM_CANCEL);
		LinkControlEx(plist, "useItemChangeColorPopupTitleStatic", m_pChangeColorItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupTitleValue", m_pChangeColorItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupCurrentStatic", m_pChangeColorCurrentStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCurrentColorBox", m_pChangeColorCurrentColorBox, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupSelectStatic", m_pChangeColorSelectStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupApplyStatic", m_pChangeColorApplyStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupApplyValue", m_pChangeColorApplyValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorPopupNoticeValue", m_pChangeColorNoticeValue, GCI_UNKNOWN);

		for(INT32 i=0; i<SELECT_COLOR_BOX_COUNT; i++)
		{
			sprintf_s(strName, "useItemChangeColorSelectColorBox%d", i + 1);
			LinkControlEx(plist, strName, m_pChangeColorSelectColorBox[i], GCI_USE_ITEM_S_CHANGE_COLOR_BOX1 + i);
		}

		LinkControlEx(plist, "useItemChangeColorSelectColorSelected", m_pChangeColorSelectColorBoxFrame, GCI_UNKNOWN);

	}

	//	컬러 크로스헤어
	{
		LinkControlEx(plist, "useItemChangeColorCrosshairPopup", m_pChangeCrosshairColorRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupCaption", m_pChangeCrosshairColorCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupOk", m_pChangeCrosshairColorButtonOk, GCI_USE_ITEM_B_USE_ITEM_OK);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupCancel", m_pChangeCrosshairColorButtonCancel, GCI_USE_ITEM_B_USE_ITEM_CANCEL);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupTitleStatic", m_pChangeCrosshairColorItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupTitleValue", m_pChangeCrosshairColorItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupCurrentStatic", m_pChangeCrosshairColorCurrentStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairCurrentColorBox", m_pChangeCrosshairColorCurrentColorBox, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupSelectStatic", m_pChangeCrosshairColorSelectStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairPopupNoticeValue", m_pChangeCrosshairColorNoticeValue, GCI_UNKNOWN);

		LinkControlEx(plist, "useItemChangeColorCrosshairCross", m_pChangeCrosshairColorCrossImage, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairCrossDot", m_pChangeCrosshairColorCrossDotImage, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairDot", m_pChangeCrosshairColorDotImage, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeColorCrosshairCircle", m_pChangeCrosshairColorCircleImage, GCI_UNKNOWN);

		for(INT32 i=0; i<SELECT_CROSSHAIR_COLOR_BOX_COUNT; i++)
		{
			sprintf_s(strName, "useItemChangeColorCrosshairSelectColorBox%d", i + 1);
			LinkControlEx(plist, strName, m_pChangeCrosshairColorSelectColorBox[i], GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX1 + i);
		}

		LinkControlEx(plist, "useItemChangeColorCrosshairSelectColorSelected", m_pChangeCrosshairColorSelectColorBoxFrame, GCI_UNKNOWN);

	}

	//	계급 변경
	{
		LinkControlEx(plist, "useItemChangeRankPopup", m_pChangeRankRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupCaption", m_pChangeRankCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupOk", m_pChangeRankButtonOk, GCI_USE_ITEM_B_USE_ITEM_OK);
		LinkControlEx(plist, "useItemChangeRankPopupCancel", m_pChangeRankButtonCancel, GCI_USE_ITEM_B_USE_ITEM_CANCEL);
		LinkControlEx(plist, "useItemChangeRankPopupTitleStatic", m_pChangeRankItemStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupTitleValue", m_pChangeRankItemValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupCurrentStatic", m_pChangeRankCurrentStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupCurrentValue", m_pChangeRankCurrentValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupSelectStatic", m_pChangeRankSelectStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupSelectCombo", m_pChangeRankSelectCombo, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemChangeRankPopupSelectButton", m_pChangeRankSelectButton, GCI_USE_ITEM_B_CHANGE_RANK_SELECT_BUTTON);
		LinkControlEx(plist, "useItemChangeRankPopupSelectList", m_pChangeRankSelectList, GCI_USE_ITEM_L_CHANGE_RANK_SELECT_LIST);
		LinkControlEx(plist, "useItemChangeRankPopupNoticeValue", m_pChangeRankNoticeValue, GCI_UNKNOWN);
	}

	//	사용 확인 팝업
	{
		LinkControlEx(plist, "useItemCompletePopup", m_pCompleteRoot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupCaption", m_pCompleteCaption, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupOk", m_pCompleteButtonOk, GCI_USE_ITEM_B_CONFIRM_OK);
		LinkControlEx(plist, "useItemCompletePopupResultStatic", m_pCompleteResultStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupResultValue", m_pCompleteResultValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupNoticeValue", m_pCompleteNoticeValue, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupCrosshairCross", m_pCompleteCrosshair_Cross, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupCrosshairCrossDot", m_pCompleteCrosshair_CrossDot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupCrosshairDot", m_pCompleteCrosshair_Dot, GCI_UNKNOWN);
		LinkControlEx(plist, "useItemCompletePopupCrosshairCircle", m_pCompleteCrosshair_Circle, GCI_UNKNOWN);

	}

	return FALSE;
}

void CPopupUseItem::OnInitControl(void)
{
	//	아이템사용 팝업
	{
		INIT_WIN_CAPTION( m_pQueryCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*아이템 사용*/

		INIT_CAPTION_BUTTON(m_pQueryButtonOk, GAME_STRING("STR_POPUP_OK"));			//	"확인"
		INIT_CAPTION_BUTTON(m_pQueryButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"취소"

		INIT_STATIC_CAPTION(m_pQueryItemStatic, GAME_STRING("STR_POPUP_USE_ITEM"));/*사용 아이템:*/
		INIT_VALUE_CAPTION(m_pQueryItemValue, "");

		INIT_STATIC_CAPTION(m_pQueryAttrStatic, GAME_STRING("STR_POPUP_ITEM_ATTRIBUTE"));/*속성:*/
		INIT_VALUE_CAPTION(m_pQueryAttrValue, "");

		INIT_VALUE_CAPTION(m_pQueryNotiveValue, "");
		m_pQueryNotiveValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pQueryNotiveValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_pQueryNotiveValue->SetTextSpace(0, 2);
		m_pQueryNotiveValue->SetTextAutoWrap(TRUE);

		INIT_WIN_CAPTION(m_pQueryStatic, GAME_STRING("STR_POPUP_Q_MESSAGE_WANT_USE_ITEM"));/*아이템을 사용 하시겠습니까?*/

		m_pQueryRoot->SetEnable(FALSE);
	}

	//	닉네임 변경 팝업
	{
		INIT_WIN_CAPTION( m_pChangeNameCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*아이템 사용*/

		INIT_CAPTION_BUTTON(m_pChangeNameButtonOk, GAME_STRING("STR_POPUP_OK"));	//	"확인"
		INIT_CAPTION_BUTTON(m_pChangeNameButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"취소"

		INIT_STATIC_CAPTION(m_pChangeNameItemStatic, GAME_STRING("STR_POPUP_USE_ITEM"));/*사용 아이템:*/
		INIT_VALUE_CAPTION(m_pChangeNameItemValue, "");

		INIT_STATIC_CAPTION(m_pChangeNameBeforeStatic, GAME_STRING("STR_POPUP_CURRENT_NICK_NAME"));/*현재 닉네임:*/
		INIT_VALUE_CAPTION(m_pChangeNameBeforeValue, "");

		INIT_STATIC_CAPTION(m_pChangeNameUserStatic, GAME_STRING("STR_POPUP_MODIFIED_NICK"));/*변경 닉네임:*/

		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
		ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
		m_pChangeNameUserEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pChangeNameUserEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pChangeNameUserEdit->SetTextColor(GetColor(GCT_VALUE));
		m_pChangeNameUserEdit->SetText("");

		INIT_SLOT(m_pChangeNameNoticeValue);
		INIT_STATIC_CAPTION2(m_pChangeNameCheckStatic, GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*중복검사를 해주십시오.*/
		INIT_CAPTION_BUTTON(m_pChangeNameCheck, GAME_STRING("STR_POPUP_CONFIRM_OVERLAPPING"));/*중복 확인*/

		m_pChangeNameRoot->SetEnable(FALSE);
	}

	//	컬러 변경
	{
		INIT_WIN_CAPTION( m_pChangeColorCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*아이템 사용*/

		INIT_STATIC_CAPTION(m_pChangeColorItemStatic, GAME_STRING("STR_POPUP_USE_ITEM"));/*사용 아이템:*/
		INIT_VALUE_CAPTION(m_pChangeColorItemValue, "");

		INIT_STATIC_CAPTION(m_pChangeColorCurrentStatic, GAME_STRING("STR_POPUP_CURRENT_COLOR"));/*현재 색상:*/
		INIT_STATIC_CAPTION(m_pChangeColorSelectStatic, GAME_STRING("STR_POPUP_MODIFIED_COLOR"));/*변경 색상:*/

		INIT_STATIC_CAPTION(m_pChangeColorApplyStatic, "");
		INIT_VALUE_CAPTION(m_pChangeColorApplyValue, "");

		INIT_WIN_CAPTION( m_pChangeColorNoticeValue, GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));/*변경할 색상을 선택해 주십시오.*/

		INIT_CAPTION_BUTTON(m_pChangeColorButtonOk, GAME_STRING("STR_POPUP_OK"));	//	"확인"
		INIT_CAPTION_BUTTON(m_pChangeColorButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"취소"

		for(INT32 i=0; i<SELECT_COLOR_BOX_COUNT; i++)
		{
			m_pChangeColorSelectColorBox[i]->setInputDisable(FALSE);
			m_pChangeColorSelectColorBox[i]->SetAllShapeEnable(FALSE);
			m_pChangeColorSelectColorBox[i]->SetShapeEnable(i, TRUE);
		}

		m_pChangeColorRoot->SetEnable(FALSE);
	}

	//	계급 변경
	{
		INIT_WIN_CAPTION( m_pChangeRankCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*아이템 사용*/

		INIT_STATIC_CAPTION(m_pChangeRankItemStatic, GAME_STRING("STR_POPUP_USE_ITEM"));/*사용 아이템:*/
		INIT_VALUE_CAPTION(m_pChangeRankItemValue, "");

		INIT_STATIC_CAPTION(m_pChangeRankCurrentStatic, GAME_STRING("STR_POPUP_CURRENT_RANK"));/*현재 계급:*/
		INIT_VALUE_CAPTION(m_pChangeRankCurrentValue, "");

		INIT_STATIC_CAPTION(m_pChangeRankSelectStatic, GAME_STRING("STR_POPUP_MODIFIED_RANK"));/*변경 계급:*/
		INIT_STATIC_CAPTION(m_pChangeRankSelectCombo, "");

		INIT_WIN_CAPTION( m_pChangeRankNoticeValue, GAME_STRING("STR_POPUP_MESSAGE_MODIFY_RANK"));/*변경할 계급을 선택해 주십시오.*/

		m_pChangeRankSelectList->CreateTextEx(5,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pChangeRankSelectList->SetSelColor(100, 100, 100, 100);
		m_pChangeRankSelectList->SetTextColor(GetColor(GCT_DEFAULT));
		m_pChangeRankSelectList->SetExclusiveEnableControl(TRUE);
		m_pChangeRankSelectList->AddElement(GAME_STRING("STBL_IDX_RANK_0"));		//	"훈련병"
		m_pChangeRankSelectList->AddElement(GAME_STRING("STBL_IDX_RANK_1"));		//	"이병"
		m_pChangeRankSelectList->AddElement(GAME_STRING("STBL_IDX_RANK_2"));		//	"일병"
		m_pChangeRankSelectList->AddElement(GAME_STRING("STBL_IDX_RANK_3"));		//	"상병"
		m_pChangeRankSelectList->AddElement(GAME_STRING("STBL_IDX_RANK_4"));		//	"병장"
		m_pChangeRankSelectList->SetEnable(FALSE);

		INIT_CAPTION_BUTTON(m_pChangeRankButtonOk, GAME_STRING("STR_POPUP_OK"));	//	"확인"
		INIT_CAPTION_BUTTON(m_pChangeRankButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"취소"

		m_pChangeRankRoot->SetEnable(FALSE);
	}

	//	컬러 크로스헤어
	{
		INIT_WIN_CAPTION( m_pChangeCrosshairColorCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*아이템 사용*/

		INIT_STATIC_CAPTION(m_pChangeCrosshairColorItemStatic, GAME_STRING("STR_POPUP_USE_ITEM"));/*사용 아이템:*/
		INIT_VALUE_CAPTION(m_pChangeCrosshairColorItemValue, "");

		INIT_STATIC_CAPTION(m_pChangeCrosshairColorCurrentStatic, GAME_STRING("STR_POPUP_CURRENT_COLOR"));/*현재 색상:*/
		INIT_STATIC_CAPTION(m_pChangeCrosshairColorSelectStatic, GAME_STRING("STR_POPUP_MODIFIED_COLOR"));/*변경 색상:*/

		INIT_WIN_CAPTION( m_pChangeCrosshairColorNoticeValue, GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));/*변경할 색상을 선택해 주십시오.*/

		INIT_CAPTION_BUTTON(m_pChangeCrosshairColorButtonOk, GAME_STRING("STR_POPUP_OK"));	//	"확인"
		INIT_CAPTION_BUTTON(m_pChangeCrosshairColorButtonCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"취소"

		for(INT32 i=0; i<SELECT_CROSSHAIR_COLOR_BOX_COUNT; i++)
		{
			m_pChangeCrosshairColorSelectColorBox[i]->setInputDisable(FALSE);
			m_pChangeCrosshairColorSelectColorBox[i]->SetAllShapeEnable(FALSE);
			m_pChangeCrosshairColorSelectColorBox[i]->SetShapeEnable(i, TRUE);
		}

		m_pChangeCrosshairColorRoot->SetEnable(FALSE);
	}

	//	사용 확인 팝업
	{
		INIT_WIN_CAPTION( m_pCompleteCaption, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*아이템 사용*/
		INIT_CAPTION_BUTTON(m_pCompleteButtonOk, GAME_STRING("STR_POPUP_OK"));	//	"확인"

		INIT_STATIC_CAPTION(m_pCompleteResultStatic, "");
		INIT_VALUE_CAPTION(m_pCompleteResultValue, "");
		m_pCompleteResultValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);

		INIT_WIN_CAPTION(m_pCompleteNoticeValue, "");

		m_pCompleteRoot->SetEnable(FALSE);
	}
}

void CPopupUseItem::OnGameEvent( INT32 event, INT32 arg)
{
	if( m_ItemID <= 0)		//	아이템 사용중이 아니므로 무시한다.
		return;

	//	이벤트 결과를 받아 처리한다.
	switch( event )
	{	
	case EVENT_ITEM_AUTH:	// 갯수제 아이템 인증
	case EVENT_CLAN_PERSON_PLUS:	// 클랜원 증가 캐시 아이템 (CASHITEM_GROUP_CLAN_PERSON_PLUS_50) 사용 결과
		{
			_SetInputLock(FALSE);	//	결과 화면이므로 입력 락을 푼다.

			if (EV_SUCCESSED(arg))
			{				
				// 클랜원 증가 캐시 아이템은 EVENT_ITEM_AUTH 이벤트일때는 클랜원의 실제 증가된 수치가 서버와 싱크되지 않을수 있으므로
				// 정확하게 서버에서 업데이트되는 EVENT_CLAN_PERSON_PLUS 이벤트를 받을때만 사용 결과 창을 출력한다.
				if( event == EVENT_ITEM_AUTH && 
					m_ItemID == MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_EXPENDABLE, CASHITEM_GROUP_CLAN_PERSON_PLUS_50, CASHITEM_DAY_NO))
					break;

				_Open(USE_ITEM_COMPLETE);
			}
			else
			{
				CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_FAILED_AUTH"),
					GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*아이템 사용 실패*//*인증에 실패하였습니다. 잠시 후 다시 시도해주세요.*/

				OnExitKey();
			}
		}
		break;
	case EVENT_USE_ITEM_CHECK_NICK:
	case EVENT_CLAN_DUPLICATE_NAME:
		{
			if (arg == 0)
			{
				m_bChangeNameChecked = TRUE;

				I3COLOR color;
				i3Color::Set(&color, (UINT8) 139, 217, 37, 255);
				m_pChangeNameCheckStatic->SetTextColor(&color);

				if( m_Type == USE_ITEM_CHANGE_CLAN_NAME)
					m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_CAN_USE_CLAN_NAME"));/*사용 가능한 클랜명입니다.*/
				else
					m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_CAN_USE_NICK_NAME"));/*사용 가능한 닉네임입니다.*/

				ACTIVATE_BUTTON(m_pChangeNameButtonOk);
			}
			else
			{
				if( m_Type == USE_ITEM_CHANGE_CLAN_NAME)
					m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_CLAN_NAME"));/*현재 사용중이거나 사용할 수 없는 클랜명입니다*/
				else
					m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_NICKNAME"));/*현재 사용중이거나 사용할 수 없는 닉네임입니다*/

				m_pChangeNameCheckStatic->SetTextColor(GetColor(GCT_RED));

				m_pChangeNameUserEdit->SetFocus(TRUE);

				DEACTIVATE_BUTTON(m_pChangeNameButtonOk);
			}
		}
		break;
	}
}

void CPopupUseItem::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
		//	아이템 사용
	case GCI_USE_ITEM_B_QUERY_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	_QueryOK();
		break;
	case GCI_USE_ITEM_B_QUERY_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnCancel();
		break;
	case GCI_USE_ITEM_B_USE_ITEM_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	_UseItemOk();
		break;
	case GCI_USE_ITEM_B_USE_ITEM_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnCancel();
		break;
	case GCI_USE_ITEM_B_CHANGE_NAME_CHECK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChangeNameDuplicate();
		break;
	case GCI_USE_ITEM_B_CONFIRM_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnClosePopup();
		break;
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX1:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX2:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX3:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX4:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX5:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX6:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX7:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX8:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX9:
	case GCI_USE_ITEM_S_CHANGE_COLOR_BOX10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChangeColorSelect(pNotify->m_nID - GCI_USE_ITEM_S_CHANGE_COLOR_BOX1);
		break;
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX1:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX2:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX3:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX4:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX5:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX6:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX7:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX8:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX9:
	case GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChangeCrosshairColorSelect(pNotify->m_nID - GCI_USE_ITEM_S_CHANGE_CROSSHAIR_COLOR_BOX1);
		break;
	case GCI_USE_ITEM_B_CHANGE_RANK_SELECT_BUTTON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChangeRankPopupButton();
		break;
	case GCI_USE_ITEM_L_CHANGE_RANK_SELECT_LIST:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnChangeRankPopupList(pNotify->m_nPriParam);
		break;
	}
}

void CPopupUseItem::OnOpenPopup(void * pArg1, void * pArg2)
{
	m_ItemID = 0;
	m_AuthWareDBIndex = 0;

	m_pRoot->setInputDisable( FALSE);

	if(pArg1 == NULL)
		return;


	m_ItemID = *(INT32*)pArg1;
	if( m_ItemID <= 0)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_1"));/*아이템을 사용할 수 없습니다.\n잘못된 아이템입니다.*/
		return;
	}


	if(pArg2 != NULL)
	{
		m_Style = *(USE_ITEM_STYLE*)pArg2;
	}
	else
	{
		m_Style = USE_ITEM_STYLE_NORMAL;
	}

	if(m_Style == USE_ITEM_STYLE_RESULT)
	{
		CPopupBase::OnOpenPopup( pArg1, pArg2);		//	enable

		//	아이템 사용 결과 창
		_Open(USE_ITEM_COMPLETE);
	}
	else
	{
		CPopupBase::OnOpenPopup( pArg1, pArg2);		//	enable

		//	무조건 처음은 유저에게 아이템 사용을 묻는 팝업을 띄운다.
		if( !_Open(USE_ITEM_QUERY))
			OnExitKey();
	}
}

void CPopupUseItem::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	m_pRoot->SetEnable(FALSE);

	I3_SAFE_RELEASE(m_pStringFilter);
	I3_SAFE_RELEASE(m_pStringFilter2);

	m_ItemID = 0;
}

void CPopupUseItem::OnCancel(void)
{
	//	상점에서 아이템 사용할려고 했지만 최종 취소할 경우
	if( m_Style == USE_ITEM_STYLE_BUYING)
	{
		//	"구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다."
		CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"),
			GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
	}

	OnClosePopup();
}

void CPopupUseItem::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);
}

void CPopupUseItem::OnPreGuiNotify(void)
{

}

void CPopupUseItem::OnExitKey(void)
{
	OnClosePopup();

	m_pQueryRoot->SetEnable(FALSE);
	m_pCompleteRoot->SetEnable(FALSE);
	m_pChangeNameRoot->SetEnable(FALSE);
	m_pChangeColorRoot->SetEnable(FALSE);
	m_pChangeRankRoot->SetEnable(FALSE);
	m_pChangeCrosshairColorRoot->SetEnable(FALSE);
}


BOOL CPopupUseItem::_Open( USE_ITEM_POPUP_TYPE type)
{
	// 사용여부
	UINT8 Type = 0;
	UINT32 Arg = 0;

	I3ASSERT( g_pFramework != NULL);
	I3ASSERT( g_pFramework->GetInvenList() != NULL);

	if (g_pFramework->GetCashItemTypeByID(m_ItemID, &Type, &Arg))
	{
		//	현재 사용중인 아이템
		if (ITEM_ATTR_TYPE_AUTH == Type)
		{
			CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CURRENT_USE_ITEM"),
				GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*아이템 사용*//*현재 사용중인 아이템입니다.*/

			return FALSE;
		}
	}

	m_Type = type;

	m_pRoot->SetEnable(TRUE);

	_SetEnablePopup(m_Type);

	PlaySound(GUI_SND_POPUP_OPEN);

	switch( type)
	{
	case USE_ITEM_QUERY:
		{
			_InitQueryPopup();

			m_nSelected = -1;
		}
		break;
	case USE_ITEM_CHANGE_NICK:
		{
			_InitChangeNickPopup();
		}
		break;
	case USE_ITEM_CHANGE_CLAN_NAME:
		{
			_InitChangeClanNamePopup();
		}
		break;
	case USE_ITEM_COLOR_NICK:
		{
			_InitColorNickPopup();
		}
		break;
	case USE_ITEM_COLOR_CLAN_NAME:
		{
			_InitColorClanNamePopup();
		}
		break;
	case USE_ITEM_COLOR_CROSSHAIR:
		{
			_InitColorCrosshairPopup();
		}
		break;
	case USE_ITEM_DISGUISE_NICK:
		{
			_InitDisguiseNickPopup();
		}
		break;
	case USE_ITEM_DISGUISE_RANK:
		{
			_InitDisguiseRankPopup();
		}
		break;
	case USE_ITEM_COMPLETE:
		{
			_InitCompletePopup();
		}
		break;
	default:
		{
			CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"),
				GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*사용할 수 없는 아이템입니다.*//*아이템 사용 실패*/

			return FALSE;
		}
		break;
	}

	return TRUE;
}

void CPopupUseItem::_SetInputLock(BOOL bValue)
{
	m_pRoot->setInputDisable( bValue);
	i3GuiRoot::setGuiDisableInput( bValue);

	((CStageReady*)m_pParent)->SetMenuBarInputDisable( bValue);
}

void CPopupUseItem::_InitQueryPopup(void)
{
	m_pQueryRoot->SetEnable(TRUE);
	m_pCompleteRoot->SetEnable(FALSE);
	m_pChangeNameRoot->SetEnable(FALSE);
	m_pChangeColorRoot->SetEnable(FALSE);
	m_pChangeRankRoot->SetEnable(FALSE);
	m_pChangeCrosshairColorRoot->SetEnable(FALSE);

	INT32 itemUsage = GET_ITEM_FLAG_TYPE(m_ItemID);
	INT32 itemNum = GET_ITEM_FLAG_NUMBER(m_ItemID);

	//	아이템 이미지
	g_pFramework->SetCashItemShape(m_pQueryItemImage, m_ItemID);

	// 이름,속성,설명 출력
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(m_ItemID);
	if( pInfo == NULL)
	{
		m_pQueryNotiveValue->SetText(INVALID_TEXT);
		m_pQueryAttrValue->SetText(INVALID_TEXT);
		m_pQueryItemValue->SetText(INVALID_TEXT);
	}

	char Name[MAX_STRING_COUNT] = {};
	g_pShopItemInfoDataBase->GetItemName(m_ItemID, Name, MAX_STRING_COUNT);

	//	속성 :
	if ( GET_ITEM_FLAG_TYPE(m_ItemID) != ITEM_TYPE_POINT )
	{
		char Attr[MAX_STRING_COUNT] = INVALID_TEXT;
		switch( itemUsage)
		{
		case ITEM_TYPE_MAINTENANCE:	i3String::Format(Attr, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_USABLE_TERM"), itemNum);/*%d일권*/	break;
		case ITEM_TYPE_EXPENDABLE:	i3String::Copy( Attr, GAME_STRING("STR_POPUP_DISPOSABLE"), MAX_STRING_COUNT );/*1회용*/	break;
		}
		m_pQueryAttrValue->SetText(Attr);
		m_pQueryAttrValue->SetEnable(TRUE);
		m_pQueryAttrStatic->SetEnable(TRUE);
	}
	else
	{
		m_pQueryAttrStatic->SetEnable(FALSE);
		m_pQueryAttrValue->SetEnable(FALSE);
	}

	m_pQueryItemValue->SetText(Name);

	if( pInfo != NULL)
		m_pQueryNotiveValue->SetText(pInfo->GetDescription());	// 설명
}

void CPopupUseItem::_InitCompletePopup(void)
{
	BOOL	bUseColor = FALSE;
	char	temp[MAX_STRING_COUNT * 2] = "";
	INT32	idxColor = m_nSelected+1;

	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(m_ItemID);
	I3ASSERT( pInfo != NULL);

	//	BC의 ShopItem 항목에 각 아이템마다 정의되어 있는 메시지를 출력한다.
	const char * pszResultStatic	= pInfo->GetResultStatic();
	const char * pszResultValue		= pInfo->GetResultValue();
	const char * pszResultNotice	= pInfo->GetResultNotice();

	m_pCompleteCrosshair_Cross->SetEnable(FALSE);
	m_pCompleteCrosshair_CrossDot->SetEnable(FALSE);
	m_pCompleteCrosshair_Dot->SetEnable(FALSE);
	m_pCompleteCrosshair_Circle->SetEnable(FALSE);


	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	ITEM_RESULT_METHOD	Method = ResultMethodStringToEnum(pInfo->GetResultMethod());
	switch (Method)
	{
	case RESULT_METHOD_COLOR_NICK		:
		{
			pszResultValue = MyUserInfoBasic._nick;
			bUseColor = TRUE;
		}
		break;
	case RESULT_METHOD_COLOR_CLAN		:
		{
			pszResultValue = MyUserInfoBasic._clanName;
			bUseColor = TRUE;
		}
		break;
	case RESULT_METHOD_NICK				:
		{
			pszResultValue = MyUserInfoBasic._nick;
		}
		break;
	case RESULT_METHOD_RANK				:
		{
			UINT32 rank = MyUserInfoBasic._rank;

			pszResultValue = GetCharaRank(rank);
		}
		break;
	case RESULT_METHOD_COLOR_CROSS_HAIR	:
		{
			I3COLOR * pSelectColor = (I3COLOR*)GetColorCashItem(idxColor);

			pszResultValue = "";

			m_pCompleteCrosshair_Cross->SetEnable(TRUE);
			m_pCompleteCrosshair_CrossDot->SetEnable(TRUE);
			m_pCompleteCrosshair_Dot->SetEnable(TRUE);
			m_pCompleteCrosshair_Circle->SetEnable(TRUE);

			//	크로스헤어 결과 이미지 색상 적용
			m_pCompleteCrosshair_Cross->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
			m_pCompleteCrosshair_CrossDot->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
			m_pCompleteCrosshair_Dot->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
			m_pCompleteCrosshair_Circle->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
		}
		break;
	case RESULT_METHOD_CLAN_NAME		:
		{
			//	클랜관련 정보는 서버에서 SUCCESS가 날라왔을때는 정보가 바로 갱신되지 않는다.
			//	전송 시간차 때문에 좀 있다 갱신되므로 가라로 성공한것처럼 보여야 한다.
			pszResultValue = &m_szChangeName[0];
		}
		break;
	case RESULT_METHOD_CLAN_PERSON_PLUS	:
		{
			//	클랜관련 정보는 서버에서 SUCCESS가 날라왔을때는 정보가 바로 갱신되지 않는다.
			//	전송 시간차 때문에 좀 있다 갱신되므로 가라로 성공한것처럼 보여야 한다.
			CLAN_DETAIL_INFO * pClanInfo = g_pGameContext->getMyClanDetailInfo();

			sprintf_s( temp, "%d", pClanInfo->_maxPerson);

			pszResultValue = &temp[0];
		}
		break;
	case RESULT_METHOD_GRAB_BAG			:
		{
			I3ASSERT(g_pGameContext->getCapsuleItemID() != m_ItemID);
			ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(m_ItemID);
			pszResultValue = pInfo->GetCapsuleItemResultName(g_pGameContext->getCapsuleItemIdx());
		}
		break;
	case RESULT_METHOD_POINT:
		{
			OnExitKey();
			return;
		}
		break;
	}


	m_pCompleteCaption->SetText(GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"));/*아이템 사용*/
	m_pCompleteResultStatic->SetText(pszResultStatic);

	if( bUseColor)	//	컬러
	{
		SetNameUsingColorTable(m_pCompleteResultValue, pszResultValue, (UINT8)idxColor);
	}
	else
	{
		m_pCompleteResultValue->SetText(pszResultValue);
		m_pCompleteResultValue->SetTextColor( GetColor( GCT_VALUE));
	}

	m_pCompleteNoticeValue->SetText(pszResultNotice);
}

void CPopupUseItem::_QueryOK(void)
{
	// 개수제 아이템일 경우엔 바로 사용한다.
	if ( ITEM_TYPE_EXPENDABLE == GET_ITEM_FLAG_TYPE(m_ItemID) )
	{
		_UseItem( m_ItemID);
		return;
	}

	// - 참고
	// 기간제 아이템은 갯수를 가지며 인증된 기간제 아이템은 갯수에서 기간제로 아이템이 바뀌어 인벤토리에 저장된다.
	//

	//
	// 기간제 아이템은 안내 메시지를 출력해준다.
	//

	INT32 ItemID = GetCashItemByGroup( m_ItemID);

	UINT8 Type = 0;
	UINT32 Arg = 0;
	g_pFramework->GetCashItemTypeByID(ItemID, &Type, &Arg);
	if( Arg <= 0)
	{
		// 기간제 아이템인데 남은기간이 0이면 사용중인게 없는것이다.
		// 바로 사용하도록 한다.
		_UseItem( m_ItemID);
		return;
	}



	//	"사용중인 아이템입니다.\n아이템의 기간만큼 남은 기간이 증가합니다.\n \n사용하시겠습니까?"
	const char * pszMessage = GAME_STRING("STR_POPUP_MESSAGE_CAN_NORMAL_ITEM");

	switch( GET_ITEM_FLAG_CLASS( m_ItemID))
	{
	case CASHITEM_GROUP_COLOR_CLAN:
	case CASHITEM_GROUP_COLOR_NICK:
	case CASHITEM_GROUP_COLOR_CROSSHAIR:
	case CASHITEM_GROUP_COLOR_CHATTING:
		{
			//	"사용중인 아이템입니다.\n사용시 색상을 변경할 수 있으며\n아이템의 기간만큼 남은 기간이 증가합니다.\n \n사용하시겠습니까?"
			pszMessage = GAME_STRING("STR_POPUP_MESSAGE_CAN_CHANGE_COLOR");
		}
		break;
	case CASHITEM_GROUP_DISGUISE_RANK:
		{
			//	"사용중인 아이템입니다.\n사용시 위장 계급으로 변경할 수 있으며\n아이템의 기간만큼 남은 기간이 증가합니다.\n \n사용하시겠습니까?"
			pszMessage = GAME_STRING("STR_POPUP_MESSAGE_CAN_DISGUISE_RANK");
		}
		break;
	case CASHITEM_GROUP_DISGUISE_NICK:
		{
			//	"사용중인 아이템입니다.\n사용시 임시 닉네임으로 변경할 수 있으며\n아이템의 기간만큼 남은 기간이 증가합니다.\n \n사용하시겠습니까?"
			pszMessage = GAME_STRING("STR_POPUP_MESSAGE_CAN_DISGUISE_NICK");
		}
		break;
	}

	// 사용중인 아이템이면 다시 한번 묻는다.
	CAPTION_POPUP_Q(pszMessage, GAME_STRING("STR_POPUP_CAPTION_USE_ITEM"), MAKE_CALLBACK(CbQueryOk), this);

}

void CPopupUseItem::CbQueryOk(void * pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		((CPopupUseItem*) pThis)->OnUseItem();
	}
	else
	{
		((CPopupUseItem*) pThis)->OnCancel();
	}
}

void CPopupUseItem::OnUseItem(void)
{
	_UseItem( m_ItemID);
}

BOOL CPopupUseItem::_CheckItem(INT32 nItemFlag)
{
	switch( GET_ITEM_FLAG_TYPE(nItemFlag))
	{
		//	지속성
	case ITEM_TYPE_MAINTENANCE:
		{
			switch( GET_ITEM_FLAG_CLASS( nItemFlag))
			{
				//	HP 5%를 이미 사용중이면 HP 10%를 사용할 수 없다.
			case CASHITEM_GROUP_MEGA_HP10:
				{
					INT32 useItem = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_MEGA_HP5, CASHITEM_DAY_NO);

					if( g_pFramework->IsUsingCashItem( useItem))
					{
						CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_BY_DUPLICATE"),
							GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*"중복으로 사용할 수 없는 아이템입니다.".*//*아이템 사용 실패*/

						return FALSE;
					}
				}
				break;
				//	HP 10%를 이미 사용중이면 HP 5%를 사용할 수 없다.
			case CASHITEM_GROUP_MEGA_HP5:
				{
					INT32 useItem = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_MEGA_HP10, CASHITEM_DAY_NO);

					if( g_pFramework->IsUsingCashItem( useItem))
					{
						CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_BY_DUPLICATE"),
							GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*"중복으로 사용할 수 없는 아이템입니다.".*//*아이템 사용 실패*/

						return FALSE;
					}
				}
				break;
				//	Full Metal Jacket Ammo 사용중이면 Hollow Point Ammo 또는 Jacketed Hollow Point 아이템과 중복 사용 불가.
			case CASHITEM_GROUP_FULLMETALJACKETAMMO:
			case CASHITEM_GROUP_HOLLOW_POINT_AMMO:
			case CASHITEM_GROUP_JACKETED_HELLOW_POINT_AMMO:
			case CASHITEM_GROUP_HOLLOW_POINT_AMMO_PLUS:
				{
					INT32 checkItemID[] =
					{
						MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_FULLMETALJACKETAMMO, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_HOLLOW_POINT_AMMO, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_JACKETED_HELLOW_POINT_AMMO, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_HOLLOW_POINT_AMMO_PLUS, CASHITEM_DAY_NO),
					};
					const INT32 checkItemCount = GET_ARRAY_COUNT(checkItemID);

					for ( INT32 i = 0 ; i < checkItemCount ; ++i )
					{
						// 체크하지 않습니다.
						if ( GET_ITEM_FLAG_CLASS(checkItemID[i]) == GET_ITEM_FLAG_CLASS( nItemFlag) )
							continue;

						if ( g_pFramework->IsUsingCashItem( checkItemID[i]) )
						{
							CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_BY_DUPLICATE"),
								GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*"중복으로 사용할 수 없는 아이템입니다.".*//*아이템 사용 실패*/

							return FALSE;
						}
					}
				}
				break;

			case CASHITEM_GROUP_BULLET_PROOF_VEST:
			case CASHITEM_GROUP_BULLET_PROOF_VEST_PLUS:
			case CASHITEM_GROUP_BULLET_PROOF_VEST_GM:
			case CASHITEM_GROUP_BULLET_PROOF_VEST_METAL:
				{
					INT32 checkItemID[] =
					{
						MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_BULLET_PROOF_VEST, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_BULLET_PROOF_VEST_PLUS, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_BULLET_PROOF_VEST_GM, CASHITEM_DAY_NO),
							MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_BULLET_PROOF_VEST_METAL, CASHITEM_DAY_NO),
					};
					const INT32 checkItemCount = GET_ARRAY_COUNT(checkItemID);

					for ( INT32 i = 0 ; i < checkItemCount ; ++i )
					{
						// 체크하지 않습니다.
						if ( GET_ITEM_FLAG_CLASS(checkItemID[i]) == GET_ITEM_FLAG_CLASS( nItemFlag) )
							continue;

						if ( g_pFramework->IsUsingCashItem( checkItemID[i]) )
						{
							CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_BY_DUPLICATE"),
								GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*"중복으로 사용할 수 없는 아이템입니다.".*//*아이템 사용 실패*/

							return FALSE;
						}
					}
				}
				break;
			}
		}
		break;
		//	소모성
	case ITEM_TYPE_EXPENDABLE:
		{
		}
		break;
	}

	return TRUE;
}

void CPopupUseItem::_UseItem(INT32 ItemID)
{
	m_AuthWareDBIndex = g_pFramework->GetInvenList()->GetCashItemWareDBIdx( ItemID);

	if( m_AuthWareDBIndex == INVALID_WAREDB_INDEX)
	{
		CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"),
			GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*사용할 수 없는 아이템입니다.*//*아이템 사용 실패*/

		return;
	}

	//	사용 가능한 아이템인지 예외 검사.
	if( !_CheckItem( ItemID))
	{
		return;
	}

	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(ItemID);
	I3ASSERT( pInfo != NULL);

	ITEM_USE_METHOD	Method = UseMethodStringToEnum(pInfo->GetUseMethod());
	switch (Method)
	{
	case USE_METHOD_DEFAULT_AUTH		:
		{
			_SetInputLock(TRUE);

			//	구입한 아이템의 인증을 요구
			g_pGameContext->SetEvent(EVENT_ITEM_AUTH, &m_AuthWareDBIndex);
		}
		break;
	case USE_METHOD_DISGUISE_NICK		:
		{
			//	닉네임 설정 창 띄움
			if( !_Open(USE_ITEM_DISGUISE_NICK))
				OnExitKey();
		}
		break;
	case USE_METHOD_DISGUISE_RANK		:
		{
			//	계급 설정 창 띄움
			if( !_Open(USE_ITEM_DISGUISE_RANK))
				OnExitKey();
		}
		break;
	case USE_METHOD_COLOR_NICK			:
		{
			if( !_Open(USE_ITEM_COLOR_NICK))
				OnExitKey();
		}
		break;
	case USE_METHOD_COLOR_CLAN			:
		{
			if( !_Open(USE_ITEM_COLOR_CLAN_NAME))
				OnExitKey();
		}
		break;
	case USE_METHOD_COLOR_CROSS_HAIR	:
		{
			if( !_Open(USE_ITEM_COLOR_CROSSHAIR))
				OnExitKey();
		}
		break;
	case USE_METHOD_CHANGE_NICK			:
		{
			//	'임시 닉네임' 아이템을 이미 사용중이라면 아이템 사용 불가
			INT32 itemDisguiseNick = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_NICK, CASHITEM_DAY_NO);

			if( g_pFramework->IsUsingCashItem( itemDisguiseNick))
			{
				OnClosePopup();

				//	임시 닉네임 적용 중에는 닉네임 변경권을 사용할 수 없습니다.\n임시 닉네임 아이템 삭제 후 닉네임을 변경하시기 바랍니다.
				CAPTION_POPUP(GAME_STRING("STR_POPUP_Q_MESSAGE_MODIFY_NICK_WITHOUT_DISUISE_NICK"),
					GAME_STRING("STR_INVEN_DO_NOT_USE"));
			}
			else	//	사용중이 아닐경우 바로 닉네임 변경 창으로
			{
				OnOpenChangeNick();
			}
		}
		break;
	case USE_METHOD_CHANGE_CLAN_NAME	:
		{
			if( !_Open(USE_ITEM_CHANGE_CLAN_NAME))
				OnExitKey();
		}
		break;
	case USE_METHOD_CHANGE_CLAN_MARK	:
		{
			CLAN_DETAIL_INFO * pClanInfo = g_pFramework->GetClanContext()->GetMyClanDetailInfo();
			UINT32 nOldMark = pClanInfo->_mark;

			OnClosePopup();

			//	클랜 마크 변경의 경우 현재 팝업이 아닌 다른 팝업(CPopupChangeClanMark)를 사용한다.
			((CStageReady*)m_pParent)->TogglePopup( POPUP_CHANGE_CLANMARK, &m_AuthWareDBIndex, &nOldMark);
		}
		break;
	case USE_METHOD_BATTLE_FIELD_MOVE	:
		{
			CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_USE_IN_GAME"),
				GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*게임중에만 사용할 수 있습니다.*//*아이템 사용 실패*/

			OnExitKey();
		}
		break;
	default:
		{
			CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"),
				GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));/*사용할 수 없는 아이템입니다.*//*아이템 사용 실패*/

			OnExitKey();
		}
		break;
	}

}

void CPopupUseItem::_UseItemOk(void)
{
	switch( m_Type)
	{
	case USE_ITEM_DISGUISE_NICK:
	case USE_ITEM_CHANGE_NICK:
	case USE_ITEM_CHANGE_CLAN_NAME:		OnChangeNameOk();			break;
	case USE_ITEM_COLOR_NICK:			_ChangeColorNickOk();		break;
	case USE_ITEM_COLOR_CLAN_NAME:		_ChangeColorClanNameOk();	break;
	case USE_ITEM_COLOR_CROSSHAIR:		_ChangeColorCrosshairOk();	break;
	case USE_ITEM_DISGUISE_RANK:		_ChangeDisguiseRankOk();	break;
	}
}

BOOL CPopupUseItem::_SetCurrentUseItemName(i3GuiEditBox * pControl)
{
	I3ASSERT( pControl != NULL);
	char Name[MAX_STRING_COUNT] = {};
	g_pShopItemInfoDataBase->GetItemName(m_ItemID, Name, MAX_STRING_COUNT);
	pControl->SetText( Name);
	return TRUE;
}

void CPopupUseItem::_SetEnablePopup(USE_ITEM_POPUP_TYPE type)
{
	switch( type)
	{
	case USE_ITEM_QUERY:
		{
			m_pQueryRoot->SetEnable(TRUE);
			m_pCompleteRoot->SetEnable(FALSE);
			m_pChangeNameRoot->SetEnable(FALSE);
			m_pChangeColorRoot->SetEnable(FALSE);
			m_pChangeRankRoot->SetEnable(FALSE);
			m_pChangeCrosshairColorRoot->SetEnable(FALSE);
		}
		break;
	case USE_ITEM_CHANGE_NICK:
	case USE_ITEM_CHANGE_CLAN_NAME:
	case USE_ITEM_DISGUISE_NICK:
		{
			m_pQueryRoot->SetEnable(FALSE);
			m_pCompleteRoot->SetEnable(FALSE);
			m_pChangeNameRoot->SetEnable(TRUE);
			m_pChangeColorRoot->SetEnable(FALSE);
			m_pChangeRankRoot->SetEnable(FALSE);
			m_pChangeCrosshairColorRoot->SetEnable(FALSE);
		}
		break;
	case USE_ITEM_COLOR_NICK:
	case USE_ITEM_COLOR_CLAN_NAME:
		{
			m_pQueryRoot->SetEnable(FALSE);
			m_pCompleteRoot->SetEnable(FALSE);
			m_pChangeNameRoot->SetEnable(FALSE);
			m_pChangeColorRoot->SetEnable(TRUE);
			m_pChangeRankRoot->SetEnable(FALSE);
			m_pChangeCrosshairColorRoot->SetEnable(FALSE);
		}
		break;
	case USE_ITEM_COLOR_CROSSHAIR:
		{
			m_pQueryRoot->SetEnable(FALSE);
			m_pCompleteRoot->SetEnable(FALSE);
			m_pChangeNameRoot->SetEnable(FALSE);
			m_pChangeColorRoot->SetEnable(FALSE);
			m_pChangeRankRoot->SetEnable(FALSE);
			m_pChangeCrosshairColorRoot->SetEnable(TRUE);
		}
		break;
	case USE_ITEM_DISGUISE_RANK:
		{
			m_pQueryRoot->SetEnable(FALSE);
			m_pCompleteRoot->SetEnable(FALSE);
			m_pChangeNameRoot->SetEnable(FALSE);
			m_pChangeColorRoot->SetEnable(FALSE);
			m_pChangeRankRoot->SetEnable(TRUE);
			m_pChangeCrosshairColorRoot->SetEnable(FALSE);
		}
		break;
	case USE_ITEM_COMPLETE:
		{
			m_pQueryRoot->SetEnable(FALSE);
			m_pCompleteRoot->SetEnable(TRUE);
			m_pChangeNameRoot->SetEnable(FALSE);
			m_pChangeColorRoot->SetEnable(FALSE);
			m_pChangeRankRoot->SetEnable(FALSE);
			m_pChangeCrosshairColorRoot->SetEnable(FALSE);
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_CHANGE_NICK

void CPopupUseItem::_InitChangeNickPopup(void)
{
	// 아이템
	_SetCurrentUseItemName( m_pChangeNameItemValue);

	m_pChangeNameBeforeStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_NICK_NAME"));/*현재 닉네임:*/

	// 닉네임
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	SetNameUsingColorTable(m_pChangeNameBeforeValue, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);

	m_pChangeNameUserStatic->SetText(GAME_STRING("STR_POPUP_MODIFIED_NICK"));/*변경 닉네임:*/

	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
	m_pChangeNameUserEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pChangeNameUserEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pChangeNameUserEdit->SetIMEText( "");
	m_pChangeNameUserEdit->SetFocus(TRUE);

	m_pChangeNameCheckStatic->SetTextColor( GetColor(GCT_DEFAULT));
	m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*중복검사를 해주십시오.*/

	m_pChangeNameNoticeValue->SetText(GAME_STRING("STR_POPUP_MESSAGE_MODIFY_NICK"));/*변경할 닉네임을 입력해주십시오.*/

	DEACTIVATE_BUTTON(m_pChangeNameButtonOk);

	m_bChangeNameChecked = FALSE;

	m_szChangeName[0] = 0;

	if(m_pStringFilter == NULL)
	{
		m_pStringFilter = CStringFilter::NewObject();
		m_pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	}

	if(m_pStringFilter2 == NULL)
	{
		m_pStringFilter2 = CStringFilter::NewObject();
		m_pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	}
}

void CPopupUseItem::_ChangeNickOk(void)
{
	_SetInputLock(TRUE);

	//	서버에 최종 변경할 닉네임을 보낸다.
	UINT8 ui8Size = static_cast<UINT8>(i3String::Length( m_szChangeName )+1);
	g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, m_szChangeName);
}

BOOL CPopupUseItem::OnChangeNameOk( void)
{
	//	현재 닉네임과 클랜명만 가능
	if( m_Type == USE_ITEM_CHANGE_NICK || m_Type == USE_ITEM_CHANGE_CLAN_NAME || m_Type == USE_ITEM_DISGUISE_NICK)
	{
		if(m_bChangeNameChecked == FALSE)
		{
			STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*중복검사를 해주십시오.*/

			return FALSE;
		}
		else
		{
			char szInputName[MAX_STRING_COUNT] = "";
			i3String::Copy( szInputName, m_pChangeNameUserEdit->getIMETextBuf(), MAX_STRING_COUNT );

			//	중복검사된 닉네임과 입력창의 닉네임이 다르면 다시 중복검사를 하도록 한다.
			if( i3String::Compare(m_szChangeName, szInputName) != 0)
			{
				m_bChangeNameChecked = FALSE;

				m_pChangeNameCheckStatic->SetTextColor( GetColor(GCT_DEFAULT));
				m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*중복검사를 해주십시오.*/

				m_pChangeNameUserEdit->SetFocus(TRUE);

				return FALSE;
			}

			switch( m_Type)
			{
			case USE_ITEM_CHANGE_NICK:			_ChangeNickOk();			break;
			case USE_ITEM_CHANGE_CLAN_NAME:		_ChangeClanNameOk();		break;
			case USE_ITEM_DISGUISE_NICK:		_ChangeDisguiseNickOk();	break;
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CPopupUseItem::OnChangeNameDuplicate(void)
{
	BOOL ret = TRUE;
	char szError[MAX_STRING_COUNT] = "";

	//	이미 체크가 끝난것이다.
	if( m_szChangeName[0] != 0 && i3String::Compare( m_szChangeName, m_pChangeNameUserEdit->getIMETextBuf()) == 0)
	{
		return TRUE;
	}

	m_bChangeNameChecked = FALSE;

	i3String::Copy( m_szChangeName, m_pChangeNameUserEdit->getIMETextBuf(), MAX_STRING_COUNT );

	if( i3String::Length(m_szChangeName) <= 0)
	{
		switch( m_Type)
		{
		case USE_ITEM_CHANGE_NICK:		i3String::Copy(szError, GAME_STRING("STR_POPUP_MESSAGE_PLEASE_NAME"), MAX_STRING_COUNT);/*"닉네임을 적어주세요."*/	break;
		case USE_ITEM_CHANGE_CLAN_NAME:	i3String::Copy(szError, GAME_STRING("STR_POPUP_MESSAGE_INPUT_CLAN_NAME"), MAX_STRING_COUNT);/*클랜명을 적어주세요.*/	break;
		case USE_ITEM_DISGUISE_NICK:	i3String::Copy(szError, GAME_STRING("STR_POPUP_MESSAGE_INPUT_DISGUISED_NICK_1"), MAX_STRING_COUNT);/*임시 닉네임을 적어주세요.*/break;
		}

		ret = FALSE;

		goto LABEL_CHECK_NAME_EXIT;
	}

	// 닉네임만 최소글자수 적용
	if( m_Type == USE_ITEM_CHANGE_NICK || m_Type == USE_ITEM_DISGUISE_NICK)
	{
		if (FALSE == CStringFilter::IsValidNickname(m_szChangeName))
		{
			//  최소 글자수 포함되도록 내용 수정해야합니다.
			i3String::Copy( szError, GAME_STRING("STBL_IDX_EP_LOBBY_NICK_LENGTH"), MAX_STRING_COUNT );/*2자에서 16자 사이로 설정하여 주시기 바랍니다.*/

			ret = FALSE;

			goto LABEL_CHECK_NAME_EXIT;
		}
	}

	// 유효성 검사
	{
		if ( ! g_pFramework->CheckValidNickname(m_szChangeName) )
		{
			//	입력 할 수 없는 단어가 포함되어 있습니다"
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
			ret = FALSE;
			goto LABEL_CHECK_NAME_EXIT;
		}
	}

LABEL_CHECK_NAME_EXIT:
	//	기본 검사
	if( ret)
	{
		switch( m_Type)
		{
		case USE_ITEM_DISGUISE_NICK:
		case USE_ITEM_CHANGE_NICK:			g_pGameContext->SetEvent(EVENT_USE_ITEM_CHECK_NICK, m_szChangeName);		break;
		case USE_ITEM_CHANGE_CLAN_NAME:		g_pFramework->GetClanContext()->SendOverlappingClanName(m_szChangeName);	break;
		}

		return TRUE;
	}
	else
	{
		//m_pChangeNameUserEdit->SetIMEText( "");
		m_pChangeNameUserEdit->SetFocus(TRUE);

		m_pChangeNameCheckStatic->SetText(szError);
		m_pChangeNameCheckStatic->SetTextColor(GetColor(GCT_DEFAULT));

		return FALSE;
	}
}

void CPopupUseItem::OnOpenChangeNick(void)
{
	if( !_Open(USE_ITEM_CHANGE_NICK))
		OnExitKey();
}

void CPopupUseItem::CbChangeNickWarnning(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		((CPopupUseItem*)pThis)->OnOpenChangeNick();
	}
	else
	{
		((CPopupUseItem*)pThis)->OnCancel();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_CHANGE_CLAN_NAME

void CPopupUseItem::_InitChangeClanNamePopup(void)
{
	// 아이템
	_SetCurrentUseItemName( m_pChangeNameItemValue);

	m_pChangeNameBeforeStatic->SetText(GAME_STRING("STR_POPUP_CURRNET_CLAN_NAME"));/*현재 클랜명:*/

	// 클랜명
	char * pszName = g_pGameContext->getMyClanDetailInfo()->_name;
	m_pChangeNameBeforeValue->SetText(pszName);

	m_pChangeNameUserStatic->SetText(GAME_STRING("STR_POPUP_MODIFIED_CLAN_NAME"));/*변경 클랜명:*/

	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NET_CLAN_NAME_SIZE - 1;
	ime._StyleIME = 0;
	m_pChangeNameUserEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pChangeNameUserEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pChangeNameUserEdit->SetIMEText( "");
	m_pChangeNameUserEdit->SetFocus(TRUE);

	m_pChangeNameCheckStatic->SetTextColor( GetColor(GCT_DEFAULT));
	m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*중복검사를 해주십시오.*/

	m_pChangeNameNoticeValue->SetText(GAME_STRING("STR_POPUP_MESSAGE_INPUT_MODIFY_CLAN_NAME"));/*변경할 클랜명을 입력해주십시오.*/

	DEACTIVATE_BUTTON(m_pChangeNameButtonOk);

	m_bChangeNameChecked = FALSE;

	m_szChangeName[0] = 0;

	if(m_pStringFilter == NULL)
	{
		m_pStringFilter = CStringFilter::NewObject();
		m_pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	}

	if(m_pStringFilter2 == NULL)
	{
		m_pStringFilter2 = CStringFilter::NewObject();
		m_pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	}
}

void CPopupUseItem::_ChangeClanNameOk(void)
{
	_SetInputLock(TRUE);

	//	서버에 최종 변경할 닉네임을 보낸다.
	g_pFramework->GetClanContext()->ProcessChangeClanName(m_AuthWareDBIndex, m_szChangeName);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_DISGUISE_NICK

void CPopupUseItem::_InitDisguiseNickPopup(void)
{
	// 아이템
	_SetCurrentUseItemName( m_pChangeNameItemValue);

	m_pChangeNameBeforeStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_NICK_NAME"));/*현재 닉네임:*/

	// 닉네임
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	SetNameUsingColorTable(m_pChangeNameBeforeValue, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);

	m_pChangeNameUserStatic->SetText(GAME_STRING("STR_POPUP_DISGUISED_NICK"));/*임시 닉네임:*/

	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
	m_pChangeNameUserEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pChangeNameUserEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pChangeNameUserEdit->SetIMEText( "");
	m_pChangeNameUserEdit->SetFocus(TRUE);

	m_pChangeNameCheckStatic->SetTextColor( GetColor(GCT_DEFAULT));
	m_pChangeNameCheckStatic->SetText(GAME_STRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*중복검사를 해주십시오.*/

	m_pChangeNameNoticeValue->SetText(GAME_STRING("STR_POPUP_MESSAGE_INPUT_DISGUISED_NICK_2"));/*임시 닉네임을 입력해주십시오.*/

	DEACTIVATE_BUTTON(m_pChangeNameButtonOk);

	m_bChangeNameChecked = FALSE;

	m_szChangeName[0] = 0;

	if(m_pStringFilter == NULL)
	{
		m_pStringFilter = CStringFilter::NewObject();
		m_pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	}

	if(m_pStringFilter2 == NULL)
	{
		m_pStringFilter2 = CStringFilter::NewObject();
		m_pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	}
}

void CPopupUseItem::_ChangeDisguiseNickOk(void)
{
	_SetInputLock(TRUE);

	//	서버에 작성된 임시 닉네임을 보낸다.
	UINT8 ui8Size = static_cast<UINT8>(i3String::Length( m_szChangeName )+1);
	g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, m_szChangeName);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_DISGUISE_RANK

void CPopupUseItem::_InitDisguiseRankPopup(void)
{
	// 아이템
	_SetCurrentUseItemName( m_pChangeRankItemValue);

	m_pChangeRankCurrentStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_RANK"));/*현재 계급:*/

	// 현재 계급
	const char * pszCurRank = GetCharaRank(g_pGameContext->GetMyRank());
	m_pChangeRankCurrentValue->SetText(pszCurRank);

	m_pChangeRankSelectStatic->SetText(GAME_STRING("STR_POPUP_DISGUISE_RANK"));/*위장 계급:*/
	const char * pszSelected = m_pChangeRankSelectList->GetText( m_pChangeRankSelectList->getSelectedElement());
	m_pChangeRankSelectCombo->SetText( pszSelected);
}

void CPopupUseItem::_ChangeDisguiseRankOk(void)
{
	switch( m_pChangeRankSelectList->getSelectedElement())
	{
	case 0:		m_nSelected = 0;		break;	//	훈련병
	case 1:		m_nSelected = 1;		break;	//	이병
	case 2:		m_nSelected = 2;		break;	//	일병
	case 3:		m_nSelected = 3;		break;	//	상병
	case 4:		m_nSelected = 4;		break;	//	병장
	default:	m_nSelected = -1;		break;
	}

	_SetInputLock(TRUE);

	//	위장 계급을 보낸다.
	UINT8 ui8Size = sizeof(INT32);
	g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &m_nSelected);
}

void CPopupUseItem::OnChangeRankPopupButton(void)
{
	ExclusionPopup(m_pChangeRankSelectList);
}

void CPopupUseItem::OnChangeRankPopupList(UINT32 idxItem)
{
	char * pszSelected = (char*) m_pChangeRankSelectList->GetText( m_pChangeRankSelectList->getSelectedElement());
	m_pChangeRankSelectCombo->SetText( pszSelected);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_COLOR_NICK

void CPopupUseItem::_InitColorNickPopup(void)
{
	m_pChangeColorCurrentStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_COLOR"));/*현재 색상:*/
	m_pChangeColorSelectStatic->SetText(GAME_STRING("STR_POPUP_MODIFIED_COLOR"));/*변경 색상:*/
	m_pChangeColorApplyStatic->SetText(GAME_STRING("STR_POPUP_APPLIED_NICK_NAME"));/*적용 닉네임:*/

	_SetCurrentUseItemName( m_pChangeColorItemValue);

	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	SetNameUsingColorTable(m_pChangeColorApplyValue, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);

	m_pChangeColorCurrentColorBox->SetEnable( FALSE);
	m_pChangeColorSelectColorBoxFrame->SetEnable( FALSE);

	for(INT32 i=0; i<SELECT_COLOR_BOX_COUNT; i++)
	{
		m_pChangeColorSelectColorBox[ i]->SetEnable( TRUE);
		m_pChangeColorSelectColorBox[ i]->SetAllShapeEnable(FALSE);
		m_pChangeColorSelectColorBox[ i]->SetShapeEnable( i, TRUE);
	}

	INT8 idxColor = MyUserInfoBasic._nickcolor;
	if( idxColor == 0)
		OnChangeColorSelect( -1);	//	기본 흰색
	else
		OnChangeColorSelect( idxColor-1);	//	서버는 1부터 색상, GUI는 0부터 색상
}

void CPopupUseItem::_ChangeColorNickOk(void)
{
	if( m_nSelected == -1)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));/*변경할 색상을 선택해 주십시오.*/
	}
	else
	{
		INT32 idx = m_nSelected + 1;	//	GUI는 -1 베이스이며, 서버는 0 베이스이다. 주의할것!

		_SetInputLock(TRUE);

		UINT8 ui8Size = sizeof(INT32);
		g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &idx);
	}
}

void CPopupUseItem::OnChangeColorSelect(INT32 idx)
{
	I3ASSERT( idx >= -1 && idx < SELECT_COLOR_BOX_COUNT);	//	-1번은 흰색, 0~9번까지 색상

	I3COLOR * pSelectColor = NULL;

	m_nSelected = idx;

	if(m_nSelected == -1)
	{
		//	선택되지 않은 상태 또는 기본 상태
		m_pChangeColorSelectColorBoxFrame->SetEnable( FALSE);
		m_pChangeColorCurrentColorBox->SetEnable( FALSE);

		//	기본 컬러
		pSelectColor = (I3COLOR*)GetColorCashItem(0);
	}
	else
	{
		//	선택된 컬러 박스의 위치로 선택 커서를 맞춘다.
		INT32 idxColor = m_nSelected+1;	//	idxColor는 1 base이다.
		REAL32 slotPos = m_pChangeColorSelectColorBox[ m_nSelected]->getPositionX();

		REAL32 offsetWidth = (REAL32) (m_pChangeColorSelectColorBoxFrame->getOriginalWidth() -
			m_pChangeColorSelectColorBox[ m_nSelected]->getOriginalWidth()) * 0.5f;

		m_pChangeColorSelectColorBoxFrame->SetEnable( TRUE);
		m_pChangeColorSelectColorBoxFrame->setPositionX( slotPos - offsetWidth);

		m_pChangeColorCurrentColorBox->SetEnable( TRUE);
		m_pChangeColorCurrentColorBox->SetAllShapeEnable(FALSE);
		m_pChangeColorCurrentColorBox->SetShapeEnable( m_nSelected, TRUE);

		//	선택 컬러
		pSelectColor = (I3COLOR*)GetColorCashItem(idxColor);
	}

	m_pChangeColorApplyValue->SetTextColor( pSelectColor);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_COLOR_CLAN_NAME

void CPopupUseItem::_InitColorClanNamePopup(void)
{
	m_pChangeColorCurrentStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_COLOR"));/*현재 색상:*/
	m_pChangeColorSelectStatic->SetText(GAME_STRING("STR_POPUP_MODIFIED_COLOR"));/*변경 색상:*/
	m_pChangeColorApplyStatic->SetText(GAME_STRING("STR_POPUP_APPLIED_CLAN_NAME"));/*적용 클랜명:*/

	_SetCurrentUseItemName( m_pChangeColorItemValue);

	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	SetNameUsingColorTable( m_pChangeColorApplyValue, MyUserInfoBasic._clanName, MyUserInfoBasic._clanNamecolor );

	INT8 idxColor = MyUserInfoBasic._clanNamecolor;
	if( idxColor == 0)
		OnChangeColorSelect( -1);	//	기본 흰색
	else
		OnChangeColorSelect( idxColor-1);	//	서버는 1부터 색상, GUI는 0부터 색상
}

void CPopupUseItem::_ChangeColorClanNameOk(void)
{
	if( m_nSelected == -1)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));/*변경할 색상을 선택해 주십시오.*/
	}
	else
	{
		INT32 idx = m_nSelected + 1;	//	GUI는 -1 베이스이며, 서버는 0 베이스이다. 주의할것!

		_SetInputLock(TRUE);

		UINT8 ui8Size = sizeof(INT32);
		g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &idx);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//	USE_ITEM_COLOR_CROSSHAIR

void CPopupUseItem::_InitColorCrosshairPopup(void)
{
	m_pChangeCrosshairColorCurrentStatic->SetText(GAME_STRING("STR_POPUP_CURRENT_COLOR"));/*현재 색상:*/
	m_pChangeCrosshairColorSelectStatic->SetText(GAME_STRING("STR_POPUP_MODIFIED_COLOR"));/*변경 색상:*/

	_SetCurrentUseItemName( m_pChangeCrosshairColorItemValue);

	USER_INFO_INVITEM_DATA Data;
	g_pGameContext->GetMyUserInfoInvItemData(&Data);

	INT8 idxColor = Data._cColorCrossHair;
	if( idxColor == 0)
		OnChangeCrosshairColorSelect( -1);	//	기본 흰색
	else
		OnChangeCrosshairColorSelect( idxColor-1);	//	서버는 1부터 색상, GUI는 0부터 색상
}

void CPopupUseItem::_ChangeColorCrosshairOk(void)
{
	if( m_nSelected == -1)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));/*변경할 색상을 선택해 주십시오.*/
	}
	else
	{
		INT32 idx = m_nSelected + 1;	//	GUI는 -1 베이스이며, 서버는 0 베이스이다. 주의할것!

		_SetInputLock(TRUE);

		UINT8 ui8Size = sizeof(INT32);
		g_pGameContext->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &idx);
	}
}

void CPopupUseItem::OnChangeCrosshairColorSelect(INT32 idx)
{
	I3ASSERT( idx >= -1 && idx < SELECT_CROSSHAIR_COLOR_BOX_COUNT);	//	-1번은 흰색, 0~9번까지 색상

	I3COLOR * pSelectColor = NULL;

	m_nSelected = idx;

	if(m_nSelected == -1)
	{
		//	선택되지 않은 상태 또는 기본 상태
		m_pChangeCrosshairColorSelectColorBoxFrame->SetEnable( FALSE);
		m_pChangeCrosshairColorCurrentColorBox->SetEnable( FALSE);

		//	기본 컬러
		pSelectColor = (I3COLOR*)GetColorCashItem(0);
	}
	else
	{
		//	선택된 컬러 박스의 위치로 선택 커서를 맞춘다.
		INT32 idxColor = m_nSelected+1;	//	idxColor는 1 base이다.

		REAL32 slotPos = m_pChangeCrosshairColorSelectColorBox[ m_nSelected]->getPositionX();

		REAL32 offsetWidth = (REAL32) (m_pChangeCrosshairColorSelectColorBoxFrame->getOriginalWidth() -
			m_pChangeCrosshairColorSelectColorBox[ m_nSelected]->getOriginalWidth()) * 0.5f;

		m_pChangeCrosshairColorSelectColorBoxFrame->SetEnable( TRUE);
		m_pChangeCrosshairColorSelectColorBoxFrame->setPositionX( slotPos - offsetWidth);

		m_pChangeCrosshairColorCurrentColorBox->SetEnable( TRUE);
		m_pChangeCrosshairColorCurrentColorBox->SetAllShapeEnable(FALSE);
		m_pChangeCrosshairColorCurrentColorBox->SetShapeEnable( m_nSelected, TRUE);

		//	선택 컬러
		pSelectColor = (I3COLOR*)GetColorCashItem(idxColor);
	}

	//	크로스헤어 이미지 색상 적용
	m_pChangeCrosshairColorCrossImage->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
	m_pChangeCrosshairColorCrossDotImage->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
	m_pChangeCrosshairColorDotImage->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
	m_pChangeCrosshairColorCircleImage->SetColor( I3GUI_CONTROL_STATE_NORMAL, pSelectColor);
}


#endif	// ENABLE_UI2
