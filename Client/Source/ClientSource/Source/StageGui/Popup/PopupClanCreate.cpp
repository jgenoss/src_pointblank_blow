#include "pch.h"
#include "GameDefine.h"
#include "ClanDef.h"
#include "ClanContext.h"
#include "GlobalVariables.h"
#include "PopupClanCreate.h"
#include "GameContext.h"
#include "ClanMark.h"

I3_CLASS_INSTANCE( CPopupClanCreate, CPopupBase);

CPopupClanCreate::CPopupClanCreate(void)
{
	m_pStringFilter = NULL;
	m_pStringFilter2 = NULL;

	//Create Popup
	m_pClanCreate						= NULL;
	m_pClanCreateCaption				= NULL;	// 클랜 생성
	m_pClanCreateOk						= NULL;	// 클랜생성 확인
	m_pClanCreateCancel					= NULL;	// 클랜생성 취소
	m_pClanCreateGuideValue				= NULL;	// 클랜생성 도움말

	m_pClanInputInfo					= NULL;	// 클랜정보 입력
	m_pClanInputInfoCaption				= NULL;	// 클랜 기본정보 입력
	m_pClanInputInfoOk					= NULL;	// 클랜 기본정보 입력 확인
	m_pClanInputInfoCancel				= NULL;	// 클랜 기본정보 입력 취소

	m_pClanInputInfoClanName			= NULL;	// 클랜명
	m_pClanInputInfoClanNameEdit		= NULL;	// 클랜명 입력
	m_pClanInputInfoClanNameCheck		= NULL;	// 클랜명 중복 체크
	m_pClanInputInfoClanNameGuide		= NULL;	// (한글 8자, 영문 12자)
	m_pClanInputInfoClanIntroduce		= NULL;	// 클랜소개( 한글 120자, ...)
	m_pClanInputInfoClanIntroduceValue	= NULL;	// 클랜소개글 입력

	m_pClanInputInfoClanAddressGuideStatic = NULL;
	m_pClanInputInfoClanAddressStatic = NULL;
	m_pClanInputInfoClanAddressEdit = NULL;
	m_pClanInputInfoClanAddressBox = NULL;
	m_pClanInputInfoClanAddressCheck = NULL;

	m_pOverlappingCheck					= NULL;
	m_pOverlappingCheckCaption			= NULL;	// 클랜명 중복 검사
	m_pOverlappingCheckOk				= NULL;	// 클랜명 중복검사 확인
	m_pOverlappingCheckCancel			= NULL;	// 클랜명 중복검사 취소
	m_pOverlappingCheckCheck			= NULL;	// 중복검사 버튼

	m_pOverlappingCheckInput			= NULL;	// 다른 클랜명 입력
	m_pOverlappingCheckInsertedName		= NULL;	// 중복으로 판명된 클랜명
	m_pOverlappingCheckInsertedNameGuide= NULL;	// 는 이미 사용중이거나 사용할수 없는 이름입니다....
	m_pOverlappingCheckInputEdit		= NULL;	// 다른 클랜명 입력

	m_pOverlappingAddress				= NULL;

	m_pCreateClanMarkGui = NULL;
	m_pCreateClanMarkCaption = NULL;
	m_pCreateClanMarkSelectStatic = NULL;
	m_pCreateClanMarkBackColorStatic = NULL;
	m_pCreateClanMarkShapeColorStatic = NULL;
	m_pCreateClanMarkButtonOk = NULL;
	m_pCreateClanMarkButtonCancel = NULL;
	m_pCreateClanMarkButtonOverlappingCheck = NULL;
	m_pCreateClanMarkConfirmImage = NULL;

	i3mem::FillZero(m_pCreateClanMarkPrev, sizeof(m_pCreateClanMarkPrev));
	i3mem::FillZero(m_pCreateClanMarkNext, sizeof(m_pCreateClanMarkNext));

	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
		for(INT32 j=0; j<CREATE_CLANMARK_SHAPE_SLOT_MAX; j++)
			m_pCreateClanMarkSelectImage[i][j] = NULL;

	for(INT32 i=0; i<CREATE_CLANMARK_COLOR_SLOT_MAX; i++)
	{
		m_pCreateClanMarkBackColorBox[i] = NULL;
		m_pCreateClanMarkShapeColorBox[i] = NULL;
	}

	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
	{
		for(INT32 j=0; j<CREATE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			m_pTexClanMarkSelect[i][j] = NULL;
		}
	}

	m_pTexClanMarkFinal = NULL;

	m_nClanMarkBackImageIdx = 1;
	m_nClanMarkShapeImageIdx = 1;
	m_nClanMarkBackColorIdx = 1;
	m_nClanMarkShapeColorIdx = 1;

	m_pSubScription						= NULL;
	m_pSubScriptionCaption				= NULL;	// 클랜가입요청
	m_pSubScriptionOk					= NULL;	// 클랜 가입 확인
	m_pSubScriptionCancel				= NULL;	// 클랜 가입 취소

	m_pSubScriptionReceiver				= NULL;	// To. 
	m_pSubScriptionReceiverValue		= NULL;	// 대상 클랜명
	m_pSubScriptionSenderNick			= NULL;	// 닉네임
	m_pSubScriptionSenderRank			= NULL;	// 발신자 등급
	m_pSubScriptionSenderNickValue		= NULL;	// 발신자 닉네임 

	m_pSubScriptionSenderWin			= NULL;	// 승수 :
	m_pSubScriptionSenderWinValue		= NULL;	// 발신자 승수
	m_pSubScriptionSenderScore			= NULL;	// 전적	:
	m_pSubScriptionSenderScoreValue		= NULL;	// 발신자 전적

	m_pSubScriptMessageStatic			= NULL;	// 가입 신청 메시지*( 100자이내)
	m_pSubScriptMessageValue			= NULL;	// 가입 신청 메시지 입력창

	m_nCurClanIndex						= -1;

	m_bWaitingConfirmSubscripting		= FALSE;
	m_bWaitingConfirmCreateClan			= FALSE;
	m_bWaitingIsDuplicateName			= FALSE;
	m_bWaitingIsDuplicateAddress		= FALSE;
	m_bWaitingIsDuplicateMark			= FALSE;
	m_bCheckingName						= FALSE;
	m_bCheckingAddress					= FALSE;

	m_bCheckedName = FALSE;
	m_bCheckedAddress = FALSE;
	m_bCheckedClanMark = FALSE;
	m_bCreateConfirm = FALSE;
	m_bJumpToClanMarkStep = FALSE;
}

CPopupClanCreate::~CPopupClanCreate(void)
{
	I3_SAFE_RELEASE(m_pStringFilter);
	I3_SAFE_RELEASE(m_pStringFilter2);
	
	_CleanupTexture();
}
	
void CPopupClanCreate::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_CLAN_POPUP_CREATE_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateConfirm();
		break;
	case GCI_CLAN_POPUP_CREATE_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateCancel();
		break;
	case GCI_CLAN_POPUP_INPUTINFO_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputOK();
		break;
	case GCI_CLAN_POPUP_INPUTINFO_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCancel();
		break;
	case GCI_CLAN_POPUP_INPUTINFO_CHECKNICK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCheckNick();
		break;	
	case GCI_CLAN_POPUP_INPUTINFO_ADDRESS_CHECK:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCheckClanAddress();
		break;
	case GCI_CLAN_POPUP_OVERLAPPING_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingOK();
		break;
	case GCI_CLAN_POPUP_OVERLAPPING_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingCancel();
		break;
	case GCI_CLAN_POPUP_OVERLAPPING_CHECKNICK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingCheck();
		break;	
	case GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingAddressOK();
		break;
	case GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingAddressCancel();
		break;
	case GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_CHECK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _OverLappingAddressCheck();
		break;	
	case GCI_CLAN_POPUP_SUBSCRIPTION_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _SubscriptionOK();
		break;
	case GCI_CLAN_POPUP_SUBSCRIPTION_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _SubScriptionCancel();
		break;
	//	클랜 마크
	case GCI_CLAN_B_POPUP_CLANMARK_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkOK();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkCancel();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_OVERLAPPING_CHECK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkOverlappingCheck();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_GROUP_PREV1:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkPrevBackImage();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_GROUP_PREV2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkPrevShapeImage();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_GROUP_NEXT1:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkNextBackImage();
		break;
	case GCI_CLAN_B_POPUP_CLANMARK_GROUP_NEXT2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkNextShapeImage();
		break;
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR1:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR2:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR3:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR4:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR5:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR6:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR7:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR8:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR9:
	case GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkChangeBackColor( pNotify->m_nID - GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR1 + 1);
		break;
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR1:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR2:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR3:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR4:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR5:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR6:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR7:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR8:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR9:
	case GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) _CreateClanMarkChangeShapeColor( pNotify->m_nID - GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR1 + 1);
		break;
	}
}

