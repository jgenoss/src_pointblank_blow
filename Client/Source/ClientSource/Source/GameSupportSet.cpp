#include "pch.h"
#include "GlobalVariables.h"
#include "AllStage.h"
//#include "PhaseReadyRoom.h"
#include "GameSupportSet.h"
#include "BattleHud.h"
#include "UI/UIMainFrame.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GlobalInfoDef.h"

I3_CLASS_INSTANCE( CGameSupportSet); //, i3ElementBase);

#define LOADING_PROGRESS_POSX		37.0f
#define LOADING_PROGRESS_POSY		740.0f
#define LOADING_PROGRESS_WIDTH		948.0f
#define LOADING_PROGRESS_HEIGHT	10.0f

CGameSupportSet::CGameSupportSet(void)
{
	m_pRoot				= nullptr;
	m_pSpriteTraining	= nullptr;
	m_pTextureTraining  = nullptr;
	m_pSpriteBG			= nullptr;
	m_pTextureBG		= nullptr;
	m_pSpriteProgress	= nullptr;
	m_rLoadingPercent	= 0.0f;

	m_pTextTip			= nullptr;
}

CGameSupportSet::~CGameSupportSet(void)
{
	I3_SAFE_RELEASE(m_pTextTip);

	I3_SAFE_RELEASE( m_pRoot);
	I3_SAFE_RELEASE( m_pSpriteTraining);
	I3_SAFE_RELEASE( m_pTextureTraining);
	I3_SAFE_RELEASE( m_pSpriteBG);
	I3_SAFE_RELEASE( m_pTextureBG);
	I3_SAFE_RELEASE( m_pSpriteProgress);
}

void CGameSupportSet::_SetLoadingBG(REAL32 fScreenWidthRate, REAL32 fScreenHeightRate)
{
	// 스테이지에 따라서 배경을 로딩합니다.
#if defined(UI2_RESOLUTION_FIX)
	i3Viewer * pViewer = g_pFramework->GetViewer();
//	i3VideoInfo* pVideo = pViewer->GetRenderContext()->GetVideoInfo();

	i3VideoInfo Video;

	i3VideoInfo* pOrigVideo = pViewer->GetRenderContext()->GetVideoInfo();
	Video = *pOrigVideo;
	i3VideoInfo* pVideo = &Video;

	pVideo->m_bFullScreen			= g_pEnvSet->m_bFullScreen;
	pVideo->m_Width					= g_pEnvSet->m_nScreenWidth;
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

	pViewer->SetVideoInfo( pVideo);

	g_pFramework->SetVideoCaptureInfo(g_pEnvSet->m_nVideoResolution);

#endif

	char conv[MAX_PATH];
	const char * pszStageName = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex())->GetName();
	if( pszStageName != nullptr)
	{
		char szTeam[ 32];
		if( !(BattleSlotContext::i()->getMySlotIdx()%2))/*Red팀*/
			i3::safe_string_copy( szTeam, "Red", sizeof( szTeam));
		else
			i3::safe_string_copy( szTeam, "Blue", sizeof( szTeam));

		if(  MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DEATHMATCH))
		{
			i3::sprintf( conv, "World/%s/Images/load_%s_DM_%s.i3i", pszStageName, pszStageName, szTeam);

			if( i3FileStream::isFile( conv) == false)
			{
				i3::sprintf( conv, "World/%s/Images/load_%s_%s.i3i", pszStageName, pszStageName, szTeam);

				if( i3FileStream::isFile( conv) == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "not exist file %s!!", conv);
					return;
				}
			}
		}
		else
		{
			i3::sprintf( conv, "World/%s/Images/load_%s_%s.i3i", pszStageName, pszStageName, szTeam);

			if( i3FileStream::isFile( conv) == false)
			{
				I3PRINTLOG(I3LOG_FATAL,  "no exist file %s!!", conv);
				return;
			}
		}

		m_pTextureBG = g_pFramework->GetResourceManager()->LoadTexture( conv);

		if( m_pTextureBG != nullptr)
		{
			if( m_pSpriteBG == nullptr)
			{
				m_pSpriteBG = i3Sprite2D::new_object();
				m_pSpriteBG->Create( 1, true, true);
			}

			m_pSpriteBG->SetTexture( m_pTextureBG );
			m_pSpriteBG->SetEnable( 0, true);
			m_pSpriteBG->SetRect( 0, 0.0f, 0.0f, 1024.0f * fScreenWidthRate, 768.0f * fScreenHeightRate);
			m_pSpriteBG->SetColor( 0, 255,255,255,255);
			m_pSpriteBG->SetTextureCoord( 0, 0.0f, 0.0f, 1024.0f, 768.0f);
		}
	}
}

