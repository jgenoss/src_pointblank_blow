#include "pch.h"
#include "StepProfile_CardCompleteWin.h"
#include "GuiNotifyReceiver.h"

I3_CLASS_INSTANCE( CStepProfile_CardCompleteWin, i3ElementBase);

CStepProfile_CardCompleteWin::CStepProfile_CardCompleteWin()
{
	m_pNotifyBase = NULL;

	m_pCardComplateWin = NULL;
	m_pCardComplateCaption = NULL;
	m_pCardComplateConfirm = NULL;
	m_pCardComplateName = NULL;
	m_pCardComplateDescription = NULL;
	m_pCardComplateBg = NULL;
	m_pCardComplateDummy = NULL;
}

CStepProfile_CardCompleteWin::~CStepProfile_CardCompleteWin()
{
}

BOOL CStepProfile_CardCompleteWin::LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl)
{
	m_pNotifyBase = pBase;

	LinkControlEx(plist, 		"prCardComplateWin",			m_pCardComplateWin,				-1);
	LinkControlEx(plist, 		"prCardComplateCaption",		m_pCardComplateCaption,			-1);
	LinkControlEx(plist, 		"prCardComplateConfirm",		m_pCardComplateConfirm,			GCI_PRS_B_COMPLATE_CONFIRM);
	LinkControlEx(plist, 		"prCardComplateName",			m_pCardComplateName,			-1);
	LinkControlEx(plist, 		"prCardComplateDescription",	m_pCardComplateDescription,		-1);
	LinkControlEx(plist, 		"prCardComplateBg",				m_pCardComplateBg,				-1);
	LinkControlEx(plist, 		"prCardComplateDummy",			m_pCardComplateDummy,			-1);

	return TRUE;
}

void CStepProfile_CardCompleteWin::Init( void)
{
	m_pCardComplateWin->SetEnable( FALSE);

	{
		INIT_WIN_CAPTION2(m_pCardComplateCaption, GAME_STRING("STR_TBL_PROFILE_NOTICE_TEXT"), GetColor(GCT_DEFAULT));/*알림*/
	}

	// 버튼 초기화
	{
		INIT_BUTTON_EX( m_pCardComplateConfirm);
		{
			m_pCardComplateConfirm->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
			m_pCardComplateConfirm->SetCaptionColor( GetColor(GCT_VALUE));
			m_pCardComplateConfirm->SetCaption( GAME_STRING("STR_TBL_PROFILE_CONFIRM"));/*확인*/
		}
	}

	// 에디트박스 초기화
	m_pCardComplateName->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardComplateName->SetTextColor( GetColor(GCT_DEFAULT));

	m_pCardComplateDescription->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pCardComplateDescription->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardComplateDescription->SetTextSpace(0, 3);
	m_pCardComplateDescription->SetText( GAME_STRING("STR_TBL_PROFILE_REWARD_ITEM_MESSAGE"));/*{col:255,152,52,255}축하 드립니다!!{/col}\n \n인벤토리를 확인하시면 보상으로 아이템을 받으실 수 있습니다.(모으신 카드와 교환되며, 이미 보유중인 아이템일 경우에는 남은 기간이 증가합니다.)*/
}

void CStepProfile_CardCompleteWin::Update( REAL32 tm)
{
}

void CStepProfile_CardCompleteWin::GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify)
{	
}