BOOL CPopupClanCreate::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist,  "ClanCreate", m_pClanCreateRoot, -1);

	//////////////////////////////////////////	클랜 생성 안내
	LinkControlEx(plist, 	"clanCreatePopup", m_pClanCreate, -1);
	{
		LinkControlEx(plist,  "clanCreateCaption", m_pClanCreateCaption, -1);								// 클랜 생성
		LinkControlEx(plist,  "clanCreateOk", m_pClanCreateOk, GCI_CLAN_POPUP_CREATE_OK);					// 클랜생성 확인
		LinkControlEx(plist,  "clanCreateCancel", m_pClanCreateCancel, GCI_CLAN_POPUP_CREATE_CANCEL);		// 클랜생성 취소
		LinkControlEx(plist,  "clanCreateGuideValue", m_pClanCreateGuideValue, -1);							// 클랜생성 도움말
	}

	//////////////////////////////////////////	클랜 생성 정보 입력
	LinkControlEx(plist, 	"clanInputInfoPopup", m_pClanInputInfo, -1);								// 클랜정보 입력
	{
		LinkControlEx(plist, "clanInputInfoCaption", m_pClanInputInfoCaption, -1);						// 클랜 기본정보 입력
		LinkControlEx(plist, "clanInputInfoOk", m_pClanInputInfoOk, GCI_CLAN_POPUP_INPUTINFO_OK);		// 클랜 기본정보 입력 확인
		LinkControlEx(plist, "clanInputInfoCancel", m_pClanInputInfoCancel, GCI_CLAN_POPUP_INPUTINFO_CANCEL);			// 클랜 기본정보 입력 취소

		LinkControlEx(plist, "clanInputInfoClanNameStatic", m_pClanInputInfoClanName, -1);				// 클랜명
		LinkControlEx(plist, "clanInputInfoClanNameEdit", m_pClanInputInfoClanNameEdit, -1);			// 클랜명 입력
		LinkControlEx(plist, "clanInputInfoClanNameCheck", m_pClanInputInfoClanNameCheck, GCI_CLAN_POPUP_INPUTINFO_CHECKNICK);	// 클랜명 중복 체크
		LinkControlEx(plist, "clanInputInfoClanNameGuideStatic", m_pClanInputInfoClanNameGuide, -1);	// (한글 8자, 영문 12자)
		LinkControlEx(plist, "clanInputInfoClanIntroduceStatic", m_pClanInputInfoClanIntroduce, -1);	// 클랜소개( 한글 120자, ...)
		LinkControlEx(plist, "clanInpuIntroduceValue", m_pClanInputInfoClanIntroduceValue, -1);			// 클랜소개글 입력
		LinkControlEx(plist, "clanInputInfoClanAddressGuideStatic", m_pClanInputInfoClanAddressGuideStatic, -1);	//	클랜 아지트 주소 입력 안내
		LinkControlEx(plist, "clanInputInfoClanAddressStatic", m_pClanInputInfoClanAddressStatic, -1);	//	클랜 아지트 기본 주소
		LinkControlEx(plist, "clanInputInfoClanAddressEdit", m_pClanInputInfoClanAddressEdit, -1);		//	클랜 아지트 유저 주소 입력
		LinkControlEx(plist, "clanInputInfoClanAddressCheck", m_pClanInputInfoClanAddressCheck, GCI_CLAN_POPUP_INPUTINFO_ADDRESS_CHECK);	//	클랜 아지트 주소 중복 검사 버튼
		LinkControlEx(plist,  "clanInputInfoClanAddressBox", m_pClanInputInfoClanAddressBox, -1);		
	}

	//////////////////////////////////////////	클랜 이름 중복 체크
	LinkControlEx(plist, 	"clanOverlappingCheckPopup", m_pOverlappingCheck, -1);
	{
		LinkControlEx(plist,  "clanOverlappingCheckCaption", m_pOverlappingCheckCaption, -1);									// 클랜명 중복 검사
		LinkControlEx(plist,  "clanOverlappingCheckOk", m_pOverlappingCheckOk, GCI_CLAN_POPUP_OVERLAPPING_OK);				// 클랜명 중복검사 확인
		LinkControlEx(plist,  "clanOverlappingCheckCancel", m_pOverlappingCheckCancel, GCI_CLAN_POPUP_OVERLAPPING_CANCEL);	// 클랜명 중복검사 취소
		LinkControlEx(plist,  "clanOverlappingCheckButton", m_pOverlappingCheckCheck, GCI_CLAN_POPUP_OVERLAPPING_CHECKNICK);	// 중복검사 버튼

		LinkControlEx(plist,  "clanOverlappingCheckInputStatic", m_pOverlappingCheckInput, -1);									// 다른 클랜명 입력
		LinkControlEx(plist,  "clanOverlappingCheckStatic1", m_pOverlappingCheckInsertedName, -1);								// 중복으로 판명된 클랜명
		LinkControlEx(plist,  "clanOverlappingCheckStatic2", m_pOverlappingCheckInsertedNameGuide, -1);							// 는 이미 사용중이거나 사용할수 없는 이름입니다....
		LinkControlEx(plist,  "clanOverlappingCheckInputEdit", m_pOverlappingCheckInputEdit, -1);								// 다른 클랜명 입력
	}

	//////////////////////////////////////////	URL 중복 체크
	LinkControlEx(plist,  "clanOverlappingAddressPopup", m_pOverlappingAddress, -1);
	{
		LinkControlEx(plist, "clanOverlappingAddressCaption", m_pOverlappingAddressCaption, -1);
		LinkControlEx(plist, "clanOverlappingAddressOk", m_pOverlappingAddressOk, GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_OK);
		LinkControlEx(plist, "clanOverlappingAddressCancel", m_pOverlappingAddressCancel, GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_CANCEL);
		LinkControlEx(plist, "clanOverlappingAddressCheck", m_pOverlappingAddressCheck, GCI_CLAN_POPUP_ADDRESS_OVERLAPPING_CHECK);

		LinkControlEx(plist, "clanOverlappingAddressInputStatic", m_pOverlappingAddressInputStatic, -1);
		LinkControlEx(plist, "clanOverlappingAddressStatic1", m_pOverlappingAddressInsertedName, -1);
		LinkControlEx(plist, "clanOverlappingAddressStatic2", m_pOverlappingAddressInsertedNameGuide, -1);
		LinkControlEx(plist, "clanOverlappingAddressURL", m_pOverlappingAddressURL, -1);
		LinkControlEx(plist, "clanOverlappingAddressInputEdit", m_pOverlappingAddressInputEdit, -1);
	}

	//////////////////////////////////////////	클랜 마크 생성
	LinkControlEx(plist,  "clanCreateClanMarkPopup", m_pCreateClanMarkGui, GCI_UNKNOWN);
	{
		LinkControlEx(plist,  "clanCreateClanMarkPopupCaption", m_pCreateClanMarkCaption, GCI_UNKNOWN);

		LinkControlEx(plist,  "clanCreateClanMarkSelectStatic", m_pCreateClanMarkSelectStatic, GCI_UNKNOWN);
		LinkControlEx(plist,  "clanCreateClanMarkBackColorStatic", m_pCreateClanMarkBackColorStatic, GCI_UNKNOWN);
		LinkControlEx(plist,  "clanCreateClanMarkShapeColorStatic", m_pCreateClanMarkShapeColorStatic, GCI_UNKNOWN);

		LinkControlEx(plist,  "clanCreateClanMarkOk", m_pCreateClanMarkButtonOk, GCI_CLAN_B_POPUP_CLANMARK_OK);
		LinkControlEx(plist,  "clanCreateClanMarkCancel", m_pCreateClanMarkButtonCancel, GCI_CLAN_B_POPUP_CLANMARK_CANCEL);
		LinkControlEx(plist,  "clanCreateClanMarkOverlappingCheck", m_pCreateClanMarkButtonOverlappingCheck, GCI_CLAN_B_POPUP_CLANMARK_OVERLAPPING_CHECK);

		LinkControlEx(plist,  "clanCreateClanMarkConfirmImage", m_pCreateClanMarkConfirmImage, GCI_UNKNOWN);

		char temp[256] = "";
		for(INT32 i=0; i<2; i++)
		{
			sprintf_s( temp, "clanCreateClanMarkPrev%d", i+1);
			LinkControlEx(plist,  temp, m_pCreateClanMarkPrev[i], GCI_CLAN_B_POPUP_CLANMARK_GROUP_PREV1 + i);

			sprintf_s( temp, "clanCreateClanMarkNext%d", i+1);
			LinkControlEx(plist,  temp, m_pCreateClanMarkNext[i], GCI_CLAN_B_POPUP_CLANMARK_GROUP_NEXT1 + i);

			for(INT32 j=0; j<5; j++)
			{
				sprintf_s( temp, "clanCreateClanMarkSelectImage%d%d", i+1, j+1);
				LinkControlEx(plist,  temp, m_pCreateClanMarkSelectImage[i][j], GCI_UNKNOWN);
			}
		}
	
		LinkControlEx(plist,  "clanCreateClanMarkBackColorStatic", m_pCreateClanMarkBackColorStatic, GCI_UNKNOWN);
		LinkControlEx(plist,  "clanCreateClanMarkBackColorSelected", m_pCreateClanMarkBackColorSelected, GCI_UNKNOWN);
		for(INT32 i=0; i<10; i++)
		{
			sprintf_s(temp, "clanCreateClanMarkBackColorBox%d", i+1);
			LinkControlEx(plist,  temp, m_pCreateClanMarkBackColorBox[i], GCI_CLAN_S_POPUP_CLANMARK_BACK_COLOR1 + i);
		}

		LinkControlEx(plist,  "clanCreateClanMarkShapeColorStatic", m_pCreateClanMarkShapeColorStatic, GCI_UNKNOWN);
		LinkControlEx(plist,  "clanCreateClanMarkShapeColorSelected", m_pCreateClanMarkShapeColorSelected, GCI_UNKNOWN);
		for(INT32 i=0; i<10; i++)
		{
			sprintf_s(temp, "clanCreateClanMarkShapeColorBox%d", i+1);
			LinkControlEx(plist,  temp, m_pCreateClanMarkShapeColorBox[i], GCI_CLAN_S_POPUP_CLANMARK_SHAPE_COLOR1 + i);
		}
	}

	////////////////////////////////////////// 클랜 가입 
	LinkControlEx(plist, 	"clanSubscriptionPopup", m_pSubScription, -1);
	{
		LinkControlEx(plist,  "clanSubscriptionPopupCaption",m_pSubScriptionCaption, -1);									// 클랜가입요청
		LinkControlEx(plist, "clanSubscriptionPopupOk", m_pSubScriptionOk, GCI_CLAN_POPUP_SUBSCRIPTION_OK);				// 클랜 가입 확인
		LinkControlEx(plist, "clanSubscriptionPopupCancel", m_pSubScriptionCancel, GCI_CLAN_POPUP_SUBSCRIPTION_CANCEL);	// 클랜 가입 취소

		LinkControlEx(plist,  "clanSubscriptionPopupReceiverStatic",m_pSubScriptionReceiver, -1);			// To. 
		LinkControlEx(plist,  "clanSubscriptionPopupReceiverValue",m_pSubScriptionReceiverValue, -1);		// 대상 클랜명
		LinkControlEx(plist,  "clanSubscriptionPopupNickStatic",m_pSubScriptionSenderNick, -1);				// 닉네임
		LinkControlEx(plist,  "clanSubscriptionPopupRankImage", m_pSubScriptionSenderRank, -1);			// 발신자 등급
		LinkControlEx(plist,  "clanSubscriptionPopupNickValue",m_pSubScriptionSenderNickValue, -1);			// 발신자 닉네임 

		LinkControlEx(plist,  "clanSubscriptionPopupWinStatic",m_pSubScriptionSenderWin, -1);				// 승수 :
		LinkControlEx(plist,  "clanSubscriptionPopupWinValue",m_pSubScriptionSenderWinValue, -1);			// 발신자 승수
		LinkControlEx(plist,  "clanSubscriptionPopupScoreStatic",m_pSubScriptionSenderScore, -1);			// 전적	:
		LinkControlEx(plist,  "clanSubscriptionPopupScoreValue",m_pSubScriptionSenderScoreValue, -1);		// 발신자 전적

		LinkControlEx(plist,  "clanSubscriptionPopupMessageStatic",m_pSubScriptMessageStatic, -1);			// 가입 신청 메시지*( 100자이내)
		LinkControlEx(plist,  "clanSubscriptionPopupMemoEdit", m_pSubScriptMessageValue, -1);				// 가입 신청 메시지 입력창
	}

	return FALSE;
}	

void CPopupClanCreate::OnInitControl(void)
{
	m_pClanCreate->SetEnable( FALSE);
	{
		INIT_WIN_CAPTION( m_pClanCreateCaption, GAME_STRING("STBL_IDX_CLAN_CREATING"));			// 클랜 생성
		INIT_CAPTION_BUTTON( m_pClanCreateOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));			// 확인
		INIT_CAPTION_BUTTON( m_pClanCreateCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));	// 취소

		m_pClanCreateGuideValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_512, GAME_FONT_DEFAULT_SIZE, FALSE, NULL);
		m_pClanCreateGuideValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanCreateGuideValue->SetTextSpace( 0, 5);
		m_pClanCreateGuideValue->SetTextColor(GetColor(GCT_DEFAULT));
	}

	m_pClanInputInfo->SetEnable( FALSE);		// 클랜정보 입력
	{
		INIT_WIN_CAPTION( m_pClanInputInfoCaption, GAME_STRING("STBL_IDX_CLAN_CAPTION_INPUT_INFO"));	// 클랜 기본정보 입력
		INIT_CAPTION_BUTTON( m_pClanInputInfoOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));			// 확인
		INIT_CAPTION_BUTTON( m_pClanInputInfoCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));	// 취소
		INIT_STATIC_CAPTION( m_pClanInputInfoClanName, GAME_STRING("STBL_IDX_CLAN_HEADER_NAME"));	// 클랜명

		//클랜명 입력용 창입니다.
		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = NET_CLAN_NAME_SIZE - 1;
		ime._StyleIME = 0;
		m_pClanInputInfoClanNameEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pClanInputInfoClanNameEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanInputInfoClanNameEdit->SetTextColor(GetColor(GCT_VALUE));
		m_pClanInputInfoClanNameEdit->SetText("");
		m_pClanInputInfoClanNameEdit->SetIMEMode( IME_MODE_NATIVE);

		INIT_CAPTION_BUTTON( m_pClanInputInfoClanNameCheck, GAME_STRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));			// 중복검사

		INIT_STATIC_CAPTION( m_pClanInputInfoClanNameGuide, GAME_STRING("STBL_IDX_CLAN_INPUT_NAME_CONDITION"));		//	"(한글 8자, 영문 16자)"
		INIT_STATIC_CAPTION( m_pClanInputInfoClanIntroduce, GAME_STRING("STBL_IDX_CLAN_INPUT_GUIDE_CONDITION"));		//	"클랜소개 (한글 120자 이내)"

		m_pClanInputInfoClanAddressGuideStatic->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanInputInfoClanAddressGuideStatic->SetTextColor(GetColor(GCT_DEFAULT));

		//	"클랜 아지트 주소(URL)를 입력해 주십시오.\n20자 이내의 영문과 숫자를 사용할 수 있으며 공백 문자는 입력이 불가능 합니다.\n입력을 결정한 주소는 추후 변경이 불가능 합니다."
		m_pClanInputInfoClanAddressGuideStatic->SetText(GAME_STRING("STBL_IDX_CLAN_MESSAGE_INPUT_AZIT"));

		I3COLOR color;
		i3Color::Set(&color, (UINT8) 255, 125, 0, 255);
		m_pClanInputInfoClanAddressStatic->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanInputInfoClanAddressStatic->SetTextColor(&color);
		m_pClanInputInfoClanAddressStatic->SetText((char *)g_pConfigEx->GetClan().AzitAddressBase);

		//	클랜 아지트 주소를 입력용 창입니다.
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = NET_CLAN_AZIT_URL_SIZE - 1;
		ime._StyleIME = IME_STYLE_IGNORE_KEYINPUT_IME_MODE;
		m_pClanInputInfoClanAddressEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pClanInputInfoClanAddressEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanInputInfoClanAddressEdit->SetTextColor(GetColor(GCT_VALUE));
		m_pClanInputInfoClanAddressEdit->SetText("");
		m_pClanInputInfoClanAddressEdit->SetAlphaNumericOnly( true);

		INIT_CAPTION_BUTTON(m_pClanInputInfoClanAddressCheck, GAME_STRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));	//	"중복검사"