void CGameSupportSet::_SetSubLoadingImg(REAL32 fScreenWidthRate, REAL32 fScreenHeightRate)
{
	// 훈련전 / CC모드 로딩 이미지
	bool bIsCrossCountMode = MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT);

#if defined (MULTYLANGUAGE)
	i3::stack_string strLoadFilePath;

	if( bIsCrossCountMode)	
		i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Loading/Load_CCmode.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );
	else
		i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Loading/Load_Training.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );

	m_pTextureTraining = g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
#else
#if defined(USE_LOCALE_UI)

	char szPath[MAX_PATH];

	if( bIsCrossCountMode)	
		i3::snprintf(szPath, sizeof(szPath), "%s/Image/Loading/Load_CCmode.i3i", i3UI::getManager()->GetCommonLocaleUIFolder()->c_str());
	else					
		i3::snprintf(szPath, sizeof(szPath), "%s/Image/Loading/Load_Training.i3i", i3UI::getManager()->GetCommonLocaleUIFolder()->c_str());

	m_pTextureTraining = g_pFramework->GetResourceManager()->LoadTexture(szPath); 

#else
	if( bIsCrossCountMode)	{ m_pTextureTraining = g_pFramework->GetResourceManager()->LoadTexture("Gui/Loading/Load_CCmode.i3i"); }
	else					{ m_pTextureTraining = g_pFramework->GetResourceManager()->LoadTexture("Gui/Loading/Load_Training.i3i"); }
#endif
#endif
	
	I3ASSERT(m_pTextureTraining != nullptr);

	if (m_pTextureTraining != nullptr)
	{
		if( m_pSpriteTraining == nullptr)
		{
			m_pSpriteTraining = i3Sprite2D::new_object();
			m_pSpriteTraining->Create(2, true, false);
		}

		m_pSpriteTraining->SetTexture(m_pTextureTraining);
		m_pSpriteTraining->SetEnable(false);

		const char * pszStageName = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex() )->GetName();
		if( pszStageName != nullptr)
		{
			if (MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
			{
				m_pSpriteTraining->SetEnable(true);
			}
		}

		// 훈련전 & CCMode 텍스트
		m_pSpriteTraining->SetTextureCoord(0, 0.0f, 0.0f, 333.0f, 548.0f);
		m_pSpriteTraining->SetRect(0, 14.0f * fScreenWidthRate, 151.0f * fScreenHeightRate, 333.0f * fScreenWidthRate, 548.0f * fScreenHeightRate);

		// 훈련전 & CCMode 팀마크
#if defined( DEF_OBSERVER_MODE)
		if( BattleSlotContext::i()->isObserverMe() || (0 == (BattleSlotContext::i()->getMySlotIdx() %2) )
#else
		if (0 == (BattleSlotContext::i()->getMySlotIdx() % 2) )
#endif
		{
			m_pSpriteTraining->SetTextureCoord(1, 0.0f, 550.0f, 160.0f, 686.0f);
		}
		else
		{
			m_pSpriteTraining->SetTextureCoord(1, 173.0f, 550.0f, 333.0f, 686.0f);
		}

		m_pSpriteTraining->SetRect(1, 102.0f * fScreenWidthRate, 134.0f * fScreenHeightRate, 160.0f * fScreenWidthRate, 136.0f * fScreenHeightRate);
	}

}

void CGameSupportSet::_SetLoadingTip(REAL32 fScreenWidthRate, REAL32 fScreenHeightRate)
{
	if (nullptr == m_pTextTip) {
		m_pTextTip = i3TextNodeDX2::new_object();
		m_pTextTip->Create(  GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_SIZE_LOADING_TIP, 0, false, false, DEFAULT_QUALITY, true);
		m_pTextTip->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextTip->setAutoWrapEnable( false);


		m_pTextTip->setSize( (UINT32)(1024*fScreenWidthRate), (UINT32)(80*fScreenHeightRate) );
		m_pTextTip->SetColor( 255, 255, 255, 255);
	}

	char szTemp[256];
	INT32 iRand = i3Math::Rand() % 13;
	if (iRand > 3) {
		_snprintf(szTemp, 256, "STBL_IDX_TIP_LOADING_SINGLE_%d", iRand - 4);
		m_pTextTip->setPos((UINT32)(115 * fScreenWidthRate), (UINT32)(707 * fScreenHeightRate));
	}
	else {
		_snprintf(szTemp, 256, "STBL_IDX_TIP_LOADING_DUAL_%d", iRand);
		m_pTextTip->setPos((UINT32)(115 * fScreenWidthRate), (UINT32)(702 * fScreenHeightRate));
	}
	
	if( GAME_RCSTRING(szTemp).size() != 0 ) 
	{
		m_pTextTip->SetText( GAME_RCSTRING(szTemp) );
		m_pTextTip->SetEnable( true );
	}
}

