#include "pch.h"
#include "PopupExtension.h"
#include "GlobalVariables.h"
#include "StageReady.h"
#include "PopupOption.h"

I3_CLASS_INSTANCE(CPopupExtension, CPopupBase);

INT32 CPopupExtension::m_nWeaponFlag = 0;

CPopupExtension::CPopupExtension()
{
	m_pExtensionGui = NULL;
	m_pExtensionWin = NULL;
	m_pExtensionCaption = NULL;
	m_pLeave = NULL;
	m_pImageDummy = NULL;
	m_pName = NULL;
	m_pCategory = NULL;
	m_pDamageKey = NULL;
	m_pRapidKey = NULL;
	m_pHitRateKey = NULL;
	m_pVertRecoilKey = NULL;
	m_pHorzRecoilKey = NULL;
	m_pRangeKey = NULL;
	m_pBulletKey = NULL;
	m_pDamageValue = NULL;
	m_pRapidValue = NULL;
	m_pHitRateValue = NULL;
	m_pVertRecoilValue = NULL;
	m_pHorzRecoilValue = NULL;
	m_pSpeedValue = NULL;
	m_pBulletValue = NULL;
	m_pRangeValue = NULL;
	m_pDamageGage = NULL;
	m_pRapidGage = NULL;
	m_pHitRateGage = NULL;
	m_pVertRecoilGage = NULL;
	m_pHorzRecoilGage = NULL;
	m_pSpeedGage = NULL;
	m_pDescription = NULL;
	m_pExtensionKey = NULL;
	m_pExtensionValue = NULL;
	m_pControlKey = NULL;
	m_pControlValue = NULL;
	m_pExtensionDescription = NULL;
	m_pExtensionDummy = NULL;
	m_pExtensionControlSection = NULL;
	m_pSpeedKey = NULL;
}

CPopupExtension::~CPopupExtension()
{
}

void CPopupExtension::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_WEP_B_LEAVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnLeave();
		break;
	}
}

BOOL CPopupExtension::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "WeaponExtension", m_pExtensionGui, -1);
	LinkControlEx(plist, "weExtensionWin", m_pExtensionWin, -1);
	LinkControlEx(plist, "weExtensionCaption", m_pExtensionCaption, -1);
	LinkControlEx(plist, "weLeave", m_pLeave, GCI_WEP_B_LEAVE);
	LinkControlEx(plist, "weImageDummy", m_pImageDummy, -1);
	LinkControlEx(plist, "weName", m_pName, -1);
	LinkControlEx(plist, "weCategory", m_pCategory, -1);
	LinkControlEx(plist, "weDamageKey", m_pDamageKey, -1);
	LinkControlEx(plist, "weRapidKey", m_pRapidKey, -1);
	LinkControlEx(plist, "weHitRateKey", m_pHitRateKey, -1);
	LinkControlEx(plist, "weVertRecoilKey", m_pVertRecoilKey, -1);
	LinkControlEx(plist, "weHorzRecoilKey", m_pHorzRecoilKey, -1);
	LinkControlEx(plist, "weSpeedKey", m_pSpeedKey, -1);
	LinkControlEx(plist, "weBulletKey", m_pBulletKey, -1);
	LinkControlEx(plist, "weRangeKey", m_pRangeKey, -1);
	LinkControlEx(plist, "weDamageValue", m_pDamageValue, -1);
	LinkControlEx(plist, "weRapidValue", m_pRapidValue, -1);
	LinkControlEx(plist, "weHitRateValue", m_pHitRateValue, -1);
	LinkControlEx(plist, "weVertRecoilValue", m_pVertRecoilValue, -1);
	LinkControlEx(plist, "weHorzRecoilValue", m_pHorzRecoilValue, -1);
	LinkControlEx(plist, "weSpeedValue", m_pSpeedValue, -1);
	LinkControlEx(plist, "weBulletValue", m_pBulletValue, -1);
	LinkControlEx(plist, "weRangeValue", m_pRangeValue, -1);
	LinkControlEx(plist, "weDamageGage", m_pDamageGage, -1);
	LinkControlEx(plist, "weRapidGage", m_pRapidGage, -1);
	LinkControlEx(plist, "weHitRateGage", m_pHitRateGage, -1);
	LinkControlEx(plist, "weVertRecoilGage", m_pVertRecoilGage, -1);
	LinkControlEx(plist, "weHorzRecoilGage", m_pHorzRecoilGage, -1);
	LinkControlEx(plist, "weSpeedGage", m_pSpeedGage, -1);
	LinkControlEx(plist, "weDescription", m_pDescription, -1);
	LinkControlEx(plist, "weExtensionKey", m_pExtensionKey, -1);
	LinkControlEx(plist, "weExtensionValue", m_pExtensionValue, -1);
	LinkControlEx(plist, "weControlKey", m_pControlKey, -1);
	LinkControlEx(plist, "weControlValue", m_pControlValue, -1);
	LinkControlEx(plist, "weExtensionDescription", m_pExtensionDescription, -1);
	LinkControlEx(plist, "weExtensionDummy", m_pExtensionDummy, -1);
	LinkControlEx(plist, "weExtension", m_pExtensionControlSection, -1);	

	return FALSE;
}

