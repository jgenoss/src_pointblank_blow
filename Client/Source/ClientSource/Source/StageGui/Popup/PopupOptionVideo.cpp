#include "pch.h"
#include "PopupOption.h"
#include "StageReady.h"
#include "GlobalVariables.h"

#include "UI/UIUtil.h"

/** \brief 임시 옵션창에 가려져 옆으로 옮깁니다. */
static void		RePositionMsgBox( UIMessageBox * pBox)
{
	if( pBox != NULL)
	{
		if( g_pFramework->IsBattleStage())
			pBox->SetPosition( pBox->GetPositionX() - 100, pBox->GetPositionY() + 100);
		else
			pBox->SetPosition( pBox->GetPositionX() - 100, pBox->GetPositionY());
	}
}

///////////////////////////////////////////////////////////////////
// Video Informations
INT32 CPopupOption::_findVideoRes( INT32 width, INT32 height)
{
	INT32 i;
	VID_INFO * pInfo;

	for( i = 0; i < getVidInfoCount(); i++)
	{
		pInfo = (VID_INFO *) getVidInfo( i);

		if((pInfo->m_Width == width) && (pInfo->m_Height == height))
			return i;
	}

	return -1;
}

void CPopupOption::_addVidInfo( I3G_DISPLAY_MODE_INFO * pInfo)
{
	VID_INFO * pNew;

	pNew = (VID_INFO *) i3MemAlloc( sizeof( VID_INFO));
	I3ASSERT( pNew != NULL);

	pNew->m_Width = pInfo->m_Width;
	pNew->m_Height = pInfo->m_Height;
	pNew->m_Hz = pInfo->m_RefreshRate;

	REAL32 ratio = (REAL32)pNew->m_Width / pNew->m_Height;
	if( ratio > 1.5f)
	{
		m_WideModeCount++;
	}
	else
	{
		m_NormalModeCount++;
	}

	m_VidList.Add( pNew);
}

void CPopupOption::_removeAllVidInfo(void)
{
	m_VidList.SafeFreeAll();
}

void CPopupOption::_updateResList( INT32 bits)
{
	INT32 i, modeCount;
	I3G_DISPLAY_MODE_INFO info;
	I3G_IMAGE_FORMAT fmt;
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();	
	char conv[256];

	_removeAllVidInfo();

	if( bits == 16)
		fmt = I3G_IMAGE_FORMAT_BGR_565;
	else
		fmt = I3G_IMAGE_FORMAT_BGRX_8888;

	modeCount = pCaps->getDisplayModeCount( fmt);

	m_NormalModeCount = m_WideModeCount = 0;

	INT32 maxW = ::GetSystemMetrics( SM_CXSCREEN);
	INT32 maxH = ::GetSystemMetrics( SM_CYSCREEN);

	// 물리적인 최대 해상도를 구한다.
	for( i = 0; i < modeCount; i++)
	{
		pCaps->getDisplayMode( i, fmt, &info);

		if( (info.m_Width >= maxW) && (info.m_Height >= maxH))
		{
			maxW = info.m_Width;
			maxH = info.m_Height;
		}
	}

	REAL32 ratioMonitor = (REAL32) maxW / maxH;
	bool bAdd;

	for( i = 0; i < modeCount; i++)
	{
		pCaps->getDisplayMode( i, fmt, &info);

		if( (info.m_Width < 1024) || (info.m_Height < 768))
			continue;


		REAL32 ratio = (REAL32) info.m_Width / info.m_Height;

		bAdd = false;

		if( ratioMonitor > 1.5f)
			bAdd = true;
		else if((ratioMonitor < 1.5f) && (ratio < 1.5f))
			bAdd = true;

		if( bAdd && (_findVideoRes( info.m_Width, info.m_Height) == -1))
		{
			_addVidInfo( &info);
		}
	}

	// 현재의 해상도가 있는지 확인한다.
	{
		INT32 w = ::GetSystemMetrics( SM_CXSCREEN);
		INT32 h = ::GetSystemMetrics( SM_CYSCREEN);

		if( _findVideoRes( w, h) == -1)
		{
			// 만약 현재의 해상도가 존재하지 않는다면...
			// 규칙과는 무관하게 현재의 해상도를 추가해둔다. 
			info.m_Width = w;
			info.m_Height = h;
			info.m_RefreshRate = 0;
			
			_addVidInfo( &info);
		}
	}

	m_pVideoResolutionPopup->Clear();
	m_pVideoResolutionPopup->SetDisplayCount( getVidInfoCount());
	m_pVideoResolutionPopup->SetSelColor(100, 100, 100, 100);
	m_pVideoResolutionPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pVideoResolutionPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	
	INT32 w = ::GetSystemMetrics( SM_CXSCREEN);
	INT32 h = ::GetSystemMetrics( SM_CYSCREEN);
	INT32 Count = getVidInfoCount();
	
	for( i = 0; i < Count; i++)
	{	
		VID_INFO * pInfo = getVidInfo( i);
		pInfo->m_CanApply = true;
		if(w <= pInfo->m_Width || h <= pInfo->m_Height )
		{
			// 바탕화면 해상도 보다 크면 지원안된다.
			pInfo->m_CanApply = false;
		}
		CEnvSet::GetVideoInfoString( pInfo->m_Width, pInfo->m_Height, (INT32)pInfo->m_Hz, pInfo->m_CanApply,  m_pVideoWindowMode->getCheckBoxState(), conv);	
		m_pVideoResolutionPopup->AddElement( conv);
	}

}