void CGameSupportSet::_SetLoadingProgress(REAL32 fScreenWidthRate, REAL32 fScreenHeightRate)
{
	if(m_pSpriteProgress == nullptr)
	{
		m_pSpriteProgress = i3Sprite2D::new_object();
		m_pSpriteProgress->Create( 1, false, true, false);
	}

	m_rLoadingPercent = -1.0f;

	REAL32 rPosX = LOADING_PROGRESS_POSX * fScreenWidthRate;
	REAL32 rPosY = LOADING_PROGRESS_POSY * fScreenHeightRate;
	REAL32 rHeight = LOADING_PROGRESS_HEIGHT * fScreenHeightRate;	

	m_pSpriteProgress->SetEnable( 0, true);
	m_pSpriteProgress->SetRect( 0, rPosX, rPosY, 0.0f, rHeight);
	m_pSpriteProgress->SetColor( 0, 255, 128, 0, 255);
}

void CGameSupportSet::SetLoadingSpr(void)
{
	REAL32 fScreenWidthRate =  g_pEnvSet->m_nScreenWidth / 1024.0f;
	REAL32 fScreenHeightRate =  g_pEnvSet->m_nScreenHeight / 768.0f;

	if(g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex()) != nullptr && g_pFramework->IsBattleStage())
	{
		GLOBAL_CHARA_INFO * pCharaInfo = g_pCharaManager->setCharaInfo( BattleSlotContext::i()->getMySlotIdx());
		g_pCharaManager->MakeCharaTeamInfo( pCharaInfo, BattleSlotContext::i()->getMySlotIdx(), CHARA::TYPE_PLAYER);

		_SetLoadingBG(fScreenWidthRate, fScreenHeightRate);

		_SetSubLoadingImg(fScreenWidthRate, fScreenHeightRate);

		_SetLoadingTip(fScreenWidthRate, fScreenHeightRate);

		_SetLoadingProgress(fScreenWidthRate, fScreenHeightRate);

		if( m_pRoot == nullptr)
		{
			m_pRoot = i3AttrSet::new_object();
			I3ASSERT( m_pRoot != nullptr);

			if( m_pSpriteBG != nullptr)		m_pRoot->AddChild( m_pSpriteBG);
			if( m_pSpriteTraining != nullptr)	m_pRoot->AddChild( m_pSpriteTraining);
			if( m_pTextTip != nullptr)			m_pRoot->AddChild( m_pTextTip);
			if( m_pSpriteProgress != nullptr)	m_pRoot->AddChild( m_pSpriteProgress);

			{
				i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();

				pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);
				pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);

				m_pRoot->AddAttr( pAttr);
			}

			g_pFramework->getGuiLayer()->AddChild( m_pRoot);
		}

		m_pRoot->RemoveFlag( I3_NODEFLAG_DISABLE);
	}
	else
	{
		if(m_pSpriteBG != nullptr)
			m_pSpriteBG->SetEnable(false);

		if(m_pSpriteTraining != nullptr)
			m_pSpriteTraining->SetEnable(false);

		if(m_pTextTip != nullptr)
			m_pTextTip->SetEnable(false);

		if(m_pSpriteProgress != nullptr)
			m_pSpriteProgress->SetEnable(false);
	}
}

void CGameSupportSet::UpdateLoadingProgress( REAL32 rDeltaSeconds)
{
	if(g_pFramework->IsBattleStage() && m_rLoadingPercent >= 0.0f)
	{
		REAL32 fScreenWidthRate =  g_pEnvSet->m_nScreenWidth / 1024.0f;
		REAL32 fScreenHeightRate =  g_pEnvSet->m_nScreenHeight / 768.0f;

		m_rLoadingPercent += (rDeltaSeconds * 0.333f);

		//I3TRACE("m_rLoadingPercent = %f\n", m_rLoadingPercent);

		if(m_rLoadingPercent >= 0.99f)
			m_rLoadingPercent = 0.99f;

		REAL32 rPosX = LOADING_PROGRESS_POSX * fScreenWidthRate;
		REAL32 rPosY = LOADING_PROGRESS_POSY * fScreenHeightRate;
		REAL32 rHeight = LOADING_PROGRESS_HEIGHT * fScreenHeightRate;	

		REAL32 rWidth = LOADING_PROGRESS_WIDTH * m_rLoadingPercent;

		//I3TRACE("CGameSupportSet::UpdateLoadingProgress()\n");

		m_pSpriteProgress->SetRect(0, rPosX, rPosY, rWidth * fScreenWidthRate, rHeight );
	}
}