#ifdef CLAN_ADDRESS_ENABLE
		m_pClanInputInfoClanAddressGuideStatic->SetEnable( TRUE);
		m_pClanInputInfoClanAddressStatic->SetEnable( TRUE);
		m_pClanInputInfoClanAddressEdit->SetEnable( TRUE);
		m_pClanInputInfoClanAddressCheck->SetEnable( TRUE);
		m_pClanInputInfoClanAddressBox->SetEnable( TRUE);
#else
		m_pClanInputInfoClanAddressGuideStatic->SetEnable( FALSE);
		m_pClanInputInfoClanAddressStatic->SetEnable( FALSE);
		m_pClanInputInfoClanAddressEdit->SetEnable( FALSE);
		m_pClanInputInfoClanAddressCheck->SetEnable( FALSE);
		m_pClanInputInfoClanAddressBox->SetEnable( FALSE);
#endif

		//클랜 소개글 입력용 창입니다.
		CREATE_IME_ELEMENT ime2;
		ime2._hWnd = g_hWnd;
		ime2._hInst = g_hInstance;
		ime2._nTextLimitCount = 240;//NET_CLAN_INTRO_SIZE - 1;
		ime2._StyleIME = IME_STYLE_MULTILINE;
		m_pClanInputInfoClanIntroduceValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_512, GAME_FONT_DEFAULT_SIZE, TRUE, &ime2);
		m_pClanInputInfoClanIntroduceValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pClanInputInfoClanIntroduceValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_pClanInputInfoClanIntroduceValue->SetText(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ADD_HERE"));		//	"여기에 입력하세요"
		m_pClanInputInfoClanIntroduceValue->SetTextAutoWrap( TRUE);
		m_pClanInputInfoClanIntroduceValue->SetIMEMode( IME_MODE_NATIVE);
	}

	m_pOverlappingCheck->SetEnable( FALSE);
	{
		INIT_WIN_CAPTION( m_pOverlappingCheckCaption, GAME_STRING("STBL_IDX_CLAN_CAPTION_CHECKING_NAME"));	// "클랜명 중복 검사"
		INIT_CAPTION_BUTTON( m_pOverlappingCheckOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));					// 확인
		INIT_CAPTION_BUTTON( m_pOverlappingCheckCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));			// 취소
		INIT_CAPTION_BUTTON( m_pOverlappingCheckCheck, GAME_STRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));		// 중복검사

		INIT_VALUE_CAPTION( m_pOverlappingCheckInput, GAME_STRING("STBL_IDX_CLAN_CAPTION_SEARCHING_OTHER_CLAN"));		// "다른 이름 검색하기"

		// 중복으로 판명된 클랜명
		m_pOverlappingCheckInsertedName->SetTextColor( GetColor( GCT_VALUE));
		m_pOverlappingCheckInsertedName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingCheckInsertedName->SetTextColor(GetColor(GCT_VALUE));
		m_pOverlappingCheckInsertedName->SetText("");

		//
		m_pOverlappingCheckInsertedNameGuide->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE, FALSE, NULL);
		m_pOverlappingCheckInsertedNameGuide->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingCheckInsertedNameGuide->SetTextColor(GetColor(GCT_DEFAULT));
		m_pOverlappingCheckInsertedNameGuide->SetTextSpace( 0, 5);
		//	"은/는 이미 사용중이거나 사용할수 없는 이름입니다. \n 다른 이름을 선택해 주세요."
		m_pOverlappingCheckInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));

		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = NET_CLAN_NAME_SIZE - 1;
		ime._StyleIME = 0;
		m_pOverlappingCheckInputEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pOverlappingCheckInputEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingCheckInputEdit->SetTextColor(GetColor(GCT_VALUE));
		m_pOverlappingCheckInputEdit->SetText("");
		m_pOverlappingCheckInputEdit->SetIMEMode( IME_MODE_NATIVE);
	}

	m_pOverlappingAddress->SetEnable( FALSE);
	{
		INIT_WIN_CAPTION( m_pOverlappingAddressCaption, GAME_STRING("STBL_IDX_CLAN_CAPTION_CHECK_AZIT"));	// "아지트 주소 중복 검사"
		INIT_CAPTION_BUTTON( m_pOverlappingAddressOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));				// 확인
		INIT_CAPTION_BUTTON( m_pOverlappingAddressCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));		// 취소
		INIT_CAPTION_BUTTON( m_pOverlappingAddressCheck, GAME_STRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));	// 중복검사 버튼

		INIT_STATIC_CAPTION( m_pOverlappingAddressInputStatic, GAME_STRING("STBL_IDX_CLAN_SEARCHING_OTHER_AZIT"));	// "다른 아지트 주소 검색하기"

		// 중복으로 판명된 아지트 주소
		m_pOverlappingAddressInsertedName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingAddressInsertedName->SetTextColor(GetColor(GCT_VALUE));
		m_pOverlappingAddressInsertedName->SetText("");

		m_pOverlappingAddressInsertedNameGuide->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE, FALSE, NULL);
		m_pOverlappingAddressInsertedNameGuide->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingAddressInsertedNameGuide->SetTextColor(GetColor(GCT_DEFAULT));
		m_pOverlappingAddressInsertedNameGuide->SetTextSpace( 0, 5);
		//	"은/는 이미 사용중이거나 사용할수 없는 주소입니다.\n다른 주소를 선택해 주세요."
		m_pOverlappingAddressInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_AZIT"));
		
		I3COLOR color;
		i3Color::Set(&color, (UINT8) 255, 125, 0, 255);
		m_pOverlappingAddressURL->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingAddressURL->SetTextColor(&color);
		m_pOverlappingAddressURL->SetText((char *)g_pConfigEx->GetClan().AzitAddressBase);

		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = NET_CLAN_AZIT_URL_SIZE - 1;
		ime._StyleIME = IME_STYLE_IGNORE_KEYINPUT_IME_MODE;
		m_pOverlappingAddressInputEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pOverlappingAddressInputEdit->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pOverlappingAddressInputEdit->SetTextColor(GetColor(GCT_VALUE));
		m_pOverlappingAddressInputEdit->SetText("");
		m_pOverlappingAddressInputEdit->SetAlphaNumericOnly( true);
	}

	m_pCreateClanMarkGui->SetEnable(FALSE);
	{
		INIT_WIN_CAPTION( m_pCreateClanMarkCaption, GAME_STRING("STBL_IDX_CLAN_CREATE_CLAN_MARK"));		//	"클랜 마크 등록"

		INIT_WIN_CAPTION(m_pCreateClanMarkSelectStatic, "SELECT");

		INIT_CAPTION_BUTTON(m_pCreateClanMarkButtonOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));	//	"확인"
		INIT_CAPTION_BUTTON(m_pCreateClanMarkButtonCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));	//	"취소"
		INIT_CAPTION_BUTTON(m_pCreateClanMarkButtonOverlappingCheck, GAME_STRING("STBL_IDX_CLAN_OVERLAPPING_CHECK"));	//	"중복검사"

		INIT_STATIC_CAPTION(m_pCreateClanMarkBackColorStatic, GAME_STRING("STBL_IDX_CLAN_BACK_COLOR"));		//	"바탕 색상"
		INIT_STATIC_CAPTION(m_pCreateClanMarkShapeColorStatic, GAME_STRING("STBL_IDX_CLAN_SHAPE_COLOR"));	//	"모양 색상"

		for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
		{
			INIT_BUTTON_EX( m_pCreateClanMarkPrev[i]);
			INIT_BUTTON_EX( m_pCreateClanMarkNext[i]);
		}

		for(INT32 i=0; i<CREATE_CLANMARK_COLOR_SLOT_MAX; i++)
		{
			m_pCreateClanMarkBackColorBox[i]->setInputDisable(FALSE);
			m_pCreateClanMarkBackColorBox[i]->SetAllShapeEnable(FALSE);
			m_pCreateClanMarkBackColorBox[i]->SetShapeEnable(i, TRUE);

			m_pCreateClanMarkShapeColorBox[i]->setInputDisable(FALSE);
			m_pCreateClanMarkShapeColorBox[i]->SetAllShapeEnable(FALSE);
			m_pCreateClanMarkShapeColorBox[i]->SetShapeEnable(i, TRUE);
		}
	}

	m_pSubScription->SetEnable( FALSE);
	{
		char temp[256] = "";

		INIT_WIN_CAPTION( m_pSubScriptionCaption, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIBER_NOTE"));	// "클랜 가입 쪽지"
		INIT_CAPTION_BUTTON(m_pSubScriptionOk, GAME_STRING("STBL_IDX_CLAN_SEND"));						// "보내기"
		INIT_CAPTION_BUTTON(m_pSubScriptionCancel, GAME_STRING("STBL_IDX_CLAN_BUTTON_CANCEL"));			// "취소"

		INIT_STATIC_CAPTION( m_pSubScriptionReceiver, "To.");			// To."
		INIT_VALUE_CAPTION( m_pSubScriptionReceiverValue, "");			// 대상 클랜명

		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_CLAN_MEMBER_NAME"));	// "닉네임 :"
		INIT_STATIC_CAPTION( m_pSubScriptionSenderNick, temp);	
		m_pSubScriptionSenderRank->SetEnable( FALSE);					// 발신자 등급
		INIT_VALUE_CAPTION( m_pSubScriptionSenderNickValue, "");		// 발신자 닉네임 

		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_CLAN_WIN_SCORE"));	// "승수 :"
		INIT_STATIC_CAPTION( m_pSubScriptionSenderWin, temp);			
		INIT_VALUE_CAPTION( m_pSubScriptionSenderWinValue, "");			// 발신자 승수

		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_CLAN_USER_RECORD"));			// "전적 :"
		INIT_STATIC_CAPTION( m_pSubScriptionSenderScore, temp);	

		INIT_VALUE_CAPTION( m_pSubScriptionSenderScoreValue, "");		// 발신자 전적

		INIT_STATIC_CAPTION( m_pSubScriptMessageStatic, GAME_STRING("STBL_IDX_CLAN_SUBSCRIBER_NOTE_CONDITION"));		//	"가입 신청메시지(한글100자 이내)"

		CREATE_IME_ELEMENT ime;
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = 200;//NET_CLAN_REQUEST_SIZE - 1;
		ime._StyleIME = IME_STYLE_MULTILINE;
		m_pSubScriptMessageValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_512, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pSubScriptMessageValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pSubScriptMessageValue->SetTextColor(GetColor(GCT_VALUE));
		m_pSubScriptMessageValue->SetText(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ADD_HERE"));	//	"여기에 입력하세요"
		m_pSubScriptMessageValue->SetTextSpace( 0, 5);
		m_pSubScriptMessageValue->SetTextAutoWrap( TRUE);
		m_pSubScriptMessageValue->SetIMEMode( IME_MODE_NATIVE);
	}

	CPopupBase::OnInitControl();
}

BOOL CPopupClanCreate::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			if( InputKeyboardEnter())
			{
				if (m_pClanCreate->isEnable())
				{
					_CreateConfirm();
				}
				else if( m_pClanInputInfo->isEnable())	
				{
					if( !m_pClanInputInfoClanIntroduceValue->getIsFocus())
						_InputOK();
				}
				else if( m_pOverlappingCheck->isEnable())
				{
					_OverLappingOK();
				}
				else if( m_pOverlappingAddress->isEnable())
				{
					_OverLappingAddressOK();
				}
				else if( m_pSubScription->isEnable())
				{
					if( !m_pSubScriptMessageValue->getIsFocus())
						_SubscriptionOK();
				}
			}
		}
	}

	return CPopupBase::OnEvent(event, pObj, param2, code);
}