///////////////////////////////////////////////////////////////////
// AA Informations
void CPopupOption::_getAAString( INT32 aa, INT32 aaq, char * pszStr, const size_t sizeStrMax)
{
	switch( aa)
	{
		case 0 :		sprintf_s( pszStr, sizeStrMax, GAME_STRING("STR_POPUP_NONE_VALUE"));/*(없음)*/	break;
		default :		sprintf_s( pszStr, sizeStrMax, "x%d", aa);
			break;
	}

	if( aaq > 1)
	{
		char temp[32];

		sprintf_s( temp, 32, " (%dQ)", aaq); 
		i3String::Copy( pszStr, temp, sizeStrMax );
	}
}

void CPopupOption::_refreshAAInfo(void)
{
	// 인게임 캐시충전 팝업 기능을 사용할 경우 인게임 웹페이지 때문에 안티얼라이싱이 적용이 안됩니다. (강제로 적용할 경우 웹페이지 안나옵니다)
	// 안티얼라이싱을 사용하기 위해서는 인게임 내부에 웹페이지를 띄우는 방식이 아니라 별도의 인터넷 브라우저를 띄우는 방식으로 변경해야 합니다.
	if( g_pConfigEx->GetShop().ChargeCash_InGame)
		return;

	I3G_IMAGE_FORMAT fmt;
	INT32 i;
	I3G_AA_INFO * pInfo;
	char conv[256];

	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	_removeAllAAInfo();

	if( m_ScreenDepth == 32)
		fmt = I3G_IMAGE_FORMAT_BGRX_8888;
	else
		fmt = I3G_IMAGE_FORMAT_BGR_565;

	pCaps->buildAAList( fmt, !m_bFullScreen, &m_AAList);

	m_pVideoAntiAliasingPopup->SetDisplayCount( m_AAList.GetCount());
	m_pVideoAntiAliasingPopup->SetSelColor(100, 100, 100, 100);
	m_pVideoAntiAliasingPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pVideoAntiAliasingPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	for( i = 0; i < m_AAList.GetCount(); i++)
	{
		pInfo = (I3G_AA_INFO *) m_AAList.Items[i];

		_getAAString( pInfo->m_Sampling, pInfo->m_QualityCount, conv, 256);

		m_pVideoAntiAliasingPopup->AddElement( conv);
	}
}

void CPopupOption::_removeAllAAInfo( bool bDestructor)
{
	m_AAList.SafeFreeAll();

	if( bDestructor == false)
		m_pVideoAntiAliasingPopup->Clear();
}

