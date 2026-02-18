#include "pch.h"
#if defined( ENABLE_NEW_CHALLENGE)

#include "StepProfile_CompensateWin.h"
#include "GuiNotifyReceiver.h"
#include "QuestTextDictionary.h"

I3_CLASS_INSTANCE( CStepProfile_CompensateWin, i3ElementBase);

CStepProfile_CompensateWin::CStepProfile_CompensateWin()
{
	m_pNotifyBase	= NULL;

	m_pPopupWinBG	= NULL;
	m_pCaption		= NULL;
	m_pButton_Confirm = NULL;
	m_pNoticeCaption = NULL;

	m_pDescription_CardName_Key		= NULL;
	m_pDescription_CardName_Value	= NULL;
	m_pDescription_CardInfo_Key		= NULL;
	m_pDescription_CardInfo_Value	= NULL;
	m_pDescription_CardNotice		= NULL;
	m_pDescription_CardNotice1		= NULL;


	i3mem::FillZero(m_CardSetList, sizeof(m_CardSetList));
}

CStepProfile_CompensateWin::~CStepProfile_CompensateWin()
{
}


BOOL CStepProfile_CompensateWin::LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl)
{
	INT32 i;
	char szText[ MAX_STRING_COUNT];

	m_pNotifyBase = pBase;

	LinkControlEx(plist,  "prCardCompensateWin",			m_pPopupWinBG,			-1);
	LinkControlEx(plist, 	"prCardCompensateCaption",		m_pCaption,				-1);
	LinkControlEx(plist, 	"prCardCompensateConfirm",		m_pButton_Confirm,		GCI_PRS_B_PROFILE_COMPENSATE_CONFIRM);
	INIT_COMBO_EX( m_pButton_Confirm);

	LinkControlEx(plist, 	"prCardCompensateNotice",		m_pNoticeCaption,		-1);

	for( i = 0; i < PROFILE_COMPENSATE_CARD_COUNT; i++)
	{
		sprintf_s( szText, "prCardCompensateBG%d", i);
		LinkControlEx(plist, 	szText,		m_CardSetList[ i]._CardButton,			GCI_PRS_B_PROFILE_COMPENSATE_CARD1 + i);
		{
			m_CardSetList[ i]._CardButton->SetDownClickAction(TRUE);
			//m_CardSetList[ i]._CardButton->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
			//m_CardSetList[ i]._CardButton->SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
			//m_CardSetList[ i]._CardButton->SetUserDefine( I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_OFFSET + 2);
			m_CardSetList[ i]._CardButton->setInputMode(I3GUI_INPUT_MODE_PRESS);
		}
		
		sprintf_s( szText, "prCardCompenstateImage%d", i);
		LinkControlEx(plist, 	szText,		m_CardSetList[ i]._CardImage,			-1);

		sprintf_s( szText, "prCardCompensateText%d", i);
		LinkControlEx(plist, 	szText,		m_CardSetList[ i]._CardName,			-1);
	}

	LinkControlEx(plist, 	"prCardCompensate_CardName_Key",			m_pDescription_CardName_Key,		-1);
	LinkControlEx(plist, 	"prCardCompensate_CardName_Value",			m_pDescription_CardName_Value,		-1);
	LinkControlEx(plist, 	"prCardCompensate_CardDescription_Key",		m_pDescription_CardInfo_Key,		-1);
	LinkControlEx(plist, 	"prCardCompensate_CardDescription_Value",	m_pDescription_CardInfo_Value,		-1);
	LinkControlEx(plist, 	"prCardCompensate_CardNotice",				m_pDescription_CardNotice,			-1);
	LinkControlEx(plist, 	"prCardCompensate_CardNotice1",				m_pDescription_CardNotice1,			-1);


	return TRUE;
}

void CStepProfile_CompensateWin::Init( void)
{
	m_pPopupWinBG->SetEnable( FALSE);
	
	#if defined(ENABLE_BOLD_CAPTION)
		m_pCaption->ReCreateTextEx(GetDefaultFontName(), FONT_COUNT_DEFAULT, GAME_FONT_DEFAULT_CAPTION_SIZE, FALSE, NULL, FW_BOLD);
	#endif
	m_pCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCaption->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCaption->SetText(GAME_STRING("STR_TBL_PROFILE_NOTICE_TEXT"));

	m_pButton_Confirm->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
	m_pButton_Confirm->SetCaptionEnable( TRUE);
	m_pButton_Confirm->SetCaptionAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pButton_Confirm->SetCaptionColor( GetColor(GCT_VALUE));
	m_pButton_Confirm->SetCaption( GAME_STRING("STR_TBL_PROFILE_CONFIRM"));		/*확인*/

	m_pNoticeCaption->SetTextColor( GetColor(GCT_DEFAULT));
	m_pNoticeCaption->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoticeCaption->SetText( GAME_STRING("STR_TBL_PROFILE_COMPENSATE_NOTICE_TEXT"));		/*원하시는 임무카드를 선택하여 주십시오*/

	m_pDescription_CardName_Key->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDescription_CardName_Key->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDescription_CardName_Key->SetText( GAME_STRING("STR_TBL_PROFILE_COMPENSATE_CARDNAME_TEXT"));	/*임무카드 명칭:*/

	m_pDescription_CardInfo_Key->SetTextColor( GetColor(GCT_DEFAULT));
	m_pDescription_CardInfo_Key->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDescription_CardInfo_Key->SetText( GAME_STRING("STR_TBL_PROFILE_COMPENSATE_CARDINFO_TEXT"));	/*임무카드 정보:*/

	m_pDescription_CardNotice->SetTextColor( GetColor( GCT_DEFAULT));
	m_pDescription_CardNotice->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDescription_CardNotice->SetText( GAME_STRING("STR_TBL_PROFILE_COMPENSATE_CARDNOTICE1_TEXT")); /*임무카드를 수행하시면 표장을 획득할 수 있으며 표장으로 개인정보 내 호칭 트리에서 호칭과 교환할 수 있습니다.*/

	m_pDescription_CardNotice1->SetTextColor( GetColor( GCT_DEFAULT));
	m_pDescription_CardNotice1->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDescription_CardNotice1->SetText( GAME_STRING("STR_TBL_PROFILE_COMPENSATE_CARDNOTICE2_TEXT")); /*(임무카드를 지속적으로 수행하여 획득한 표장으로 모든 호칭을 획득할 수 있습니다.)*/

	COLOR color;
	i3Color::Set( &color, 150, 100, 100, 255);

	m_pDescription_CardName_Value->SetTextColor( &color);
	m_pDescription_CardName_Value->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDescription_CardName_Value->SetText( "");

	m_pDescription_CardInfo_Value->SetTextColor( &color);
	m_pDescription_CardInfo_Value->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDescription_CardInfo_Value->SetText( "");

	for( INT32 i = 0; i < PROFILE_COMPENSATE_CARD_COUNT; i++)
	{
		m_CardSetList[ i]._CardName->SetTextColor( GetColor( GCT_DEFAULT));
		m_CardSetList[ i]._CardName->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_CardSetList[ i]._CardName->SetText( "");
	}

	__UpdateCardList();
	__UpdateCardInfo( __CardSlot_0);
}