void CPopupClanCreate::OnGameEvent( INT32 event, INT32 arg)
{
	//	이벤트 결과를 받아 처리한다.
	switch( event )
	{
	case EVENT_CLAN_DUPLICATE_NAME:				//	클랜 이름 중복 체크
		{
			if( m_bWaitingIsDuplicateName)
			{
				_OnEventDuplicateName( arg);
			}
		}
		break;
	case EVENT_CLAN_DUPLICATE_ADDRESS:			//	클랜 주소 중복 체크
		{
			if( m_bWaitingIsDuplicateAddress)
			{
				_OnEventDuplicateAddress( arg);
			}
		}
		break;
	case EVENT_CLAN_DUPLICATE_MARK:				//	클랜 마크 중복 체크
		{
			if(m_bWaitingIsDuplicateMark)
			{
				if (EV_SUCCESSED(arg))
				{
					m_bCheckedClanMark = TRUE;

					if( !m_bCreateConfirm)
					{
						ACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);

						STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_POSSIBLE_MARK"));	//	"사용 가능한 클랜 마크입니다."
					}
				}
				else
				{
					m_bCheckedClanMark = FALSE;

					if( !m_bCreateConfirm)
					{
						DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);

						//	"사용할 수 없는 클랜 마크입니다. \n 다른 클랜 마크를 생성하시겠습니까?"
						//	"클랜 마크 등록"
						CAPTION_POPUP_Q(GAME_STRING("STBL_IDX_CLAN_MESSAGE_IMPOSSIBLE_MARK_Q"), 
										GAME_STRING("STBL_IDX_CLAN_CREATE_CLAN_MARK"), MAKE_CALLBACK(cbCreateClanMarkOverlapping), this);
					}
				}

				m_bWaitingIsDuplicateMark = FALSE;
			}
		}
		break;
	case EVENT_CLAN_JOIN_REQUEST:				//	클랜 가입 
		{
			_OnEventSubScriptionPage( arg);
		}
		break;
	case EVENT_CLAN_CREATE:						//	클랜 생성
		{
			_OnEventCreateClan( arg);
		}
		break;
	//case EVENT_CLAN_CANCEL_REQUEST:
	//	{
	//	}
		break;
	}
	
	if( m_bCreateConfirm && m_bCheckedName && m_bCheckedAddress && m_bCheckedClanMark)
	{
		//	클랜생성 이벤트를 보낸다.
		_SendCreateClan();
		return;
	}
	//	클랜 이름, 주소 중복 체크 끝난후 확인버튼을 누르면 다시 중복체크가 끝날때까지 기다렸다가 클랜 마크 생성 팝업으로 넘어가도록 한다.
	else if( m_pCreateClanMarkGui->isEnable() == FALSE && m_bJumpToClanMarkStep && 
			m_bCheckedName && m_bCheckedAddress)
	{
		//	클랜 마크 생성 페이지로
		_OnPopupCreateClanMark();
		return;
	}
}

void CPopupClanCreate::SetInputDisable(BOOL bDisable)
{
	m_pClanCreateRoot->setInputDisable(bDisable);
}

BOOL CPopupClanCreate::GetInputDisable(void)
{
	return m_pClanCreateRoot->getInputDisable();
}

void CPopupClanCreate::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);

	//	클랜 소개란을 클릭하는 순간 문자 정렬을 가운데에서 왼쪽으로 변경한다. komet
	if(m_pClanInputInfo->isEnable())
	{
		if(i3String::Length(m_pClanInputInfoClanIntroduceValue->GetText()) == 0)
		{
			m_bCheckedClanIntro = TRUE;
			m_pClanInputInfoClanIntroduceValue->SetTextColor(GetColor(GCT_VALUE));
			m_pClanInputInfoClanIntroduceValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		}
	}

	//	가입 소개란을 클릭하는 순간 문자 정렬을 가운데에서 왼쪽으로 변경한다. komet
	if(m_pSubScription->isEnable())
	{
		if(i3String::Length(m_pSubScriptMessageValue->GetText()) == 0)
		{
			m_pSubScriptMessageValue->SetTextColor(GetColor(GCT_VALUE));
			m_pSubScriptMessageValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		}
	}
}

void CPopupClanCreate::OnPreGuiNotify(void)
{
	CPopupBase::OnPreGuiNotify();
}

void CPopupClanCreate::OnWheelUpdate(INT32 scrollby)
{
	CPopupBase::OnWheelUpdate( scrollby);
}	

void CPopupClanCreate::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);

	m_pClanCreateRoot->SetEnable(TRUE);

	// _OnPopupClanCreate()안에서 STRING_POPUP()호출 후 return 시,
	// 배경 GUI의 입력을 다시 풀어야하기때문에 먼저 막습니다.
	((CStageReady*)m_pParent)->SetMenuBarInputDisable( TRUE);

	if( pArg1)
	{
		m_nCurClanIndex = *(INT32*)pArg1;
		_ResetSubscriptionPage();
		_OnPopupSubScriptionPage();
	}
	else
	{
		_ResetClanCreatePage();
		_OnPopupClanCreate();
	}

	//	"클랜을 생성하기 위해선 {col:253,167,0,255}%s 이상{/col}의 계급이 필요하며,\n {col:253,167,0,255}%d{/col} 포인트가 소모됩니다.\n \n\n 클랜을 생성하시겠습니까? \n"
	UINT8	RequirementRank = 0;
	UINT32	RequirementPoint = 0;
	if ( ! g_pFramework->GetClanContext()->GetClanCreationRequirementInfo(&RequirementRank, &RequirementPoint) )
	{
		// 에러 발생한것입니다.
		m_pClanCreateOk->SetEnable(FALSE);
	}

	string str = BOOST_FORMAT2(GAME_STRING("STBL_IDX_CLAN_MESSAGE_FOR_CREATING"), GetCharaRank(RequirementRank), RequirementPoint);

	m_pClanCreateGuideValue->SetText(str.c_str());

	if( m_pStringFilter == NULL)
	{
		m_pStringFilter = CStringFilter::NewObject();
		m_pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	}

	if( m_pStringFilter2 == NULL)
	{
		m_pStringFilter2 = CStringFilter::NewObject();
		m_pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	}
}

void CPopupClanCreate::_ResetSubscriptionPage()
{
	m_pSubScriptMessageValue->SetIMEText( "");
	m_pSubScriptMessageValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSubScriptMessageValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSubScriptMessageValue->SetText(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ADD_HERE"));		//	"여기에 입력하세요"
}

void CPopupClanCreate::_ResetClanCreatePage()
{
	m_pClanInputInfoClanIntroduceValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pClanInputInfoClanIntroduceValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanInputInfoClanIntroduceValue->SetIMEText("");
	m_pClanInputInfoClanIntroduceValue->SetText(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ADD_HERE"));		//	"여기에 입력하세요"

	m_pClanInputInfoClanAddressEdit->SetIMEText("");
	m_pClanInputInfoClanAddressEdit->SetIMEMode(IME_MODE_ALPHANUMERIC);

	m_pClanInputInfoClanNameEdit->SetIMEText("");
	m_pClanInputInfoClanNameEdit->SetIMEMode(IME_MODE_NATIVE);
	m_pClanInputInfoClanNameEdit->SetFocus(TRUE);
	i3GuiRoot::setFocusControl( m_pClanInputInfoClanNameEdit);

	m_bCheckedName = FALSE;
	m_bCheckedAddress = FALSE;
	m_bCheckedClanMark = FALSE;
	m_bCheckedClanIntro = FALSE;

#ifdef CLAN_ADDRESS_ENABLE
	m_pClanInputInfoClanAddressGuideStatic->SetEnable( TRUE);
	m_pClanInputInfoClanAddressStatic->SetEnable( TRUE);
	m_pClanInputInfoClanAddressEdit->SetEnable( TRUE);
	m_pClanInputInfoClanAddressCheck->SetEnable( TRUE);
	m_pClanInputInfoClanAddressBox->SetEnable( TRUE);
#else
	m_pClanInputInfoClanAddressGuideStatic->SetEnable( FALSE);
	m_pClanInputInfoClanAddressStatic->SetEnable( FALSE);
	m_pClanInputInfoClanAddressEdit->SetEnable( FALSE);
	m_pClanInputInfoClanAddressCheck->SetEnable( FALSE);
	m_pClanInputInfoClanAddressBox->SetEnable( FALSE);

	m_bCheckedAddress = TRUE;
#endif
}

void CPopupClanCreate::OnClosePopup(void * pArg1, void * pArg2)
{
	m_pClanCreate->SetEnable( FALSE);
	m_pClanInputInfo->SetEnable( FALSE);
	m_pOverlappingCheck->SetEnable( FALSE);
	m_pOverlappingAddress->SetEnable(FALSE);
	m_pCreateClanMarkGui->SetEnable(FALSE);
	m_pSubScription->SetEnable( FALSE);

	((CStageReady*)m_pParent)->SetMenuBarInputDisable( FALSE);

	I3_SAFE_RELEASE(m_pStringFilter);
	I3_SAFE_RELEASE(m_pStringFilter2);

	_CleanupTexture();

	m_pClanCreateRoot->SetEnable(FALSE);

	CPopupBase::OnClosePopup( pArg1, pArg2);
}

void CPopupClanCreate::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupClanCreate::_CleanupTexture(void)
{
	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
	{
		for(INT32 j=0; j<CREATE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			I3_SAFE_RELEASE( m_pTexClanMarkSelect[i][j]);
		}
	}

	I3_SAFE_RELEASE(m_pTexClanMarkFinal);
}

void CPopupClanCreate::_CreateConfirm(void)
{
	UINT32 MyPoint = g_pGameContext->GetMyPoint();

	UINT32 wRank = g_pGameContext->GetRealRank();

	UINT8	RequirementRank = 0;
	UINT32	RequirementPoint = 0;
	if ( ! g_pFramework->GetClanContext()->GetClanCreationRequirementInfo(&RequirementRank, &RequirementPoint) )
	{
		return;
	}

	if( MyPoint > RequirementPoint && wRank >= RequirementRank)
	{
		_OnPopupInputInfo();
		return;
	}

	if( MyPoint < RequirementPoint)
	{ 
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_NO_POINT"));		//	"포인트가 부족합니다."
	}
	else if( wRank < RequirementRank)
	{
		string str = BOOST_FORMAT2("{col:253,167,0,255}%s{/col} %s", GetCharaRank( RequirementRank), GAME_STRING("STBL_IDX_CLAN_NEED_HIGH_RANK"));
		STRING_POPUP(str.c_str());
	}
	else
	{
		//	"죄송합니다. 클랜 생성에 필요한 계급에\n도달하지 못하였거나 포인트가 부족합니다.\n다음에 다시 시도해 주세요."
#if defined(LOCALE_CHINA)
		char szColMsg[512] = "";
		char str[512] = "";
		sprintf( szColMsg, "{col:253,167,0,255}%s{/col}", GetCharaRank( RequirementRank));
		sprintf( str, GAME_STRING("STBL_IDX_CLAN_NEED_HIGH_RANK"), szColMsg);
		STRING_POPUP(str);
#else
		string str = BOOST_FORMAT2("{col:253,167,0,255}%s{/col} %s", GetCharaRank( RequirementRank), GAME_STRING("STBL_IDX_CLAN_NEED_HIGH_RANK"));
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANNOT_CREATING_CLAN"));
#endif
	}

	OnClosePopup();

}

void CPopupClanCreate::_CreateCancel(void)
{
	OnClosePopup();
}

void CPopupClanCreate::_InputOK(void)
{
	char pNick[MAX_STRING_COUNT] = {0,};
	i3String::Copy( pNick, m_pClanInputInfoClanNameEdit->getIMETextBuf(), MAX_STRING_COUNT );

	const char * pAddress = m_pClanInputInfoClanAddressEdit->GetText();

	m_bCheckingName = FALSE;
	m_bCheckingAddress = FALSE;

	if( m_bWaitingIsDuplicateName || m_bWaitingIsDuplicateAddress)
		return;
	
	//	check step 1
	if( i3String::Length(pNick) <= 0)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_NAME"));		//	"클랜명을 적어주세요"
		return;
	}

	//	check step 2
	if( !m_bCheckedName)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_NAME"));		//	"클랜명 중복검사를 해주십시오."
		return;
	}

#ifdef CLAN_ADDRESS_ENABLE
	//	check step 3
	if( i3String::Length(pAddress) <= 0)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));	// "클랜 아지트 주소를 적어주세요."
		return;
	}

	//	check step 4
	if( !m_bCheckedAddress)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_AZIT"));	//	"클랜 아지트 주소 중복검사를 해주십시오."
		return;
	}
	
	//	check step 5
	{
		CStringFilter* pStringFilter = CStringFilter::NewObject();
		pStringFilter->CreateFilter(STR_FILTER_INCLUDE_CHAT_NICK);

		if( pStringFilter->IsFilterSame( pAddress ) == FALSE ) {
			//	"클랜 아지트 주소에 입력 할 수 없는 단어가 포함되어 있습니다"
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_AZIT"));
			m_pClanInputInfoClanAddressEdit->SetIMEText("");
			I3_SAFE_RELEASE( pStringFilter );
			return;
		}

		I3_SAFE_RELEASE( pStringFilter );
	}