INT32 CPopupOption::_findAA( INT32 type, INT32 quality)
{
	INT32 i;
	I3G_AA_INFO * pInfo;

	for( i = 0; i < m_AAList.GetCount(); i++)
	{
		pInfo = (I3G_AA_INFO *) m_AAList.Items[i];

		if((pInfo->m_Sampling == type) && (pInfo->m_QualityCount == quality))
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////
// Hz Informations

void CPopupOption::_refreshHzInfo(void)
{
	I3G_IMAGE_FORMAT fmt;
	UINT32 modeCount, i;
	I3G_DISPLAY_MODE_INFO info;
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();
	char conv[64];

	_removeAllHzInfo();

	if( m_ScreenDepth == 16)
		fmt = I3G_IMAGE_FORMAT_BGR_565;
	else
		fmt = I3G_IMAGE_FORMAT_BGRX_8888;

	modeCount = pCaps->getDisplayModeCount( fmt);

	m_HzList.Clear();

	for( i = 0; i < modeCount; i++)
	{
		pCaps->getDisplayMode( i, fmt, &info);

		if((info.m_Width == m_nScreenWidth) && (info.m_Height == m_nScreenHeight))
		{
			INT32 hz = (INT32 ) info.m_RefreshRate;

			m_HzList.Add( (void *) hz);
		}
	}

	m_pVideoScanRatePopup->Clear();

	if( m_bFullScreen == FALSE)
	{
		m_pVideoScanRatePopup->AddElement( GAME_STRING("STR_POPUP_WINDOWS_DEFAULT_VALUE_2"));/*(Windows 기본값)*/
		m_pVideoScanRatePopup->setInputDisable( TRUE);		
		m_pVideoScanRateButton->setControlDisable(TRUE);
		m_pVideoScanRateCombo->setInputDisable( TRUE);		
	}
	else
	{
		m_pVideoScanRatePopup->setInputDisable( FALSE);
		m_pVideoScanRateButton->setControlDisable( FALSE);
		m_pVideoScanRateCombo->setInputDisable( FALSE);
		m_pVideoScanRatePopup->SetDisplayCount( m_HzList.GetCount() + 1);
		m_pVideoScanRatePopup->SetSelColor(100, 100, 100, 100);
		m_pVideoScanRatePopup->SetTextColor(GetColor(GCT_DEFAULT));
		m_pVideoScanRatePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

		m_pVideoScanRatePopup->SetDisplayCount( m_HzList.GetCount());

		for( i = 0; i < (UINT32) m_HzList.GetCount(); i++)
		{
			sprintf_s( conv, "%d Hz", (INT32) m_HzList.Items[i]);

			m_pVideoScanRatePopup->AddElement( conv);
		}
	}
}

void CPopupOption::_removeAllHzInfo( bool bDestructor)
{
	m_HzList.Clear();

	if( bDestructor == false)
	{
		m_pVideoScanRatePopup->Clear();
	}
}

INT32 CPopupOption::_findHz( INT32 hz)
{
	INT32 i, idx = -1, mindiff = 100;

	for( i = 0; i < m_HzList.GetCount(); i++)
	{
		INT32 temphz = (INT32) m_HzList.Items[i];

		if( abs( temphz - hz) < mindiff)
		{
			idx = i;
			mindiff = abs( temphz - hz);
		}
	}

	return idx;
}

///////////////////////////////////////////////////////////////////
// Video Capture Resolution Informations

void CPopupOption::_refreshVCResInfo(void)
{
	m_pVideoCapturePopup->Clear();

	m_pVideoCapturePopup->setInputDisable( FALSE);
	m_pVideoCaptureButton->setControlDisable( FALSE);
	m_pVideoCaptureTooltip->setControlDisable( FALSE);
	m_pVideoCaptureCombo->setInputDisable( FALSE);
	m_pVideoCapturePopup->SetSelColor(100, 100, 100, 100);
	m_pVideoCapturePopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pVideoCapturePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	m_pVideoCapturePopup->AddElement( GAME_STRING("STR_POPUP_VIDEO_QUALITY_LOW"));
	m_pVideoCapturePopup->AddElement( GAME_STRING("STR_POPUP_VIDEO_QUALITY_DEFAULT"));
	m_pVideoCapturePopup->AddElement( GAME_STRING("STR_POPUP_VIDEO_QUALITY_HIGH"));

	char conv[128] = {0};

	switch(m_nVideoResolution)
	{
		case 0: i3String::Copy( conv, GAME_STRING("STR_POPUP_VIDEO_QUALITY_LOW"), 128 );		break;
		case 1: i3String::Copy( conv, GAME_STRING("STR_POPUP_VIDEO_QUALITY_DEFAULT"), 128 );	break;
		case 2: i3String::Copy( conv, GAME_STRING("STR_POPUP_VIDEO_QUALITY_HIGH"), 128 );		break;
	}	

	m_pVideoCaptureCombo->SetText(conv);
}

void CPopupOption::_removeAllVCResInfo(bool bDestructor)
{
	if( bDestructor == false)
	{
		m_pVideoCapturePopup->Clear();
	}
}


///////////////////////////////////////////////////////////////////
void CPopupOption::OnVideoResolutionCombo(void)
{	
	_updateResList( m_ScreenDepth);
	if( !m_bFullScreen && g_pViewer->isFullScreen())
		return;
	ExclusionPopup(m_pVideoResolutionPopup);
	
}

void CPopupOption::OnVideoResolutionPopup(UINT32 idxItem)
{
	if (m_pVideoResolutionPopup->GetElementCount() <= static_cast<INT32>(idxItem) )
		return;


	if(m_pVideoWindowMode->getCheckBoxState() )
	{
		VID_INFO * pInfo = getVidInfo(idxItem);
		if( pInfo->m_CanApply )
		{
			_SetVideoResolution(idxItem);
		}
	}
	else
		_SetVideoResolution(idxItem);

}

void CPopupOption::OnVideoColorCombo(void)
{
	ExclusionPopup(m_pVideoColorPopup);
}

void CPopupOption::OnVideoColorPopup(UINT32 idxItem)
{
	switch( idxItem)
	{
		case 0 :		m_ScreenDepth = 16;	break;
		default :		m_ScreenDepth = 32;	break;
	}

	_SetVideoColor();
}

void CPopupOption::OnVideoScanRateCombo(void)
{
	ExclusionPopup(m_pVideoScanRatePopup);
}

void CPopupOption::OnVideoScanRatePopup(UINT32 idxItem)
{
	_SetVideoScanRate(idxItem);
}

void CPopupOption::OnVideoAntiAliasingCombo(void)
{
	ExclusionPopup(m_pVideoAntiAliasingPopup);
}

void CPopupOption::OnVideoAntiAliasingPopup(UINT32 idxItem)
{
	_SetVideoAntiAliasing(idxItem);
}

void CPopupOption::OnVideoCaptureCombo(void)
{
	ExclusionPopup(m_pVideoCapturePopup);
}

void CPopupOption::OnVideoCapturePopup(UINT32 idxItem)
{
	m_nVideoResolution = idxItem;

	_SetVideoCaptureResolution();
}

void CPopupOption::OnVideoWindowMode(void)
{
	//STRING_POPUP(GAME_STRING("STBL_IDX_EP_OPTION_RESTART_MESSAGE"));

	m_bFullScreen = !m_pVideoWindowMode->getCheckBoxState();	

	_SetVideoMode( false);
}

void CPopupOption::OnVideoScreenSync(void)
{
	m_bVSync = m_pVideoScreenSync->getCheckBoxState();
}

void CPopupOption::OnVideoSmoothDistance(void)
{
	m_bTriLinearFilter = m_pVideoSmoothDistance->getCheckBoxState();
}

void CPopupOption::OnVideoGammaPrev(void)
{
	if( m_fDelayLocalTime == 0.0f )
	{
		_SetVideoGammaDown();	
	}
	else
	{
		if( m_fDelayLocalTime > 0.3f)
		{
			_SetVideoGammaDown();	
		}
	}

	m_fDelayLocalTime += m_fElapsedTime;
}

void CPopupOption::OnVideoGammaNext(void)
{
	if( m_fDelayLocalTime == 0.0f )
	{
		_SetVideoGammaUp();	
	}
	else
	{
		if( m_fDelayLocalTime > 0.3f)
		{
			_SetVideoGammaUp();
		}
	}

	m_fDelayLocalTime += m_fElapsedTime;
}

///////////////////////////////////////////////////////////////////
void CPopupOption::_ResetVideo(void)
{
	i3RenderContext * pCtx = g_pViewer->GetRenderContext();
	i3VideoInfo * pInfo = pCtx->GetVideoInfo();

	_SetVideoMode( true);

	// 해상도
	{
		INT32 idx;
		
			_updateResList( m_ScreenDepth);
			// 현재 해상도 선택
			idx = _findVideoRes( m_nScreenWidth, m_nScreenHeight);
			

		if( idx == -1)
			idx = 0;

		_SetVideoResolution( idx, true);
	}

	// 주사율
	{
		INT32 idx;

		if( m_bFullScreen)
		{
			idx = _findHz( (INT32) pInfo->m_RefreshRate);
			if( idx == -1)
				idx = 0;
		}
		else
		{
			idx = 0;
		}

		_SetVideoScanRate( idx);
	}

	_SetVideoColor( true);

	// Anti-Aliasing
	{
		_refreshAAInfo();

		INT32 idx = _findAA( pInfo->m_MultiSampling, pInfo->m_MultiSamplingQuality);
		if( idx == -1)
			idx = 0;

		_SetVideoAntiAliasing( idx);
	}

	// Anti-Aliasing
	{
		_refreshVCResInfo();

		_SetVideoCaptureResolution();
	}

	// VSync
	{
		_SetVideoVSync();
	}

	// Trilinear Filter
	{
		_SetVideoTriLinearFilter();
	}

	// Video Capture Resolution
	{
		_SetVideoCaptureResolution();
	}
	
	if (g_pViewer->isFullScreen())
	{
		char szTemp[MAX_PATH] = {0,};
		
		sprintf_s( szTemp, "%d", (INT32) m_GammaVal);
		m_pVideoGammaNumber->SetIMEText( szTemp);

		SetGammaState();
	}
	else
	{
		m_pVideoGammaNumber->SetIMEText( "45");
	}
}

void CPopupOption::_SetGammaGage(UINT32 value)
{
	if (0 == value)
	{
		m_pVideoGammaGage->SetEnable(FALSE);
	}
	else
	{
		REAL32 rateX = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;
		REAL32 rTotal = m_pVideoGammaNext->getPositionX() * rateX;
		rTotal -= m_pVideoGammaNext->getWidth();

		i3Sprite2D* pSprite = m_pVideoGammaGage->getSprite();
		
		m_pVideoGammaGage->setSize( INT32((rTotal * value) / 100), m_pVideoGammaGage->getHeight());
		pSprite->SetRect(GUI_SHAPE_OFFSET, m_pVideoGammaGage->getTotalPosX(), m_pVideoGammaGage->getTotalPosY(), (rTotal * value) / 100.0f, (REAL32)m_pVideoGammaGage->getHeight());
		m_pVideoGammaGage->SetEnable(TRUE);
	}
}


void CPopupOption::_InitVideo(void)
{
	// Display Modes
	{	

		
		m_pVideoResolutionKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoResolutionKey->SetTextColor(GetColor(GCT_STATIC));
		m_pVideoResolutionKey->SetText(GAME_STRING("STR_POPUP_SCREEN_RESOLUTION"));/*해상도*/
		m_pVideoResolutionCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoResolutionCombo->SetTextColor(GetColor(GCT_DEFAULT));
		INIT_COMBO_EX(m_pVideoResolutionButton);	

		m_pVideoResolutionPopup->CreateTextEx(10,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pVideoResolutionPopup->SetSelColor(100, 100, 100, 100);
		m_pVideoResolutionPopup->SetTextColor(GetColor(GCT_DEFAULT));
		m_pVideoResolutionPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoResolutionPopup->SetExclusiveEnableControl(TRUE);
		m_pVideoResolutionPopup->SetEnable(FALSE);

		_updateResList( g_pEnvSet->m_ScreenDepth);	
	}

	// 색상
	{
		m_pVideoColorKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoColorKey->SetTextColor(GetColor(GCT_STATIC));
		m_pVideoColorKey->SetText(GAME_STRING("STR_POPUP_VIDEO_COLOR"));/*색상*/
		m_pVideoColorCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoColorCombo->SetTextColor(GetColor(GCT_DEFAULT));
		INIT_COMBO_EX(m_pVideoColorButton);
		m_pVideoColorPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pVideoColorPopup->SetSelColor(100, 100, 100, 100);	
		m_pVideoColorPopup->SetTextColor(GetColor(GCT_DEFAULT));
		m_pVideoColorPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoColorPopup->SetExclusiveEnableControl(TRUE);

		m_pVideoColorPopup->AddElement( "16 bits");
		m_pVideoColorPopup->AddElement( "32 bits");

		m_pVideoColorPopup->SetEnable(FALSE);
	}

	// 주사율
	{
		// 주사율 선택 기능을 제거합니다. 이진균
		//m_pVideoScanRateKey->SetEnable( FALSE);
		m_pVideoScanRateKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoScanRateKey->SetTextColor(GetColor(GCT_STATIC));
		m_pVideoScanRateKey->SetText(GAME_STRING("STR_POPUP_SCREEN_SCAN_LINE"));/*주사율*/

		//m_pVideoScanRateCombo->SetEnable( FALSE);
		m_pVideoScanRateCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoScanRateCombo->SetTextColor(GetColor(GCT_DEFAULT));
		INIT_COMBO_EX(m_pVideoScanRateButton);

		//m_pVideoScanRatePopup->SetEnable( FALSE);
		m_pVideoScanRatePopup->CreateTextEx(2, GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pVideoScanRatePopup->SetSelColor(100, 100, 100, 100);		
		m_pVideoScanRatePopup->SetTextColor(GetColor(GCT_DEFAULT));
		m_pVideoScanRatePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoScanRatePopup->SetExclusiveEnableControl(TRUE);
		m_pVideoScanRatePopup->SetEnable(FALSE);
	}

	// 엘리어싱 제거
	{
		// 인게임 캐시충전 팝업 기능을 사용할 경우 인게임 웹페이지 때문에 안티얼라이싱이 적용이 안됩니다. (강제로 적용할 경우 웹페이지 안나옵니다)
		// 안티얼라이싱을 사용하기 위해서는 인게임 내부에 웹페이지를 띄우는 방식이 아니라 별도의 인터넷 브라우저를 띄우는 방식으로 변경해야 합니다.
		if( g_pConfigEx->GetShop().ChargeCash_InGame)
		{	// 안티얼라이싱을 사용못하게 막습니다.
			m_pVideoAntiAliasingKey->SetEnable( FALSE);
			m_pVideoAntiAliasingCombo->SetEnable( FALSE);
			m_pVideoAntiAliasingPopup->SetEnable( FALSE);
			m_pVideoAntiAliasingButton->SetEnable( FALSE);
			m_pArrowVideoAntiAliasing->SetEnable( FALSE);
		}
		else
		{
			m_pVideoAntiAliasingKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
			m_pVideoAntiAliasingKey->SetTextColor(GetColor(GCT_STATIC));
			m_pVideoAntiAliasingKey->SetText(GAME_STRING("STR_POPUP_DISABLE_ANTIALIASING"));/*앨리어싱 제거*/
			m_pVideoAntiAliasingCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
			m_pVideoAntiAliasingCombo->SetTextColor(GetColor(GCT_DEFAULT));
			INIT_COMBO_EX(m_pVideoAntiAliasingButton);

			m_pVideoAntiAliasingPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
			m_pVideoAntiAliasingPopup->SetSelColor(100, 100, 100, 100);
			m_pVideoAntiAliasingPopup->SetTextColor(GetColor(GCT_DEFAULT));
			m_pVideoAntiAliasingPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
			m_pVideoAntiAliasingPopup->SetExclusiveEnableControl(TRUE);
			m_pVideoAntiAliasingPopup->SetEnable(FALSE);
			m_pVideoAntiAliasingPopup->SetExclusiveEnableControl(TRUE);
		}

		// Dummy text for AntiAliasing comboBox
	}

	// 동영상 해상도 옵션
	{
		m_pVideoCaptureKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoCaptureKey->SetTextColor(GetColor(GCT_STATIC));
		m_pVideoCaptureKey->SetText(GAME_STRING("STR_POPUP_VIDEO_CAPTURE_RESOLUTION"));

		m_pVideoCaptureCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoCaptureCombo->SetTextColor(GetColor(GCT_DEFAULT));
		INIT_COMBO_EX(m_pVideoCaptureButton);
		INIT_COMBO_EX(m_pVideoCaptureTooltip);

		m_pVideoCapturePopup->CreateTextEx(3, GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pVideoCapturePopup->SetSelColor(100, 100, 100, 100);		
		m_pVideoCapturePopup->SetTextColor(GetColor(GCT_DEFAULT));
		m_pVideoCapturePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoCapturePopup->SetExclusiveEnableControl(TRUE);

		m_pVideoCapturePopup->AddElement( GAME_STRING("STR_POPUP_VIDEO_QUALITY_LOW"));
		m_pVideoCapturePopup->AddElement( GAME_STRING("STR_POPUP_VIDEO_QUALITY_DEFAULT"));
		m_pVideoCapturePopup->AddElement( GAME_STRING("STR_POPUP_VIDEO_QUALITY_HIGH"));		

		m_pVideoCapturePopup->SetEnable(FALSE);

		// 동영상 녹화를 사용하지 않는 국가는 옵션 비활성화
		if(!LocaleValue::Enable("EnableVideoCapture"))
		{
			m_pArrowVideoCapture->SetEnable(FALSE);
		}
	}

	// CheckBoxes
	{
		m_pVideoWindowModeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoScreenSyncKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoSmoothDistanceKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);		
		m_pVideoWindowModeKey->SetTextColor(GetColor(GCT_STATIC));
		m_pVideoScreenSyncKey->SetTextColor(GetColor(GCT_STATIC));
		m_pVideoSmoothDistanceKey->SetTextColor(GetColor(GCT_STATIC));
#if defined( ENABLE_ALT_ENTER)
		m_pVideoWindowModeKey->SetText(GAME_STRING("STR_POPUP_WINDOW_MODE_KEY"));/*창모드 {col:255,120, 0, 255}(Alt + Enter){/col}*/
#else
		m_pVideoWindowModeKey->SetText(GAME_STRING("STR_POPUP_WINDOW_MODE"));/*창모드*/
#endif
		m_pVideoScreenSyncKey->SetText(GAME_STRING("STR_POPUP_VIDEO_SYNC"));/*화면 동기화*/
		m_pVideoSmoothDistanceKey->SetText(GAME_STRING("STR_POPUP_SOFT_PERSPECTIVE"));/*부드러운 원근처리*/

#ifdef WINDOW_MODE_DISABLE
		m_pVideoWindowModeKey->SetEnable( FALSE);
		m_pVideoWindowMode->SetEnable( FALSE);
#endif
	}

	// Gamma
	{
		m_pVideoGammaKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoGammaKey->SetTextColor(GetColor(GCT_STATIC));
		m_pVideoGammaKey->SetText(GAME_STRING("STR_POPUP_SCREEN_BRIGHTNESS"));/*밝기*/		

		CREATE_IME_ELEMENT ime;
		
		ime._hWnd = g_hWnd;
		ime._hInst = g_hInstance;
		ime._nTextLimitCount = 3;
		ime._StyleIME = IME_STYLE_NUMERIC_ONLY | IME_STYLE_IGNORE_KEYINPUT_IME_MODE;

		m_pVideoGammaNumber->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
		m_pVideoGammaNumber->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pVideoGammaNumber->SetTextColor(GetColor(GCT_DEFAULT));
		m_pVideoGammaNumber->SetAlphaNumericOnly( true);

		INIT_REPEAT_COMBO(m_pVideoGammaPrev);
		INIT_REPEAT_COMBO(m_pVideoGammaNext);
	}

}

////////////////////////////////////////////////////////////////
// 게임내에서의 Video 설정
////////////////////////////////////////////////////////////////
void CPopupOption::_SetVideoOptionEnable()
{
	// 인게임시 원래 설정 유지
	if(m_bInGame)
		return;

	// 영상 녹화중에는 비디오 설정을 비활성화 시킨다. // 실시간으로 되어야한다
	if(g_pFramework->IsVideoCapturing())
	{
		COLOR col;
		i3Color::Set( &col, (UINT8) 180, 180, 180, 100);
		m_sect[OS_VIDEO]._pButton->setInputDisable( TRUE);
		m_sect[OS_VIDEO]._pButton->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);
		m_sect[OS_VIDEO]._pButton->SetCaptionColor( &col);
		m_sect[OS_VIDEO]._pButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_sect[OS_VIDEO]._pSect->SetEnable( FALSE);

		if(m_currentSect == OS_VIDEO)
			OnPlay();
	}
	else
	{
		COLOR col;
		i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
		m_sect[OS_VIDEO]._pButton->setInputDisable( FALSE);
		m_sect[OS_VIDEO]._pButton->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);
		m_sect[OS_VIDEO]._pButton->SetCaptionColor( &col);
	}
}


void CPopupOption::_SetVideoMode( bool bInit)
{
	m_pVideoWindowMode->setCheckBoxState( !m_bFullScreen);

	if( bInit == false)
	{
		_refreshAAInfo();
		_refreshHzInfo();
		_refreshVCResInfo();
	}
}

void CPopupOption::_SetVideoResolution(INT32 idxResolution, bool bInit)
{
	char szTemp[MAX_PATH] = { 0, };
	VID_INFO * pInfo = getVidInfo( idxResolution);
	
	m_nScreenWidth = pInfo->m_Width;
	m_nScreenHeight = pInfo->m_Height;
	
	CEnvSet::GetVideoInfoString( pInfo->m_Width, pInfo->m_Height, (INT32) pInfo->m_Hz, TRUE,m_pVideoWindowMode->getCheckBoxState(), szTemp);
	m_pVideoResolutionCombo->SetText( szTemp);

	_refreshHzInfo();

	if( bInit == false)
	{
		INT32 idx = _findHz( 60);

		if( idx == -1)
			idx = 0;

		_SetVideoScanRate( idx);
	}
}

void CPopupOption::_SetVideoColor( bool bInit)
{
	char conv[32];

	sprintf_s( conv, "%d bits", m_ScreenDepth);
	m_pVideoColorCombo->SetText( conv);

	_updateResList( m_ScreenDepth);


	// 현재 선택한 해상도가 있는지 확인하고 선택해 준다.
	INT32 idx = _findVideoRes( m_nScreenWidth, m_nScreenHeight);

	if( idx == -1)
	{
		idx = 0;		// 해당 해상도가 없다면, 기본 해상도로 선택
	}

	if( bInit == false)
	{
		_SetVideoResolution( idx);

		_refreshAAInfo();
	}
}

void CPopupOption::_SetVideoScanRate(INT32 idxScanRate)
{
	INT32 hz = (INT32) m_HzList.Items[ idxScanRate];
	char conv[256];

	if( m_bFullScreen)
	{
		sprintf_s( conv, "%d Hz", hz);
	}
	else
	{
		sprintf_s( conv, GAME_STRING("STR_POPUP_WINDOWS_DEFAULT_VALUE"));/*Windows 기본값*/
	}

	m_pVideoScanRateCombo->SetText( conv);

	m_nScreenHz = hz;
}

void CPopupOption::_SetVideoAntiAliasing(INT32 idx)
{
	// 인게임 캐시충전 팝업 기능을 사용할 경우 인게임 웹페이지 때문에 안티얼라이싱이 적용이 안됩니다. (강제로 적용할 경우 웹페이지 안나옵니다)
	// 안티얼라이싱을 사용하기 위해서는 인게임 내부에 웹페이지를 띄우는 방식이 아니라 별도의 인터넷 브라우저를 띄우는 방식으로 변경해야 합니다.
	if( g_pConfigEx->GetShop().ChargeCash_InGame)
		return;

	I3G_AA_INFO * pInfo = (I3G_AA_INFO *) m_AAList.Items[ idx];
	char conv[128];

	_getAAString( pInfo->m_Sampling, pInfo->m_QualityCount, conv, 128);

	m_pVideoAntiAliasingCombo->SetText( conv);
	m_AA = pInfo->m_Sampling;
	m_AAQ = pInfo->m_QualityCount;
}

void CPopupOption::_SetVideoCaptureResolution()
{
	char conv[128] = {0};

	switch(m_nVideoResolution)
	{
		case 0: i3String::Copy( conv, GAME_STRING("STR_POPUP_VIDEO_QUALITY_LOW"), 128 );		break;
		case 1: i3String::Copy( conv, GAME_STRING("STR_POPUP_VIDEO_QUALITY_DEFAULT"), 128 );	break;
		case 2: i3String::Copy( conv, GAME_STRING("STR_POPUP_VIDEO_QUALITY_HIGH"), 128 );		break;
	}	

	g_pFramework->SetVideoCaptureInfo(m_nVideoResolution);

	m_pVideoCaptureCombo->SetText(conv);
}


void CPopupOption::_SetVideoVSync(void)
{
	m_pVideoScreenSync->setCheckBoxState( m_bVSync);
}

void CPopupOption::_SetVideoTriLinearFilter(void)
{
	m_pVideoSmoothDistance->setCheckBoxState( m_bTriLinearFilter);
}

void CPopupOption::_SetVideoGammaUp(void)
{    
	char szTemp[MAX_PATH];

	if( g_pViewer->isFullScreen())
	{
		m_GammaVal += 1.0f;
		if( m_GammaVal > 100) m_GammaVal = 100.0f;
		
		sprintf_s( szTemp, "%d", (INT32) m_GammaVal);
		m_pVideoGammaNumber->SetIMEText( szTemp);

		SetGammaState();
	}
	else
	{
		// 윈도우 모드에서는 화면밝기를\n\n조절할수 없습니다.
		UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_STRING("STBL_IDX_EP_OPTION_GAMMA_MESSAGE"));
		RePositionMsgBox( pBox);
	}
}

void CPopupOption::_SetVideoGammaDown(void)
{
	char szTemp[MAX_PATH];

	if( g_pViewer->isFullScreen())
	{
		m_GammaVal -= 1.0f;
		if( m_GammaVal < 0) m_GammaVal = 0.0f;

		sprintf_s( szTemp, "%d", (INT32) m_GammaVal);
		m_pVideoGammaNumber->SetIMEText( szTemp);	

		SetGammaState();
	}
	else
	{
		// 윈도우 모드에서는 화면밝기를\n\n조절할수 없습니다.
		UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_STRING("STBL_IDX_EP_OPTION_GAMMA_MESSAGE"));
		RePositionMsgBox( pBox);
	}
}