void CGameSupportSet::ReleaseLoadingSpr(void)
{
	if(m_pSpriteBG != nullptr)
	{
		m_pSpriteBG->SetTexture( (i3Texture*)nullptr);
	}
	I3_SAFE_RELEASE( m_pTextureBG);

	if (m_pSpriteTraining != nullptr)
	{
		m_pSpriteTraining->SetTexture((i3Texture*)nullptr);
	}
	I3_SAFE_RELEASE(m_pTextureTraining);

	if(m_pRoot != nullptr)
	{
		m_pRoot->AddFlag( I3_NODEFLAG_DISABLE);
	}
}

void CGameSupportSet::SaveScreenShot(void)
{
	wchar_t szCurDir[ MAX_PATH ] = L"";

	SHGetSpecialFolderPathW(nullptr, szCurDir, CSIDL_PERSONAL, false);

	//document 위치
	std::wstring doc_path = szCurDir;

	//document/PointBlank 위치
	std::wstring doc_pb_path = doc_path + L"\\" + GAME_TITLE_NAME_W;
	FileUtil::create_directory( doc_pb_path );

	//document/PointBlank/ScreenShot 위치
	i3::wstring doc_pb_screenshot_path = doc_pb_path +  L"\\" + L"ScreenShot";
	FileUtil::create_directory( doc_pb_screenshot_path );

	i3::wstring screenshot_file;
	screenshot_file.append( GAME_TITLE_NAME_W );
	screenshot_file.append( L"_" );
	screenshot_file.append( FileUtil::wstring_by_date(L"") + L"_" + FileUtil::wstring_by_time(L"") );

	i3::wstring doc_pb_screenshot_file_path;
	doc_pb_screenshot_file_path = doc_pb_screenshot_path + L"\\" + screenshot_file;

	if( true == LocaleValue::Enable("EnableVideoCapture") )
	{
		doc_pb_screenshot_file_path += L".jpg";
		g_pFramework->SaveImageCapture( doc_pb_screenshot_file_path.c_str() );
	}
	else
	{
		g_pViewer->GetRenderContext()->SaveScreenShotToFileW( doc_pb_screenshot_file_path.c_str(), "jpg");
	}

	// 게임 스테이지 안에서는 스크린샷 저장 메시지를 나타냅니다.
	{
		i3Stage* pStage = g_pFramework->GetCurrentStage();

		if( pStage != nullptr)
		{
			if (g_pFramework->IsBattleStage())
			{
				if ( CHud::i())
					CHud::i()->CreateInGameMessage(GAME_RCSTRING("STBL_IDX_SCREENSHOT"));
			}
			else if( g_pFramework->IsUIStage())
			{
				UIMainFrame* pUIMainFrame = g_pFramework->GetUIMainframe();

				// 로딩 중 UIMainFrame이 없을 경우 시스템 메시지를 출력하지 않습니다 
				if (pUIMainFrame == nullptr)
					return;

				if( pUIMainFrame->GetCurrentPhaseType() == UIPHASE_READY_ROOM ||
					pUIMainFrame->GetCurrentPhaseType() == UIPHASE_CLANLOBBY ||
					pUIMainFrame->GetCurrentPhaseType() == UIPHASE_LOBBY)
				{
					g_pFramework->getChatBox()->PutSystemChatting( GAME_RCSTRING("STBL_IDX_SCREENSHOT"));
				}
			}
		}
	}

	GlobalFunc::PB_TRACE( "save Screenshot.. %.4f", g_pViewer->getDeltaTime() );

}

void CGameSupportSet::SaveAllFontTexture(void)
{
	//	폰트 매니저에서 사용중인 모든 폰트의 텍스처를 이미지 파일로 저장한다.
	i3GfxFontDX2Manager * pManager = g_pFramework->GetViewer()->GetRenderContext()->getFontDx2Manager();

	char temp[256] = "";
	for(INT32 i=0; i<pManager->getFontCount(); i++)
	{
		i3GfxFontDX2 * pFont = pManager->getFont(i);

		for(int cnt=0; cnt<pFont->getTextureCount(); cnt++)
		{
			i3Texture * pTex = pFont->getTexture(cnt);

			IDirect3DSurface9* pBackBuffer = pTex->getSurface(0)->getSurface();

			sprintf_s( temp, "FontTex%d-%d.jpg", i, cnt);
			D3DXSaveSurfaceToFile( temp, D3DXIFF_JPG, pBackBuffer, nullptr, nullptr );
		}
	}
}