#endif

	//	check step 6
	{
		char pszString[CPopupClanCreate::MAX_STRING] = {0,};
		i3String::Copy( pszString, m_pClanInputInfoClanIntroduceValue->getIMETextBuf(), CPopupClanCreate::MAX_STRING );
		
		if( m_bCheckedClanIntro == FALSE || i3String::Length( pszString ) == 0 ) 
		{
			//	"클랜 소개가 올바르지 않습니다.\n다시 확인해 주십시오."
			STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_WRONG_CLAN_GUIDE") );
			return;
		}
//		INT32 idxBlank = i3String::ContainChar(pszString, ' ');

		if( FALSE == m_pStringFilter2->IsFilter( pszString ) ||
			FALSE == m_pStringFilter->IsFilterSame( pszString ) || 
			FALSE == g_pFramework->IsFilter( pszString ) ) 
		{
			//	"클랜 소개글에 입력 할 수 없는 단어가 포함되어 있습니다"
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_CLAN_GUIDE"));

			m_pClanInputInfoClanIntroduceValue->SetIMEText("");

			return;
		}
	}

	//	check step 6
	if( _CheckNickNameOverLap( pNick))
	{
		if( _CheckClanAddressValidate( pAddress))
		{
			if( _CheckClanAddressOverLap( pAddress))
			{
				//STRING_POPUP("이대로 진행하시겠습니까?", cbQueryCreateClan);

				StepToCreateClan();
			}
		}
		else
		{
			m_pClanInputInfoClanAddressEdit->SetText("");
			m_pClanInputInfoClanAddressEdit->SetIMEText("");
		}
	}
}

BOOL CPopupClanCreate::_CheckNickNameOverLap(const char * pszText)
{
	if( m_bWaitingIsDuplicateName )
		return FALSE;

	if( i3String::Length(pszText) <= 0)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_NAME"));	//	"클랜명을 적어주세요"
		return FALSE;
	}

	{
		bool GoodNickName = true;

		INT32 idxBlank = i3String::ContainChar(pszText, ' ');

		if( -1 != idxBlank )
		{
			_LOG_("NickName Contained Blank");
			GoodNickName = false;
		}
		//else if( FALSE == m_pStringFilter2->IsFilter( pszText ) )
		else if( FALSE == m_pStringFilter2->IsFilterNick( pszText ) )
		{
			_LOG_("NickName Detected IsFilter");
			GoodNickName = false;
		}
		else if( FALSE == m_pStringFilter->IsFilterSame( pszText ) )
		{
			_LOG_("NickName Detected IsFilterSame");
			GoodNickName = false;
		}
		//else if( FALSE == g_pFramework->IsFilter( pszText ) )
		else if( FALSE == g_pFramework->IsFilterNick( pszText ) )
		{
			_LOG_("NickName Detected IsFilter");
			GoodNickName = false;
		}
		else if( FALSE == g_pFramework->CheckValidCharacterByLocale( pszText ) )
		{
			_LOG_("NickName Detected CheckValidCharacterByLocale");
			GoodNickName = false;
		}

		if( !GoodNickName )
		{
			//	입력 할 수 없는 단어가 포함되어 있습니다"
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));

			m_pClanInputInfoClanNameEdit->SetIMEText("");
			m_pOverlappingCheckInputEdit->SetIMEText("");

			return FALSE;
		}
	}
	m_bWaitingIsDuplicateName = TRUE;

	//	클랜 주소 중복 체크에 영향 받지 않도록
	if(!m_bWaitingIsDuplicateAddress)
 		m_bCheckedName = FALSE; 

	return g_pFramework->GetClanContext()->SendOverlappingClanName(pszText);
}

BOOL CPopupClanCreate::_CheckClanAddressOverLap(const char * pszText)
{
#ifdef CLAN_ADDRESS_ENABLE
 	if( m_bWaitingIsDuplicateAddress) 
		return FALSE;

	if( i3String::Length(pszText) <= 0)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));	//	"클랜 아지트 주소를 적어주세요."

		return FALSE;
	}
	
	{
		INT32 idxBlank = i3String::ContainChar(pszText, ' ');

		if( -1 != idxBlank || 
			FALSE == m_pStringFilter2->IsFilter( pszText ) || 
			FALSE == m_pStringFilter->IsFilterSame( pszText ) || 
			FALSE == g_pFramework->IsFilter( pszText ) ) 
		{
			//	"입력 할 수 없는 단어가 포함되어 있습니다."
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));

			m_pClanInputInfoClanAddressEdit->SetIMEText("");
			m_pOverlappingAddressInputEdit->SetIMEText("");

			return FALSE;
		}

		//	check step 5
		{
			CStringFilter* pStringFilter = CStringFilter::NewObject();
			pStringFilter->CreateFilter(STR_FILTER_INCLUDE_CHAT_NICK);

			if( pStringFilter->IsFilterSame( pszText ) == FALSE ) {
				//	"입력 할 수 없는 단어가 포함되어 있습니다."
				STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
				m_pClanInputInfoClanAddressEdit->SetIMEText("");
				m_pClanInputInfoClanAddressEdit->SetIMEText("");
				I3_SAFE_RELEASE( pStringFilter );
				return FALSE;
			}

			I3_SAFE_RELEASE( pStringFilter );
		}
	}

	m_bWaitingIsDuplicateAddress = TRUE;

	//	클랜명 중복 체크에 영향 받지 않도록
	if(!m_bWaitingIsDuplicateName)
		m_bCheckedAddress = FALSE;
	
	return g_pFramework->GetClanContext()->SendOverlappingClanAddress(pszText);

#else
	m_bCheckedAddress = TRUE;

	return TRUE;

#endif
}

BOOL CPopupClanCreate::_CheckClanAddressValidate(const char * pszText)
{
#ifdef CLAN_ADDRESS_ENABLE
	const char * ch = pszText;

	if( *ch == 0)
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));	//	"클랜 아지트 주소를 적어주세요."

		m_bCheckedAddress = FALSE;

		return FALSE;
	}

	while(*ch)
	{
		//	클랜 주소는 영문 소문자와 숫자로만 입력 가능하므로 일일이 체크한다. (0 ~ 9 && a ~ z)
		if( (*ch >= '0' && *ch <= '9') || (*ch >= 'a' && *ch <= 'z')) 
		{ 
			ch++;

			continue;
		}
		else
		{
			//	"클랜 아지트 URL은\n영문 소문자와 숫자만 사용 가능합니다."
			//	"아지트 주소 오류"
			CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_ONLY_ENGLISH_OR_NUMERIC"), 
						GAME_STRING("STBL_IDX_CLAN_CAPTION_ERROR_AZIT"));
			
			m_bCheckedAddress = FALSE;

			return FALSE;
		}
	}	

	return TRUE;
#else
	
	m_bCheckedAddress = TRUE;

	return TRUE;
#endif
}

void CPopupClanCreate::_InputCancel(void)
{
	OnClosePopup();
}

void CPopupClanCreate::_InputCheckNick(void)
{
	m_bCheckingName = TRUE;

	char pszBuffer[MAX_STRING_COUNT] = {0,};
	i3String::Copy( pszBuffer, m_pClanInputInfoClanNameEdit->getIMETextBuf(), MAX_STRING_COUNT );
	
	_CheckNickNameOverLap( pszBuffer);
}

void CPopupClanCreate::_InputCheckClanAddress(void)
{
	const char * pszAddress = m_pClanInputInfoClanAddressEdit->GetText();

	m_bCheckingAddress = TRUE;

	if( _CheckClanAddressValidate( pszAddress))
	{
		_CheckClanAddressOverLap( pszAddress);
	}
	else
	{
		m_pClanInputInfoClanAddressEdit->SetText("");
		m_pClanInputInfoClanAddressEdit->SetIMEText("");
	}
}