void CPopupOption::SetGammaState(void)
{
	INT32 i = 0;
	D3DGAMMARAMP ramp;
	REAL32 fGammaScale = 1.0f * 0.01f;
	REAL32 fGamma = fGammaScale * m_GammaVal + 0.5f;
	
	for( i=0; i<256; i++) 
	{
		WORD f = (WORD)(255 * pow((float)i/256, 1/fGamma));
		if (f > 255) 	f = 255; 
		ramp.red[i] = ramp.green[i] = ramp.blue[i] = ((WORD)f)<<8; 
	}
	
	// 감마설정
	((IDirect3DDevice9 *)g_pViewer->GetRenderContext()->GetNativeDevice())->SetGammaRamp( 0, D3DSGR_NO_CALIBRATION, &ramp );
}

void	CPopupOption::UpdateVideo( REAL32 rDeltaSeconds )
{	
	if (g_pViewer->isFullScreen())
	{	if(m_pVideoWindowMode->getCheckBoxState())
		{
			_SetVideoResolution(0);
		}
	m_pVideoGammaNumber->setInputDisable(FALSE);
	_UpdateGammaValue(rDeltaSeconds);
	_SetGammaGage((INT32)m_GammaVal);
	}
	else
	{	
		m_pVideoGammaNumber->setInputDisable(TRUE);
		_SetGammaGage(45);
	}

	// 사운드 문제로 인해서 일단 집어 넣습니다 - raja
	m_pVideoGammaPrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pVideoGammaNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);

