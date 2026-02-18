#include "pch.h"
#include "UIPhaseLogin_V10.h"

I3_CLASS_INSTANCE( UIPhaseLogin_V10);//, UIPhaseBase);

UIPhaseLogin_V10::UIPhaseLogin_V10()
{

}

UIPhaseLogin_V10::~UIPhaseLogin_V10()
{

}

/*virtual*/ void UIPhaseLogin_V10::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPhaseBase::_InitializeAtLoad( pScene);

	m_join_btn = (i3UIButton*) pScene->FindChildByName( "Btn_Join");
	m_login_btn = (i3UIButton*) pScene->FindChildByName( "Btn_Login");

	m_pIDBox = (i3UIEditBox*) pScene->FindChildByName( "EditBox_ID");
	I3ASSERT( m_pIDBox != nullptr);
	m_pIDBox->setAlphaNumericOnly(true);

#if defined(LOCALE_TURKEY)
	m_pIDBox->SetLetterLimit( 64);
#else
	m_pIDBox->SetLetterLimit( 16);	// NC 웹 기준 4~16
#endif

	m_pPwBox = (i3UIEditBox*) pScene->FindChildByName( "EditBox_PW");
	I3ASSERT( m_pPwBox != nullptr);
	m_pPwBox->setAlphaNumericOnly(true);

	m_pPwBox->SetLetterLimit( 16);

	i3UIFrameWnd* pLoginBackGround = (i3UIFrameWnd *) pScene->FindChildByName( "i3UIFrameWnd_Login");
	I3ASSERT( pLoginBackGround != nullptr);

#if defined(LOCALE_RUSSIA) //마스터랑 러시아랑 파일이 다르다.
	i3Texture * pTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Loading/Login_V10.i3i");
	I3ASSERT(pTexture);
#else
	i3Texture * pTexture;

	if(g_pEnvSet->GetUIVersion() == UFV_1_2) 
		pTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Loading/Login_V12.i3i");
	else
		pTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Loading/Login_V10.i3i");
	I3ASSERT(pTexture);
#endif
	

	m_pBG = i3UIImageBoxEx::new_object();
	I3ASSERT( m_pBG != nullptr);
	m_pBG->AddTexture( pTexture, 1024.f, 768.f);

	I3_MUST_RELEASE(pTexture);

	m_pBG->CreateBaseSceneObjects();
	pLoginBackGround->AddChildToHead(m_pBG);
	m_pBG->SetCurrentImage(0);

	_Resize();

	char szLoadID[MAX_STRING_COUNT] = "";
	i3::snprintf( szLoadID, MAX_STRING_COUNT, "\\%s\\Log.log", GAME_TITLE_NAME_A);
	// ID 로드
	LoadID( szLoadID); 

#if defined(BUILD_RELEASE_QA_VERSION)
	//	char id[ MAX_STRING_COUNT] = {0,};
	//	WCHAR16 pw[ MAX_STRING_COUNT] = L"";

	//	INT32 lengthW = i3::generic_string_size( m_pIDBox->getText());

	i3::string strID;
	i3::utf16_to_mb(m_pIDBox->getText(), strID);
	//	i3String::CopyWCHARtoASCII( id, MAX_STRING_COUNT, m_pIDBox->getText(), lengthW);

	if( i3::contain_string_head( strID, "gptest") )		// if (i3::Contain() == 0) 이렇게 처리하면 너무 멍청한 루틴이다..
	{													// 리턴값이 1~n인것을 찾을 이유가 하나도 없다..그렇게 하지말것..(2013.05.16.수빈)
		m_pPwBox->SetText( m_pIDBox->getText());
	}
	else if( i3::contain_string_head( strID, "angel") )
	{
		m_pPwBox->SetText( L"aaaa1111");
	}
	else if( i3::contain_string_head( strID, "gametest") )
	{
		//	const char * pID = id;
		i3::const_char_range pID = i3::as_literal(strID);

		char szNumber[256];
		char szTemp[256];
		pID.advance_begin(8);	//	pID+=8;

		//	i3::string_ncopy_nullpad( szNumber, pID , lengthW - 8 );
		i3::generic_string_copy(szNumber, pID);

		sprintf_s( szTemp, "g4m3t3st%s", szNumber);

		i3::wstring wstrPW;
		i3::mb_to_utf16(szTemp, wstrPW);
		//	i3String::CopyASCIItoWCHAR( pw, szTemp);
		m_pPwBox->SetText( wstrPW.c_str());
	}
#endif

}