void CPopupClanCreate::_SendCreateClan(void)
{
	const char * pszClanName = m_pClanInputInfoClanNameEdit->getIMETextBuf();
	const char * pszIntro = m_pClanInputInfoClanIntroduceValue->getIMETextBuf();
	const char * pszURL = m_pClanInputInfoClanAddressEdit->getIMETextBuf();

	//	클랜명 없이는 클랜 생성이 불가능한다.
	if( i3String::Length(pszClanName) == 0 )
	{
		m_bCreateConfirm = FALSE;
		m_bCheckedName = FALSE;

		return;
	}

#ifdef CLAN_ADDRESS_ENABLE
	//	클랜주소 없이는 클랜 생성이 불가능한다.
	if( i3String::Length(pszURL) == 0)
	{
		m_bCreateConfirm = FALSE;
		m_bCheckedAddress = FALSE;

		return;
	}
#endif

	if( m_bCreateConfirm && !m_bWaitingConfirmCreateClan &&	m_bCheckedName && m_bCheckedAddress && m_bCheckedClanMark)
	{		
		UINT32 nMarkFlag = 0;

#if defined(USE_CLAN_MARK) && defined(ENABLE_CREATE_CLAN_MARK)
		//	1 base (0 is default)
		I3_BOUNDCHK(m_nClanMarkBackImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
		I3_BOUNDCHK(m_nClanMarkShapeImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
		I3_BOUNDCHK(m_nClanMarkBackColorIdx, CClanMarkManager::GetMarkColorCount()+1);
		I3_BOUNDCHK(m_nClanMarkShapeColorIdx, CClanMarkManager::GetMarkColorCount()+1);

		nMarkFlag = MAKE_CLANMARK_FLAG( m_nClanMarkBackImageIdx, m_nClanMarkShapeImageIdx, m_nClanMarkBackColorIdx, m_nClanMarkShapeColorIdx);
#endif

		//	클랜 생성 이벤트를 보낸다.	
		g_pFramework->GetClanContext()->SendCreateClan(pszClanName, pszIntro, pszURL, nMarkFlag);
		
		m_bWaitingConfirmCreateClan = TRUE;
		m_bCreateConfirm = FALSE;
	}
	else if( m_bWaitingConfirmCreateClan)
	{
		//	"\n 잠시만 기다려주십시오.\n 이미 요청하였습니다."
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_WAIT"));
	}
}

void CPopupClanCreate::_OverLappingOK(void)
{
	char pszName[MAX_STRING_COUNT] = {0,};
	i3String::Copy( pszName, m_pOverlappingCheckInputEdit->getIMETextBuf(), MAX_STRING_COUNT );

	if( !m_bCheckedName)
	{
		//	"클랜명 중복검사를 해주십시오."
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_NAME"));
		return;
	}

	//if confirmed
	m_bCheckingName = FALSE;

	_CheckNickNameOverLap( pszName);
}

void CPopupClanCreate::_OverLappingCancel(void)
{
	_OnPopupInputInfo();
}

void CPopupClanCreate::_OverLappingCheck(void)
{
	m_bCheckingName = TRUE; 

	char pszName[MAX_STRING_COUNT] = {0,};
	i3String::Copy( pszName, m_pOverlappingCheckInputEdit->getIMETextBuf(), MAX_STRING_COUNT );

	_CheckNickNameOverLap( pszName);
}

void CPopupClanCreate::_OverLappingAddressOK(void)
{
	const char * pszAddress = m_pOverlappingAddressInputEdit->GetText();

	if( i3String::Length(pszAddress) <= 0)
	{
		//	"클랜 아지트 주소를 적어주세요"
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));
		return;
	}

	if( !m_bCheckedAddress)
	{
		//	"클랜 아지트 주소 중복검사를 해주십시오."
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_AZIT"));
		return;
	}

	//if confirmed
	m_bCheckingAddress = FALSE;

	if( _CheckClanAddressValidate( pszAddress))
	{
		_CheckClanAddressOverLap( pszAddress); 
	}
	else
	{
		m_pOverlappingAddressInputEdit->SetText("");
		m_pOverlappingAddressInputEdit->SetIMEText("");
	}
}

void CPopupClanCreate::_OverLappingAddressCancel(void)
{
	_OnPopupInputInfo();
}

void CPopupClanCreate::_OverLappingAddressCheck(void)
{
	const char * pszAddress = m_pOverlappingAddressInputEdit->GetText();

	m_bCheckingAddress = TRUE;

	if( _CheckClanAddressValidate( pszAddress))
	{
		_CheckClanAddressOverLap( pszAddress); 
	}
	else
	{
		m_pOverlappingAddressInputEdit->SetText("");
		m_pOverlappingAddressInputEdit->SetIMEText("");
	}
}

void CPopupClanCreate::_SubscriptionOK(void)
{
	if( m_bWaitingConfirmSubscripting )
		return;

	{
		const char * pszText = m_pSubScriptMessageValue->GetText();

		if( FALSE == m_pStringFilter2->IsFilter( pszText ) || 
			FALSE == m_pStringFilter->IsFilterSame( pszText ) || 
			FALSE == g_pFramework->IsFilter( pszText ) ) 
		{
			//	"입력 할 수 없는 단어가 포함되어 있습니다."
			STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));

			m_pSubScriptMessageValue->SetIMEText( "");

			return;
		}
	}


	//	"가입을 신청하시겠습니까?"
	//	"클랜 가입 신청"
	CAPTION_POPUP_Q(GAME_STRING("STBL_IDX_CLAN_MESSAGE_REQUEST_SUBSCRIPTION_Q"), 
					GAME_STRING("STBL_IDX_CLAN_CAPTION_REQUEST_SUBSCRIPTION"), MAKE_CALLBACK(cbQuaryJoinClan), this);
}

void CPopupClanCreate::_SubscriptionSendJoin(void)
{
	g_pFramework->GetClanContext()->SendJoinToClanMember( g_pGameContext->getClanDetailInfo()->_idx,
														  m_pSubScriptMessageValue->getIMETextBuf() );

	m_bWaitingConfirmSubscripting = TRUE;
}

void CPopupClanCreate::_SubScriptionCancel(void)
{
	OnClosePopup();
}

void CPopupClanCreate::_SubscriptionCancelRequest(void)
{
	g_pFramework->GetClanContext()->SendCancelJoinFromClanMember();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupClanCreate::_OnPopupClanCreate(void)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
	if(( MyUserInfoBasic._clanidx != 0) && (MyUserInfoBasic._clanstate == 0))
	{
		//	"이미 클랜가입을 요청한 상태입니다.\n가입 신청을 취소하시겠습니까?"
		//	"가입 신청 취소"
		CAPTION_POPUP_Q( GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANCEL_SUBSCRIPTION_Q"), 
						GAME_STRING("STBL_IDX_CLAN_CAPTION_CANCEL_SUBSCRIPTION"), MAKE_CALLBACK(cbCancelRequest), this);

		((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);
		OnClosePopup();
		return;
	}

    m_pClanCreate->SetEnable( TRUE);
	m_pClanInputInfo->SetEnable( FALSE);
	m_pOverlappingCheck->SetEnable( FALSE);
	m_pOverlappingAddress->SetEnable( FALSE);
	m_pCreateClanMarkGui->SetEnable(FALSE);

	m_bCreateConfirm = FALSE;
	m_bWaitingConfirmCreateClan = FALSE;
	m_bCheckingName = FALSE;
	m_bCheckingAddress = FALSE;
	m_bCheckedName = FALSE;
	m_bCheckedAddress = FALSE;
	m_bCheckedClanMark = FALSE;
	m_bJumpToClanMarkStep = FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupClanCreate::_OnPopupInputInfo(void)
{
	m_pClanCreate->SetEnable( FALSE);
	m_pClanInputInfo->SetEnable( TRUE);
	m_pOverlappingCheck->SetEnable( FALSE);
	m_pOverlappingAddress->SetEnable(FALSE);
	m_pCreateClanMarkGui->SetEnable(FALSE);

	m_bCreateConfirm = FALSE;

	m_pClanInputInfoClanNameEdit->SetFocus(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupClanCreate::_OnPopupCheckOverlapping( const char * pszName, BOOL bDuplicate)
{
	m_pClanCreate->SetEnable( FALSE);
	m_pClanInputInfo->SetEnable( FALSE);
	m_pOverlappingCheck->SetEnable( TRUE);
	m_pOverlappingAddress->SetEnable(FALSE);
	m_pCreateClanMarkGui->SetEnable(FALSE);

	m_pOverlappingCheckInsertedName->SetText( pszName);

	if( bDuplicate)
	{
		//	"은/는 이미 사용중이거나 사용할수 없는 이름입니다. \n 다른 이름을 사용해 주십시오."
		m_pOverlappingCheckInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));

		m_pOverlappingCheckInputEdit->SetText("");
		m_pOverlappingCheckInputEdit->SetIMEText("");
	}
	else
	{
		m_pOverlappingCheckInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_CAN_USE"));	//	"은/는 사용하실 수 있습니다."
	}

	m_bCreateConfirm = FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupClanCreate::_OnPopupCheckOverlappingAddress( const char * pszName, BOOL bDuplicate)
{
	m_pClanCreate->SetEnable( FALSE);
	m_pClanInputInfo->SetEnable( FALSE);
	m_pOverlappingCheck->SetEnable( FALSE);
	m_pOverlappingAddress->SetEnable(TRUE);
	m_pCreateClanMarkGui->SetEnable(FALSE);

	m_pOverlappingAddressInsertedName->SetText( pszName);

	if( bDuplicate)
	{
		//	"은/는 이미 사용중이거나 사용할수 없는 주소입니다.\n 다른 주소를 선택해 주세요."
		m_pOverlappingAddressInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_AZIT"));

		m_pOverlappingAddressInputEdit->SetText("");
		m_pOverlappingAddressInputEdit->SetIMEText("");
	}
	else
	{
		//	"은/는 사용하실 수 있습니다."
		m_pOverlappingAddressInsertedNameGuide->SetText(GAME_STRING("STBL_IDX_CLAN_CAN_USE"));
	}

	m_bCreateConfirm = FALSE;
}

void CPopupClanCreate::_OnPopupCreateClanMark(void)
{
	m_pClanCreate->SetEnable( FALSE);
	m_pClanInputInfo->SetEnable( FALSE);
	m_pOverlappingCheck->SetEnable( FALSE);
	m_pOverlappingAddress->SetEnable(FALSE);
	m_pCreateClanMarkGui->SetEnable(TRUE);

	//	1 base
	m_nClanMarkBackImageIdx = 1;
	m_nClanMarkShapeImageIdx = 1;
	m_nClanMarkBackColorIdx = 1;
	m_nClanMarkShapeColorIdx = 1;

	m_bCheckedClanMark = FALSE;
	m_bCreateConfirm = FALSE;
	m_bWaitingIsDuplicateMark = FALSE;	

	_CleanupTexture();

	//	선택용 클랜 마크 조합 이미지를 미리 생성
	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
	{
		for(INT32 j=0; j<CREATE_CLANMARK_SHAPE_SLOT_MAX; j++)
		{
			//	텍스처 생성
			m_pTexClanMarkSelect[i][j] = i3Texture::NewObject(); 
			I3ASSERT( m_pTexClanMarkSelect[i][j] != NULL);

			m_pTexClanMarkSelect[i][j]->Create(64, 64, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);
		}
	}

	m_pTexClanMarkFinal = i3Texture::NewObject(); 
	I3ASSERT( m_pTexClanMarkFinal != NULL);
	m_pTexClanMarkFinal->Create(64, 64, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);
	ACTIVATE_BUTTON(m_pCreateClanMarkButtonOverlappingCheck);

	_CreateClanMarkChangeBackColor(m_nClanMarkBackColorIdx);
	_CreateClanMarkChangeShapeColor(m_nClanMarkShapeColorIdx);
	
	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
	{
		ACTIVATE_BUTTON(m_pCreateClanMarkPrev[i]);
		ACTIVATE_BUTTON(m_pCreateClanMarkNext[i]);

		_CreateClanMarkUserSelectImageUpdate(i);
	}

	for(INT32 i=0; i<CREATE_CLANMARK_COLOR_SLOT_MAX; i++)
	{
		m_pCreateClanMarkBackColorBox[i]->setInputDisable(FALSE);
		m_pCreateClanMarkShapeColorBox[i]->setInputDisable(FALSE);
	}
}

void CPopupClanCreate::_CreateClanMarkOK(void)
{	
	if( !m_bWaitingIsDuplicateMark)
	{
		m_bCreateConfirm = TRUE;
		m_bCheckedClanMark = FALSE;

		//	무조건 클랜 마크를 체크해야 클랜을 생성할 수 있습니다
		_CreateClanMarkOverlappingCheck();
	}
}

void CPopupClanCreate::_CreateClanMarkCancel(void)
{	
	OnClosePopup();
}

void CPopupClanCreate::cbCreateClanMarkOverlapping(void * pThis, INT32 nParam)
{
	if( nParam == MBR_CANCEL)
	{
		CPopupClanCreate * pPopup = (CPopupClanCreate *) pThis;

		pPopup->CreateClanMarkOverlappingConfirm();
	}
}

void CPopupClanCreate::CreateClanMarkOverlappingConfirm(void)
{
	//	"클랜 마크 등록을 취소하시면 \n 다음 클랜 마크 변경시 아이템의 구입이 필요합니다. \n 그래도 취소하겠습니까?"
	//	"클랜 마크 등록"
	CAPTION_POPUP_Q(GAME_STRING("STBL_IDX_CLAN_CANCEL_CREATE_CLAN_MARK_Q"), 
					GAME_STRING("STBL_IDX_CLAN_CREATE_CLAN_MARK"), MAKE_CALLBACK(cbCreateClanMarkNoSign), this);
}

void CPopupClanCreate::cbCreateClanMarkNoSign(void * pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		CPopupClanCreate * pPopup = (CPopupClanCreate *) pThis;

		if( pPopup->IsExactTypeOf( CPopupClanCreate::GetClassMeta()))
		{
			pPopup->CreateClanMarkNoSignConfirm();
		}
	}
}

void CPopupClanCreate::CreateClanMarkNoSignConfirm(void)
{
	m_bCreateConfirm = TRUE;
	m_bCheckedClanMark = TRUE;
	
	//	마크 조합을 포기하고 기본 클랜 마크로 클랜 생성을 합니다.
	m_nClanMarkBackImageIdx = 0;
	m_nClanMarkShapeImageIdx = 0;
	m_nClanMarkBackColorIdx = 0;
	m_nClanMarkShapeColorIdx = 0;

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOverlappingCheck);

	for(INT32 i=0; i<CREATE_CLANMARK_COMBINATION_MAX; i++)
	{
		DEACTIVATE_BUTTON(m_pCreateClanMarkPrev[i]);
		DEACTIVATE_BUTTON(m_pCreateClanMarkNext[i]);
	}
	
	for(INT32 i=0; i<CREATE_CLANMARK_COLOR_SLOT_MAX; i++)
	{
		m_pCreateClanMarkBackColorBox[i]->setInputDisable(TRUE);
		m_pCreateClanMarkShapeColorBox[i]->setInputDisable(TRUE);
	}

	//	클랜생성 이벤트를 보낸다.
	_SendCreateClan();
}

void CPopupClanCreate::_CreateClanMarkOverlappingCheck(void)
{
	if(m_bWaitingIsDuplicateMark == FALSE)
	{
		//	1 base (0 is default)
		I3_BOUNDCHK(m_nClanMarkBackImageIdx, CClanMarkManager::GetMarkBackImageCount()+1);
		I3_BOUNDCHK(m_nClanMarkShapeImageIdx, CClanMarkManager::GetMarkShapeImageCount()+1);
		I3_BOUNDCHK(m_nClanMarkBackColorIdx, CClanMarkManager::GetMarkColorCount()+1);
		I3_BOUNDCHK(m_nClanMarkShapeColorIdx, CClanMarkManager::GetMarkColorCount()+1);

		UINT32 nMarkFlag = MAKE_CLANMARK_FLAG( m_nClanMarkBackImageIdx, m_nClanMarkShapeImageIdx, m_nClanMarkBackColorIdx, m_nClanMarkShapeColorIdx);

		g_pFramework->GetClanContext()->SendOverlappingClanMark(nMarkFlag);

		m_bWaitingIsDuplicateMark = TRUE;
	}
}

void CPopupClanCreate::_CreateClanMarkPrevBackImage(void)
{
	m_nClanMarkBackImageIdx--;

	if( m_nClanMarkBackImageIdx <= 0)							m_nClanMarkBackImageIdx = CClanMarkManager::GetMarkBackImageCount();

	//	이전 마크 배경 이미지로 업데이트
	_CreateClanMarkUserSelectImageUpdate(0);

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);
}