#if 0	// TTP #384 - 막습니다.
	// 감마 컨트롤 지원 가능여부
	if( g_pViewer->GetRenderContext()->GetCaps()->getEnableGamma())
	{
		if(g_pFramework->getSysKeyPress() & (GAME_KEY_LEFT))
		{
			m_GammaVal -= 0.01f;
			SetGammaState();
		}
		else if(g_pFramework->getSysKeyPress() & (GAME_KEY_RIGHT))
		{
			m_GammaVal += 0.01f;
			SetGammaState();
		}
	}
#endif
}

void CPopupOption::_UpdateGammaValue(REAL32 rDeltaSeconds)
{
	if (m_pVideoGammaNumber->getIsFocus())
	{
		m_pVideoGammaNumber->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}
	else
	{
		char temp[MAX_STRING_COUNT];

		INT32 value = i3String::ToInt(m_pVideoGammaNumber->getIMETextBuf());
		if (0 > value || 100 < value)		
		{
			/*밝기는 0에서 100사이로 설정하여 주시기 바랍니다.*/
			UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_SCREEN_BRIGHTNESS"));
			RePositionMsgBox( pBox);
		}
			
		sprintf_s(temp, "%d", (INT32)m_GammaVal);
		m_pVideoGammaNumber->SetIMEText(temp);
		m_pVideoGammaNumber->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}

	if (GUI_IME_CHANGING(m_pVideoGammaNumber))
	{
		if (m_pVideoGammaNumber->getIsFocus())
		{
			const char* text = m_pVideoGammaNumber->getIMETextBuf();			
			INT32 value = i3String::ToInt(text);

			// 유효범위 (0, 100)
			// 단, ToInt()의 에러값이 0이기 때문에, 0만 별도로 검사한다.
			if ((0 <= value && 100 >= value) || 0 == i3String::Compare(text, "0"))
			{
				m_GammaVal = (REAL32)value;
				SetGammaState();
			}			
		}
	}

	if (GUI_IME_CONFIRM(m_pVideoGammaNumber))
	{
		if (m_pVideoGammaNumber->getIsFocus())
		{
			const char* text = m_pVideoGammaNumber->getIMETextBuf();			
			INT32 value = i3String::ToInt(text);

			m_pVideoGammaNumber->InputDone();

			// 유효범위 (0, 100)
			// 단, ToInt()의 에러값이 0이기 때문에, 0만 별도로 검사한다.
			if ((0 <= value && 100 >= value) || 0 == i3String::Compare(text, "0"))
			{
				m_GammaVal = (REAL32)value;
				SetGammaState();
			}
			
			m_pVideoGammaNumber->SetFocus(FALSE);
			i3GuiRoot::setFocusControl(NULL);
		}
		else
		{
			m_pVideoGammaNumber->InputDone();
		}
	}
}