void CPopupExtension::OnInitControl(void)
{
	m_pExtensionGui->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pExtensionCaption, GAME_STRING("STR_POPUP_DETAIL_INFO"));
	
	INIT_BUTTON_EX(m_pLeave);

	m_pName->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCategory->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	
	m_pDamageKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRapidKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pHitRateKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pVertRecoilKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pHorzRecoilKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBulletKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRangeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDamageKey->SetTextColor(GetColor(GCT_STATIC));
	m_pRapidKey->SetTextColor(GetColor(GCT_STATIC));
	m_pHitRateKey->SetTextColor(GetColor(GCT_STATIC));
	m_pVertRecoilKey->SetTextColor(GetColor(GCT_STATIC));
	m_pHorzRecoilKey->SetTextColor(GetColor(GCT_STATIC));
	m_pSpeedKey->SetTextColor(GetColor(GCT_STATIC));
	m_pRangeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pBulletKey->SetTextColor(GetColor(GCT_STATIC));


	{


#if defined (LOCALE_THAILAND) || defined (GUI_ADJUST_RUSSIA)
		INT32 FontSize = GAME_FONT_SIZE_ANY_TEXT;
		const i3TextFontSetInfo * pFontName = GetDefaultFontName();
		m_pDamageKey->ReCreateTextEx( pFontName, 32, FontSize, FALSE, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pRapidKey->ReCreateTextEx( pFontName, 32, FontSize, FALSE, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pHitRateKey->ReCreateTextEx( pFontName, 32, FontSize, FALSE, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pVertRecoilKey->ReCreateTextEx( pFontName, 32, FontSize, FALSE, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pHorzRecoilKey->ReCreateTextEx( pFontName, 32, FontSize, FALSE, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pSpeedKey->ReCreateTextEx( pFontName, 32, FontSize, FALSE, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pBulletKey->ReCreateTextEx( pFontName, 32, FontSize, FALSE, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pRangeKey->ReCreateTextEx( pFontName, 32, FontSize, FALSE, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pControlKey->ReCreateTextEx( pFontName, 32, FontSize, FALSE, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
#endif

	}

	m_pDamageKey->SetText(GAME_STRING("STR_POPUP_DAMAGE"));/*데미지*/
	m_pRapidKey->SetText(GAME_STRING("STR_POPUP_RAPID_RATE"));/*연사력*/
	m_pHitRateKey->SetText(GAME_STRING("STR_POPUP_HIT_RATE"));/*명중률*/
	m_pVertRecoilKey->SetText(GAME_STRING("STR_POPUP_WEAPON_VERT_RECOIL"));/*수직반동*/
	m_pHorzRecoilKey->SetText(GAME_STRING("STR_POPUP_WEAPON_HORZ_RECOIL"));/*수평반동*/
	m_pSpeedKey->SetText(GAME_STRING("STR_POPUP_WEAPON_SPEED"));/*이동속도*/
	m_pBulletKey->SetText(GAME_STRING("STR_POPUP_BULLETS"));/*장탄수*/
	m_pRangeKey->SetText(GAME_STRING("STR_POPUP_WEAPON_RANGE"));/*사정거리*/

	m_pDamageValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRapidValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pHitRateValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pVertRecoilValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pHorzRecoilValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSpeedValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBulletValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRangeValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDamageValue->SetTextColor(GetColor(GCT_STATIC));
	m_pRapidValue->SetTextColor(GetColor(GCT_STATIC));
	m_pHitRateValue->SetTextColor(GetColor(GCT_STATIC));
	m_pVertRecoilValue->SetTextColor(GetColor(GCT_STATIC));
	m_pHorzRecoilValue->SetTextColor(GetColor(GCT_STATIC));
	m_pSpeedValue->SetTextColor(GetColor(GCT_STATIC));
	m_pBulletValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pRangeValue->SetTextColor(GetColor(GCT_DEFAULT));

	m_pDamageGage->SetEnable(FALSE);
	m_pRapidGage->SetEnable(FALSE);
	m_pHitRateGage->SetEnable(FALSE);
	m_pVertRecoilGage->SetEnable(FALSE);
	m_pHorzRecoilGage->SetEnable(FALSE);
	m_pSpeedGage->SetEnable(FALSE);

	m_pDescription->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pDescription->SetTextColor(GetColor(GCT_DEFAULT));
	m_pDescription->SetTextAutoWrap(TRUE);

	m_pExtensionKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pExtensionKey->SetTextColor(GetColor(GCT_FOCUS));
	m_pExtensionKey->SetText(GAME_STRING("STR_POPUP_EXTENSION_TEXT"));	// Extension

	m_pExtensionValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pExtensionValue->SetTextColor(GetColor(GCT_FOCUS));

	m_pControlKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pControlKey->SetText(GAME_STRING("STR_POPUP_CONTROL"));/*조작 :*/
	m_pControlKey->SetTextColor(GetColor(GCT_FOCUS));

	m_pControlValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pControlValue->SetTextColor(GetColor(GCT_MY_SLOT));

	m_pExtensionDescription->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pExtensionDescription->SetTextColor(GetColor(GCT_FOCUS));

#if defined(GUI_ADJUST_RUSSIA)
	m_pExtensionDescription->SetTextSpace(0, -3);
#endif

	CPopupBase::OnInitControl();
}

void CPopupExtension::OnOpenPopup(void * pArg1, void * pArg2)
{
	char szWeaponName[ MAX_STRING_COUNT];
	char szDesc[2048];
	char szTemp[ MAX_STRING_COUNT];

	CPopupBase::OnOpenPopup();

	if (0 != m_nWeaponFlag)
	{
		char szString[MAX_STRING_COUNT] = "";

		g_pFramework->CreateWeaponDescImageRes( m_nWeaponFlag, m_pImageDummy, m_pExtensionDummy );

		// 무기 이름
		CWeaponInfo* pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo((WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(m_nWeaponFlag), GET_ITEM_FLAG_NUMBER(m_nWeaponFlag));
		I3ASSERT(pWeaponInfo);
		pWeaponInfo->GetWeaponInfoName( szWeaponName);
		m_pName->SetText( szWeaponName);

		// 카테고리
		g_pWeaponInfoDataBase->GetWeaponCategoryEx(pWeaponInfo, szString, MAX_STRING_COUNT);

		m_pCategory->SetText(szString);

		m_pDamageGage->SetEnable(FALSE);
		m_pRapidGage->SetEnable(FALSE);
		m_pHitRateGage->SetEnable(FALSE);
		m_pVertRecoilGage->SetEnable(FALSE);
		m_pHorzRecoilGage->SetEnable(FALSE);
		m_pSpeedGage->SetEnable(FALSE);
		m_pBulletValue->SetEnable(FALSE);
		m_pRangeValue->SetEnable(FALSE);


		// 데미지
		char strBuf[MAX_STRING_COUNT];
		INT32 nRate;

		nRate = pWeaponInfo->GetPercentage(WEAPON_INFO_DAMAGE);
		if( 0 <= nRate )
		{
			sprintf_s(strBuf, "%d", nRate);
			_ShowWeaponGage(WEAPON_INFO_DAMAGE, nRate);
			m_pDamageValue->SetText(strBuf);
		}
		else
		{
			m_pDamageValue->SetText("-");
		}

		// 연사력
		nRate = pWeaponInfo->GetPercentage(WEAPON_INFO_RAPID);
		if( 0 <= nRate)
		{
			sprintf_s(strBuf, "%d", nRate);
			_ShowWeaponGage(WEAPON_INFO_RAPID, nRate);		
			m_pRapidValue->SetText(strBuf);
		}
		else
		{
			m_pRapidValue->SetText("-");
		}

		// 명중률
		nRate = pWeaponInfo->GetPercentage(WEAPON_INFO_HIT_RATE);
		if( 0 <= nRate)
		{
			sprintf_s(strBuf, "%d", nRate);
			_ShowWeaponGage(WEAPON_INFO_HIT_RATE, nRate);		
			m_pHitRateValue->SetText(strBuf);
		}
		else
		{
			m_pHitRateValue->SetText("-");
		}

		// 수평반동
		nRate = pWeaponInfo->GetPercentage(WEAPON_INFO_VERT_RECOIL);
		if( 0 <= nRate )
		{
			sprintf_s(strBuf, "%d", nRate);
			_ShowWeaponGage(WEAPON_INFO_VERT_RECOIL, nRate);		
			m_pVertRecoilValue->SetText(strBuf);
		}
		else
		{
			m_pVertRecoilValue->SetText("-");
		}

		// 수직반동
		nRate = pWeaponInfo->GetPercentage(WEAPON_INFO_HORZ_RECOIL);
		if( 0 <= nRate )
		{
			sprintf_s(strBuf, "%d", nRate);
			_ShowWeaponGage(WEAPON_INFO_HORZ_RECOIL, nRate);		
			m_pHorzRecoilValue->SetText(strBuf);
		}
		else
		{
			m_pHorzRecoilValue->SetText("-");
		}

		// 이동속도
		nRate = pWeaponInfo->GetPercentage(WEAPON_INFO_SPEED);
		if( 0 <= nRate )
		{
			sprintf_s(strBuf, "%d", nRate);
			_ShowWeaponGage(WEAPON_INFO_SPEED, nRate);		
			m_pSpeedValue->SetText(strBuf);
		}
		else
		{
			m_pSpeedValue->SetText("-");
		}

		// 장탄수
		{
			pWeaponInfo->MakeBulletCountString(strBuf, MAX_STRING_COUNT);
			m_pBulletValue->SetText(strBuf);
			m_pBulletValue->SetEnable(TRUE);
		}

		// 사정거리
		{
			pWeaponInfo->MakeWeaponRangeString(strBuf, MAX_STRING_COUNT);
			m_pRangeValue->SetText(strBuf);
			m_pRangeValue->SetEnable(TRUE);
		}

		// 설명
		pWeaponInfo->GetDescription( szDesc);
		m_pDescription->SetText( szDesc);

		if( g_pWeaponInfoDataBase->IsExtensionWeapon( pWeaponInfo))
		{
			m_pExtensionControlSection->SetEnable( TRUE);	
			m_pExtensionDummy->SetEnable( TRUE);	

			// 익스텐션 명
			pWeaponInfo->GetExtensionName( szTemp);
			m_pExtensionValue->SetText( szTemp);

			// 익스텐션 기능
			pWeaponInfo->GetExtensionFeatures( szTemp);
			m_pExtensionDescription->SetText( szTemp);

			// 익스텐션 키	
			GAME_KEY_STRUCT* pKeyMap = g_pEnvSet->getGameKeyMap();

			m_pControlValue->SetText(CPopupOption::_GetKeyboardValue(&pKeyMap[GAME_KEY_MAP_SUBFUNC]));
		}
		else
		{
			m_pExtensionControlSection->SetEnable( FALSE);	
			m_pExtensionDummy->SetEnable( FALSE);	
		}
	}	

	m_pExtensionGui->SetEnable(TRUE);
	((CStageReady*)m_pParent)->ChangeStep(STEP_NULL);
}

void CPopupExtension::OnClosePopup(void * pArg1, void * pArg2)
{
	((CStageReady*)m_pParent)->ChangeStep(STEP_NULL);
	m_pExtensionGui->SetEnable(FALSE);

	g_pFramework->DestroyWeaponDescImageRes(); 

	CPopupBase::OnClosePopup();
}

void CPopupExtension::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupExtension::SetWeaponFlag(INT32 nWeaponFlag)
{
	WEAPON_CLASS_TYPE classType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(nWeaponFlag);
	INT32 number = GET_ITEM_FLAG_NUMBER(nWeaponFlag);

	nWeaponFlag -= number;

	GUI_WEAPON_VALID( classType, number );

	nWeaponFlag += number;

	m_nWeaponFlag = nWeaponFlag;	
}

void CPopupExtension::OnLeave(void)
{
	if (FALSE == ((CStageReady*)m_pParent)->IsSlidingInventory())
	{
		OnClosePopup();
	}
}

void CPopupExtension::_ShowWeaponGage(UINT32 nInfoType,UINT32 nRate)
{
	i3GuiStatic* pStatic = NULL;

	switch(nInfoType)
	{
	case WEAPON_INFO_DAMAGE:	pStatic = m_pDamageGage;	break;
	case WEAPON_INFO_RAPID:		pStatic = m_pRapidGage;		break;
	case WEAPON_INFO_HIT_RATE:	pStatic = m_pHitRateGage;	break;
	case WEAPON_INFO_VERT_RECOIL:	pStatic = m_pVertRecoilGage;	break;
	case WEAPON_INFO_HORZ_RECOIL:	pStatic = m_pHorzRecoilGage;	break;
	case WEAPON_INFO_SPEED:		pStatic = m_pSpeedGage;	break;
	default:
		return;
	}

	if( nRate > 0)
	{			
		i3GuiImage * pGageImage = pStatic->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);
		
		REAL32 fWidth = pGageImage->getOriginalWidth() * g_pGUIRoot->getGuiResolutionWidth();
		REAL32 fHeight = pGageImage->getOriginalHeight() * g_pGUIRoot->getGuiResolutionHeight();
			
		pStatic->SetEnable( TRUE);

		//	게이지			
		pGageImage->setWidth( ((REAL32)nRate / 100.0f) * fWidth);
		pGageImage->setHeight( fHeight);

		FLUSH_CONTROL(pStatic);
	}
	else
	{
		pStatic->SetEnable( FALSE);
	}
}