void CPopupClanCreate::_CreateClanMarkNextBackImage(void)
{
	m_nClanMarkBackImageIdx++;

	if( m_nClanMarkBackImageIdx > CClanMarkManager::GetMarkBackImageCount())		m_nClanMarkBackImageIdx = 1;

	//	다음 마크 배경 이미지로 업데이트
	_CreateClanMarkUserSelectImageUpdate(0);

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);
}

void CPopupClanCreate::_CreateClanMarkPrevShapeImage(void)
{
	m_nClanMarkShapeImageIdx--;

	if( m_nClanMarkShapeImageIdx <= 0)							m_nClanMarkShapeImageIdx = CClanMarkManager::GetMarkShapeImageCount();

	//	이전 마크 모양 이미지로 업데이트
	_CreateClanMarkUserSelectImageUpdate(1);

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);
}

void CPopupClanCreate::_CreateClanMarkNextShapeImage(void)
{
	m_nClanMarkShapeImageIdx++;

	if( m_nClanMarkShapeImageIdx > CClanMarkManager::GetMarkShapeImageCount())	m_nClanMarkShapeImageIdx = 1;

	//	다음 마크 모양 이미지로 업데이트
	_CreateClanMarkUserSelectImageUpdate(1);

	DEACTIVATE_BUTTON(m_pCreateClanMarkButtonOk);
}

void CPopupClanCreate::_CreateClanMarkUserSelectImageUpdate(INT32 nShapeIdx)
{
	INT32 nImageIdx = 1;
	INT32 nColorIdx = 1;
	INT32 nImageMax = 1;

	//	관련된 클랜 마크 조각 슬롯 이미지 업데이트
	for(INT32 slot=0; slot<CREATE_CLANMARK_SHAPE_SLOT_MAX; slot++)
	{		
		if( nShapeIdx == 0)			//	배경 마크
		{
			nImageIdx = m_nClanMarkBackImageIdx;
			nColorIdx = m_nClanMarkBackColorIdx;
			nImageMax = CClanMarkManager::GetMarkBackImageCount();

			I3_BOUNDCHK( nImageIdx,CClanMarkManager::GetMarkBackImageCount()+1);
			I3_BOUNDCHK( nColorIdx,CClanMarkManager::GetMarkColorCount()+1);
		}
		else if( nShapeIdx == 1)	//	모양 마크	
		{
			nImageIdx = m_nClanMarkShapeImageIdx;
			nColorIdx = m_nClanMarkShapeColorIdx;
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

		if( nShapeIdx == 0)			//	배경 마크
		{
			I3_BOUNDCHK( nImageIdx,CClanMarkManager::GetMarkBackImageCount()+1);
			I3_BOUNDCHK( nColorIdx,CClanMarkManager::GetMarkColorCount()+1);
		}
		else if( nShapeIdx == 1)	//	모양 마크	
		{
			I3_BOUNDCHK( nImageIdx,CClanMarkManager::GetMarkShapeImageCount()+1);
			I3_BOUNDCHK( nColorIdx,CClanMarkManager::GetMarkColorCount()+1);
		}

		//	하나의 슬롯의 마크 조각 이미지를 변경합니다.
		_CreateClanMarkUserSelectImage(nShapeIdx, slot, nImageIdx, nColorIdx); 
	}

	//	클랜 마크 조각 이미지가 변경됬으므로 최종 조합 마크 이미지도 함께 변경
	_CreateClanMarkUserFinalImage();
}

void CPopupClanCreate::_CreateClanMarkUserSelectImage(INT32 nShapeIdx, INT32 nSlotIdx, INT32 nImageIdx, INT32 nColorIdx)
{	//	하나의 슬롯의 마크 조각 이미지를 변경합니다.
	I3_BOUNDCHK( nShapeIdx, CREATE_CLANMARK_COMBINATION_MAX);
	I3_BOUNDCHK( nSlotIdx, CREATE_CLANMARK_SHAPE_SLOT_MAX);

	i3Texture * pTex = m_pTexClanMarkSelect[nShapeIdx][nSlotIdx];
	I3ASSERT(pTex);

	if( nShapeIdx == 0)			//	배경 마크
	{
		::MakeClanMarkShapeTex(pTex, CLAN_MARK_MEDIUM, CLAN_MARK_LAYER_FRAME, nImageIdx, nColorIdx);
	}
	else if( nShapeIdx == 1)	//	모양 마크
	{
		::MakeClanMarkShapeTex(pTex, CLAN_MARK_MEDIUM, CLAN_MARK_LAYER_CONTENT, nImageIdx, nColorIdx);
	}

	i3GuiControl * pControl = m_pCreateClanMarkSelectImage[nShapeIdx][nSlotIdx];
	i3GuiImage * pImage = pControl->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);
	I3ASSERT( pImage != NULL);

	pImage->SetTexture( pTex, 0, 0, CLAN_MARK_MID_PIXEL, CLAN_MARK_MID_PIXEL);
	pControl->SetShapeEnable( GUI_SHAPE_OFFSET, TRUE);
	pControl->UpdateRenderObjects();	//	Sprite를 강제 업데이트 하기위해...이거 없으면 GUI에서 텍스처 정보를 업데이트 안합니다. komet
	
}

void CPopupClanCreate::_CreateClanMarkUserFinalImage(void)
{	//	모든 조합이 완료된 최종 마크 이미지 업데이트
	i3Texture * pTex = m_pTexClanMarkFinal;
	I3ASSERT(pTex);

	::MakeClanMarkTex(pTex, CLAN_MARK_MEDIUM, m_nClanMarkBackImageIdx, m_nClanMarkShapeImageIdx, m_nClanMarkBackColorIdx,m_nClanMarkShapeColorIdx);

	i3GuiControl * pControl = m_pCreateClanMarkConfirmImage;
	i3GuiImage * pImage = pControl->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);
	I3ASSERT( pImage != NULL);

	pImage->SetTexture( pTex, 0, 0, CLAN_MARK_MID_PIXEL, CLAN_MARK_MID_PIXEL);
	pControl->SetShapeEnable( GUI_SHAPE_OFFSET, TRUE);
	pControl->UpdateRenderObjects();	//	Sprite를 강제 업데이트 하기위해...이거 없으면 GUI에서 텍스처 정보를 업데이트 안합니다. komet
	
}

void CPopupClanCreate::_CreateClanMarkChangeBackColor(INT32 nColorIdx)
{	//	배경 마크 색상 변경	(1 base)
	m_nClanMarkBackColorIdx = nColorIdx;

	I3ASSERT( m_nClanMarkBackColorIdx > 0);
	I3_BOUNDCHK( m_nClanMarkBackColorIdx,CClanMarkManager::GetMarkColorCount()+1);

	INT32 idxArray = nColorIdx-1;

	INT32 selectedWidth = m_pCreateClanMarkBackColorSelected->getWidth();
	INT32 slotWidth = m_pCreateClanMarkBackColorBox[idxArray]->getWidth();
	INT32 offsetWidth = (selectedWidth - slotWidth) / 3;
	REAL32 slotPos = m_pCreateClanMarkBackColorBox[idxArray]->getPositionX();

	m_pCreateClanMarkBackColorSelected->setPositionX( slotPos - offsetWidth); 

	PlaySound(GUI_SND_ITEM_SELECTING);

	_CreateClanMarkUserSelectImageUpdate(0);
}