bool CPopupOption::_NeedToUserVerify(void)
{
	if( g_pEnvSet->m_bFullScreen != m_bFullScreen)
		return true;

	if( g_pEnvSet->m_nScreenWidth != m_nScreenWidth)
		return true;

	if( g_pEnvSet->m_nScreenHeight != m_nScreenHeight)
		return true;

	if( g_pEnvSet->m_nScreenHz != m_nScreenHz)
		return true;

	if( g_pEnvSet->m_AA != m_AA)
		return true;

	if( g_pEnvSet->m_AAQ != m_AAQ)
		return true;

	if( g_pEnvSet->m_b16BitsDepth != m_b16BitsDepth)
		return true;

	if( g_pEnvSet->m_bVSync != m_bVSync)
		return true;

	if( g_pEnvSet->m_ScreenDepth != m_ScreenDepth)
		return true;

	if( g_pEnvSet->m_bTriLinearFilter != m_bTriLinearFilter)
		return true;
	
	if(g_pEnvSet->m_bEnableNormalMap != m_bEnableNormalMap)
		return true;

	if(g_pEnvSet->m_nSpecularQualityType != m_nSpecularQualityType)
		return true;

	if(g_pEnvSet->m_nTextureQualityType != m_nTextureQualityType)
		return true;

	if(g_pEnvSet->m_nShadowQualityType != m_nShadowQualityType)
		return true;

	if(g_pEnvSet->m_nVideoResolution != m_nVideoResolution)
		return true;

	return false;
}