void CStepProfile_CompensateWin::Update( REAL32 tm)
{
}

void CStepProfile_CompensateWin::GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch( pNotify->m_nID)
	{
	case GCI_PRS_B_PROFILE_COMPENSATE_CONFIRM :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)	ConfirmOK();
		break;
	case GCI_PRS_B_PROFILE_COMPENSATE_CARD1 :
	case GCI_PRS_B_PROFILE_COMPENSATE_CARD2 :
	case GCI_PRS_B_PROFILE_COMPENSATE_CARD3 :
	case GCI_PRS_B_PROFILE_COMPENSATE_CARD4 :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)	SelectCard( pNotify->m_nID - GCI_PRS_B_PROFILE_COMPENSATE_CARD1);
		break;
	}
}

void CStepProfile_CompensateWin::ConfirmOK( void)
{
	m_pPopupWinBG->SetEnable( FALSE);
}

void CStepProfile_CompensateWin::SelectCard( INT32 iSlot)
{
	__UpdateCardInfo( (__CardSlot) iSlot);
}

void CStepProfile_CompensateWin::__UpdateCardInfo( __CardSlot iSlot)
{
	INT32 i;
	char szCardName[ MAX_STRING_COUNT];
	char * pszCardInfo = NULL;
	UINT8 cardSetId = 0;

	for( i = 0; i < PROFILE_COMPENSATE_CARD_COUNT; i++)
	{
		m_CardSetList[ i]._CardButton->SetShapeEnable( 0, TRUE);
	}

	switch( iSlot)
	{
	case __CardSlot_0 :
		{
			cardSetId = 5;
			pszCardInfo = GAME_STRING("STR_TBL_PROFILE_COMPENSATE_CARDINFO_SLOT0_TEXT");	/*돌격소총 / 근접무기를 주로 사용하여 해결할 수 있는 임무*/
		}
		break;
	case __CardSlot_1 :
		{
			cardSetId = 6;
			pszCardInfo = GAME_STRING("STR_TBL_PROFILE_COMPENSATE_CARDINFO_SLOT1_TEXT");	/*저격소총 / 보조무기를 주로 사용하여 해결할 수 있는 임무*/
		}
		break;
	case __CardSlot_2 :
		{
			cardSetId = 7;
			pszCardInfo = GAME_STRING("STR_TBL_PROFILE_COMPENSATE_CARDINFO_SLOT2_TEXT");	/*기관단총 / 투척무기를 주로 사용하여 해결할 수 있는 임무*/
		}
		break;
	case __CardSlot_3 :
		{
			cardSetId = 8;
			pszCardInfo = GAME_STRING("STR_TBL_PROFILE_COMPENSATE_CARDINFO_SLOT3_TEXT");	/*산탄총 / 투척무기를 주로 사용하여 해결할 수 있는 임무*/
		}
		break;
	}

	m_CardSetList[ iSlot]._CardButton->SetShapeEnable( 0, FALSE);

	if( pszCardInfo != NULL)
		m_pDescription_CardInfo_Value->SetText( pszCardInfo);

	g_pQuestTextDictionary->GetCardSetNameString( szCardName, cardSetId);
	m_pDescription_CardName_Value->SetText( szCardName);
}

void CStepProfile_CompensateWin::__UpdateCardList( void)
{
	INT32 i;
	char szCardName[ MAX_STRING_COUNT];

	for( i = 0; i < PROFILE_COMPENSATE_CARD_COUNT; i++)
	{
		g_pFramework->SetCardSetShape( m_CardSetList[ i]._CardImage, i + 5);

		g_pQuestTextDictionary->GetCardSetNameString( szCardName, (UINT8)(i + 5) );
		m_CardSetList[ i]._CardName->SetText( szCardName);
	}
}

#endif