void CPopupClanCreate::_CreateClanMarkChangeShapeColor(INT32 nColorIdx)
{	//	모양 마크 색상 변경 (1 base)
	m_nClanMarkShapeColorIdx = nColorIdx;

	I3ASSERT( m_nClanMarkShapeColorIdx > 0);
	I3_BOUNDCHK( m_nClanMarkShapeColorIdx,CClanMarkManager::GetMarkColorCount()+1);

	INT32 idxArray = nColorIdx-1;
	
	INT32 selectedWidth = m_pCreateClanMarkShapeColorSelected->getWidth();
	INT32 slotWidth = m_pCreateClanMarkShapeColorBox[idxArray]->getWidth();
	INT32 offsetWidth = (selectedWidth - slotWidth) / 3;
	REAL32 slotPos = m_pCreateClanMarkShapeColorBox[idxArray]->getPositionX();

	m_pCreateClanMarkShapeColorSelected->setPositionX( slotPos - offsetWidth); 

	PlaySound(GUI_SND_ITEM_SELECTING);

	_CreateClanMarkUserSelectImageUpdate(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPopupClanCreate::_OnPopupSubScriptionPage(void)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
	if(( MyUserInfoBasic._clanidx != 0) && (MyUserInfoBasic._clanstate == 0))
	{
		//	"이미 클랜가입을 요청한 상태입니다.\n가입 신청을 취소하시겠습니까?"
		//	"가입 신청 취소"
		CAPTION_POPUP_Q( GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANCEL_SUBSCRIPTION_Q"), 
						GAME_STRING("STBL_IDX_CLAN_CAPTION_CANCEL_SUBSCRIPTION"), MAKE_CALLBACK(cbCancelRequest), this);
		OnClosePopup();
		return;
	}

	char strTotalRecord[MAX_STRING_COUNT];
	char strTotalKillDeath[MAX_STRING_COUNT];

	const USER_INFO_RECORD * pRecordInfo	= g_pGameContext->GetMyUserInfoRecord();
	CLAN_DETAIL_INFO * pDetailedClanInfo	= g_pGameContext->getClanDetailInfo();

	//	"%d전 %d승 %d패"
	#ifdef USE_MODE_RECORD
	sprintf_s(strTotalRecord, GAME_STRING("STBL_IDX_CLAN_DECLARE_SCORE"), pRecordInfo->_season_record._match, pRecordInfo->_season_record._win, pRecordInfo->_season_record._lose);
	sprintf_s(strTotalKillDeath, "%d / %d", pRecordInfo->_season_record._killcount, pRecordInfo->_season_record._death);
	#else
	sprintf_s(strTotalRecord, GAME_STRING("STBL_IDX_CLAN_DECLARE_SCORE"), pRecordInfo->_smatch, pRecordInfo->_swin, pRecordInfo->_slose);
	sprintf_s(strTotalKillDeath, "%d / %d", pRecordInfo->_skillcount, pRecordInfo->_sdeath);
	#endif
	
	m_pSubScriptionSenderWinValue->SetText(strTotalRecord);
	m_pSubScriptionSenderScoreValue->SetText(strTotalKillDeath);

	{
		USER_INFO_BASIC		MyUserInfoBasic;
		g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

		GameGUI::SetRankStatic(m_pSubScriptionSenderRank);
		m_pSubScriptionSenderRank->SetShapeUVIndex(MyUserInfoBasic._rank);
		m_pSubScriptionSenderRank->SetEnable(TRUE);

		SetNameUsingColorTable( m_pSubScriptionReceiverValue, pDetailedClanInfo->_name, pDetailedClanInfo->_color );

		// 발신자 닉네임 
		SetNameUsingColorTable(m_pSubScriptionSenderNickValue, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);
	}

	m_pSubScription->SetEnable( TRUE);
}

void CPopupClanCreate::_OnEventSubScriptionPage( INT32 Arg)
{
	if( EV_SUCCESSED( Arg ) )
	{
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_COMPLETE_SUBSCRIPTION"));	//	"가입신청이 완료되었습니다."
		OnClosePopup();
	}
	else
	{
		switch( Arg ) {
			case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
				{
					//	"\n가입 신청 실패하였습니다.\n클랜을 찾을수 없습니다."
					//	클랜 가입 승인 실패
					CAPTION_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_NO_FINDING"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_MAXREQUEST:
				{
					//	"\n클랜 가입 처리가 실패했습니다.\n더이상 가입 할 수 없습니다."
					//	클랜 가입 승인 실패
					CAPTION_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_OVER_MEMBER"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;			
			case EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX:
				{
					//	"\n가입 신청 실패하였습니다.\n이미 다른 클랜에 가입되어 있거나 가입 요청한 상태입니다."
					//	클랜 가입 승인 실패	
					CAPTION_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_ALREADY_SUBSCRIPTION"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_REQUEST_FAIL:
				{
					//	"\n가입 신청 실패하였습니다"
					//	클랜 가입 승인 실패
					CAPTION_POPUP(GAME_STRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_FAIL"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL:
				{
					//	해당 클랜에 가입할 수 있는 연령과 맞지 않아 클랜 가입 신청이 취소 되었습니다."
					//	클랜 가입 승인 실패
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANNOT_SUBSCRIPTION_AGE"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_JOIN_AUTH_RANK_FAIL:
				{
					//	"해당 클랜에 가입할 수 있는 계급과 맞지 않아 클랜 가입 신청이 취소 되었습니다."
					//	클랜 가입 승인 실패
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANNOT_SUBSCRIPTION_RANK"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			default:
				{
					char szTemp[MAX_PATH];
					//	"\n가입 신청에 실패하였습니다.\n 죄송합니다. 관리자에게 문의하여 주십시오."
					sprintf_s( szTemp, "%s\n [Errcode : 0x%08x]", 
										GAME_STRING("STBL_IDX_CLAN_MESSAGE_CANNOT_SUBSCRIPTION"), Arg );

					//	클랜 가입 승인 실패
					CAPTION_POPUP( szTemp, GAME_STRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
		}
	}

	m_bWaitingConfirmSubscripting = FALSE;
}

void CPopupClanCreate::_OnEventCreateClan( INT32 Arg)
{
	if( m_bWaitingConfirmCreateClan)
	{
		if( EV_SUCCESSED( Arg ) )
		{
			//	"\n축하합니다.\n클랜 개설이 완료되었습니다."
			//	"클랜 생성 성공"
			CAPTION_POPUP_C( GAME_STRING("STBL_IDX_CLAN_MESSAGE_COMPLETE_CREATING"), 
							 GAME_STRING("STBL_IDX_CLAN_CAPTION_COMPLETE_CREATING"), MAKE_CALLBACK(cbSuccessCreateClan), this);

#if defined(USE_CLAN_MARK) && defined(ENABLE_CREATE_CLAN_MARK)
			//	배경 모델의 클랜 마크도 변경
			((CStageReady*)m_pParent)->GetReadyBg()->CharaChangeClanMarkInReadyBg();
#endif
		}
		else
		{
			switch( Arg )
			{
			case EVENT_ERROR_CLAN_MAXCLAN:
				{
					//	"\n 클랜 생성에 실패하였습니다.\n 더이상 클랜을 만들수 없습니다."
					//	"클랜 생성 실패"
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_OVERFLOW"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			case EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX:
				{
					//	"\n 클랜 생성에 실패하였습니다.\n 이미 다른 클랜에 가입되어 있거나 가입 요청한 상태입니다."
					//	"클랜 생성 실패"
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_ALREADY"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			case EVENT_ERROR_CLAN_SEARCH_CLANIDX:
				{
					//	"\n 클랜 생성에 실패하였습니다.\n 이미 중복되는 클랜명 또는 아지트 주소가 존재합니다."
					//	"클랜 생성 실패"
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_OVERLAPPING"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			case EVENT_ERROR_CLAN_CREATE_FAIL:
				{
					//	"\n 클랜 생성에 실패하였습니다."
					//	"클랜 생성 실패"
					CAPTION_POPUP( GAME_STRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING"), 
									GAME_STRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			default:
				{
					char szTemp[MAX_PATH];
					//	"\n 클랜 생성에 실패하였습니다.\n 죄송합니다. 관리자에게 문의하여 주십시오."
					sprintf_s( szTemp, "%s\n [Errcode : 0x%08x]", GAME_STRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_ADMIN"), Arg);

					//	"클랜 생성 실패"
					CAPTION_POPUP( szTemp, GAME_STRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			}
		}

		OnClosePopup();

		m_bWaitingConfirmCreateClan = FALSE;
	}
}

void CPopupClanCreate::_OnEventDuplicateName( INT32 Arg)
{
	char szTemp[MAX_PATH] = ""; 
	
	if( m_pClanInputInfo->isEnable()) 
	{
		const char * pszName = m_pClanInputInfoClanNameEdit->GetText();

		if( Arg == 0)		//	클랜명이 중복되지 않는다
		{	
			m_bCheckedName = TRUE;

			if( m_bCheckingName)
			{
				//	"은/는 사용하실 수 있습니다."
				sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s", pszName, GAME_STRING("STBL_IDX_CLAN_CAN_USE"));
				STRING_POPUP( szTemp);
			}
		}
		else		//	클랜명이 중복된다.
		{
			m_bCheckedName = FALSE;

			_OnPopupCheckOverlapping(pszName, TRUE);
		}		
	}
	else if( m_pOverlappingCheck->isEnable())
	{
		const char * pszName = m_pOverlappingCheckInputEdit->GetText();

		if( Arg == 0)
		{
			m_bCheckedName = TRUE;

			if( m_bCheckingName)
			{
				_OnPopupCheckOverlapping(pszName, FALSE);
			}
			else
			{
				m_pClanInputInfoClanNameEdit->SetIMEText( m_pOverlappingCheckInputEdit->getIMETextBuf());

				_OnPopupInputInfo();
			}
		}
		else
		{
			m_bCheckedName = FALSE;

			//	"은/는 이미 사용중이거나 사용할수 없는 이름입니다.\n 다른 이름을 선택해 주세요."
			sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s" , 
							m_pOverlappingCheckInputEdit->GetText(), GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));

			STRING_POPUP( szTemp);

			_OnPopupCheckOverlapping(m_pOverlappingCheckInputEdit->GetText(), TRUE);
		}
	}

	m_bWaitingIsDuplicateName = FALSE;
}

void CPopupClanCreate::_OnEventDuplicateAddress( INT32 Arg)
{
	char szTemp[MAX_PATH]; 
	
	if( m_pClanInputInfo->isEnable()) 
	{
		const char * pszAddress = m_pClanInputInfoClanAddressEdit->GetText(); 

		if( Arg == 0)
		{
			m_bCheckedAddress = TRUE;

			if( m_bCheckingAddress)
			{
				//	"은/는 사용하실 수 있습니다."
				sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s" , 
								pszAddress, GAME_STRING("STBL_IDX_CLAN_CAN_USE"));

				STRING_POPUP( szTemp);
			}			
		}
		else
		{			
			m_bCheckedAddress = FALSE;

			_OnPopupCheckOverlappingAddress(pszAddress, TRUE);
		}
	}
	else if( m_pOverlappingAddress->isEnable())
	{
		const char * pszAddress = m_pOverlappingAddressInputEdit->GetText();

		if( Arg == 0)
		{
			m_bCheckedAddress = TRUE;

			if( m_bCheckingAddress)
			{
				_OnPopupCheckOverlappingAddress(pszAddress, FALSE); 
			}
			else
			{
				m_pClanInputInfoClanAddressEdit->SetIMEText( pszAddress);
				
				_OnPopupInputInfo();
			}
		}
		else
		{
			m_bCheckedAddress = FALSE;

			//	"은/는 이미 사용중이거나 사용할수 없는 주소입니다.\n 다른 주소를 선택해 주세요."
			sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s", 
							pszAddress, GAME_STRING("STBL_IDX_CLAN_ALREADY_USED_AZIT"));

			STRING_POPUP( szTemp);

			_OnPopupCheckOverlappingAddress(pszAddress, TRUE);
		}
	}

	m_bWaitingIsDuplicateAddress = FALSE;
}

void CPopupClanCreate::StepToCreateClan(void)
{
#if defined(USE_CLAN_MARK) && defined(ENABLE_CREATE_CLAN_MARK)
	//	클랜 마크 생성 페이지로
	m_bJumpToClanMarkStep = TRUE;
#else
	//	바로 클랜 생성
	m_bCheckedClanMark = TRUE;
	m_bCreateConfirm = TRUE;

#if !defined( CLAN_ADDRESS_ENABLE)
	m_bCheckedAddress = TRUE;
#endif

#endif
}

void CPopupClanCreate::cbSuccessCreateClan(void* pThis, INT32 nParam)
{
	CPopupClanCreate * pThisPopup = (CPopupClanCreate*) pThis;
	I3ASSERT(pThisPopup != NULL);

	((CStageReady*)pThisPopup->getParent())->TogglePopup(POPUP_NULL);
	((CStageReady*)pThisPopup->getParent())->ChangeStep(STEP_CLAN_MEMBER);	
}

void CPopupClanCreate::cbQueryCreateClan(void* pThis, INT32 nParam)
{
	if( nParam == MB_OK)
	{
		((CPopupClanCreate*) pThis)->StepToCreateClan();
	}
}

void CPopupClanCreate::cbQuaryJoinClan(void* pThis, INT32 nParam)
{

	CPopupClanCreate * pPopup = (CPopupClanCreate*) pThis;

	if( nParam == MB_OK)
	{
		pPopup->_SubscriptionSendJoin();
	}
}

void CPopupClanCreate::cbCancelRequest(void* pThis, INT32 nParam)
{
	CPopupClanCreate * pPopup = (CPopupClanCreate*) pThis;

	if( nParam == MB_OK)
	{
		pPopup->_SubscriptionCancelRequest();
	}
	else
	{
		pPopup->OnClosePopup();
	}
}