bool CPopupOption::_NeedToRestart(void)
{
	if( g_pEnvSet->m_bEnableNormalMap != m_bEnableNormalMap)
		return true;

	if( g_pEnvSet->m_nSpecularQualityType != m_nSpecularQualityType)
		return true;

	return false;
}

void CPopupOption::_SetVideo( bool bRestore)
{
	i3Viewer * pViewer = g_pFramework->GetViewer();
	i3VideoInfo* pVideo;

	pVideo = pViewer->GetRenderContext()->GetVideoInfo();
//	pViewer->GetVideoInfo( &video);
#if !defined( UI2_RESOLUTION_FIX)
	if( bRestore)
	{
		// 호환성 문제로 지정된 해상도로 제대로 설정되지 않아,
		// 사용자가 선택을 취소한 경우...
		pVideo->m_bFullScreen			= g_pEnvSet->m_bFullScreen;

		pVideo->m_Width				= g_pEnvSet->m_nScreenWidth;
		pVideo->m_Height				= g_pEnvSet->m_nScreenHeight;
		pVideo->m_RefreshRate			= (REAL32) g_pEnvSet->m_nScreenHz;
		pVideo->m_bVSync				= g_pEnvSet->m_bVSync;

		if( g_pEnvSet->m_ScreenDepth == 32)
			pVideo->m_RenderTargetFormat	= I3G_IMAGE_FORMAT_BGRX_8888;
		else
			pVideo->m_RenderTargetFormat	= I3G_IMAGE_FORMAT_BGR_565;

		if( g_pEnvSet->m_b16BitsDepth)
			pVideo->m_ZBufferBits			= 16;
		else
			pVideo->m_ZBufferBits			= 24;

		pVideo->m_MultiSampling			= g_pEnvSet->m_AA;
		pVideo->m_MultiSamplingQuality	= g_pEnvSet->m_AAQ;

		m_nVideoResolution = g_pEnvSet->m_nVideoResolution;
	}
	else
	{
		pVideo->m_bFullScreen			= m_bFullScreen;

		pVideo->m_Width				= m_nScreenWidth;
		pVideo->m_Height				= m_nScreenHeight;
		pVideo->m_RefreshRate			= (REAL32) m_nScreenHz;
		pVideo->m_bVSync				= m_bVSync;

		if( m_ScreenDepth == 32)
			pVideo->m_RenderTargetFormat	= I3G_IMAGE_FORMAT_BGRX_8888;
		else
			pVideo->m_RenderTargetFormat	= I3G_IMAGE_FORMAT_BGR_565;

		if( m_b16BitsDepth)
			pVideo->m_ZBufferBits			= 16;
		else
			pVideo->m_ZBufferBits			= 24;

		pVideo->m_MultiSampling			= m_AA;
		pVideo->m_MultiSamplingQuality	= m_AAQ;
	}
#endif

	_SetVideoCaptureResolution();

	// UI 리뉴얼 해상도 고정 기능 : 하동익 추후 Config에 반영 여부 정해야할듯. (2011-10-05)
#if !defined( UI2_RESOLUTION_FIX)
	pViewer->SetVideoInfo( pVideo);
	if( i3UI::getManager() != NULL)
	{
		i3UI::getManager()->SetUIResolution( m_nScreenWidth, m_nScreenHeight);
	}
#endif

	SetGammaState();
}

void CPopupOption::_PopupUserVerify(void)
{
	// 설정한 해상도를 일단 변경
	_SetVideo( false);
	// UI 리뉴얼 해상도 고정 기능 : 하동익 추후 Config에 반영 여부 정해야할듯. (2011-10-05)
	if( g_pEnvSet->m_nScreenHz != i3RenderContext::GetCaps()->getDefaultRefreshRate())
	{
		/*현재 설정은 모니터의 주사율과의 차이로 인해 화면이 느려지는 문제가 발생할 수 있습니다.\n현재 설정을 계속 사용하시려면 확인 버튼을 선택해주세요.*/
		UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_STRING("STR_POPUP_Q_MESSAGE_CONTINUE_TO_CONFIRM2"), NULL, MAKE_CALLBACK(_CbUserVerify), this);
		RePositionMsgBox( pBox);
	}
	else
	{
		/*현재 설정을 계속 사용하시려면 확인 버튼을 선택해주세요.*/
		UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_STRING("STR_POPUP_Q_MESSAGE_CONTINUE_TO_CONFIRM"), NULL, MAKE_CALLBACK(_CbUserVerify), this);
		RePositionMsgBox( pBox);
	}
}

void CPopupOption::_CbUserVerify( void * pThis, INT32 nParam)
{
	CPopupOption * pDlg = (CPopupOption *) pThis;

	//if( nParam == MBR_OK || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
	if( nParam == MBR_OK || pDlg->InputKeyboardEnter())
	{
		pDlg->OptionSave();		
		pDlg->OnClosePopup();
		pDlg->m_bEnableConfirm = FALSE;
	}
	else if( nParam == MBR_CANCEL || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ESC ))
	{
		// 원래의 해상도로 되돌림.
		pDlg->_SetVideo( true);
	}
}
