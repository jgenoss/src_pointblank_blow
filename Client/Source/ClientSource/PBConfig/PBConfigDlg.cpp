// PBConfigDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "PBConfig.h"
#include "PBConfigDlg.h"
#include "i3Base/string/ext/extract_directoryname.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPBConfigDlg 대화 상자



CPBConfigDlg::CPBConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPBConfigDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pCtx = nullptr;

	m_nLanguageID		= 0;
	m_nUIVersion		= 0;

	m_bFullScreen		= true;
	m_ScreenWidth		= 1024;
	m_ScreenHeight		= 768;
	m_ScreenDepth		= 32;
	m_ScreenHz			= 60;
	m_bVSync			= false;
	m_bTriLinearFilter	= false;
	m_b16BitsDepth		= false;
	m_bDynamicLight		= false;
	m_bNormalMap		= false;
	m_bSpecularMap		= false;
	m_TexQuality		= 1;
	m_ShadowQuality		= 0;
	m_AA				= 0;
	m_AAQ				= 0;
	m_Gamma				= 50.0f;
	m_SpeakerType		= 0;
	m_bBulletTrailer	= true;
	m_bTerrainEffect	= true;

	m_InterFace =0;
	m_TeamBand =1;
	m_Parts = 1;
	m_Chara = 1;

	m_EnvFileName = ENVFILENAME;
}

void CPBConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CB_RES, m_ResListCtrl);
	DDX_Control(pDX, IDC_CB_COLOR, m_ColorBitsCtrl);
	DDX_Control(pDX, IDC_CHK_DYNAMICLIGHT, m_DynamicLightCtrl);
	DDX_Control(pDX, IDC_CHK_NORMALMAP, m_NormalMapCtrl);
	DDX_Control(pDX, IDC_CHK_SPECULARMAP, m_SpecularMapCtrl);
	DDX_Control(pDX, IDC_CHK_TRIFILTER, m_TriLinearCtrl);
	DDX_Control(pDX, IDC_CHK_VSYNC, m_VSyncCtrl);
	DDX_Control(pDX, IDC_CB_TEXTURE, m_TexQualityCtrl);
	DDX_Control(pDX, IDC_CB_SHADOW, m_ShadowQualityCtrl);
	DDX_Control(pDX, IDC_CHK_WINDOWMODE, m_CHK_WindowMode);

	DDX_Control(pDX, IDC_CHK_TEAM_BAND, m_Btn_TeamBand);

	DDX_Control(pDX, IDC_RADIO_INTERFACE_NEW, m_Btn_InterFace[0]);
	DDX_Control(pDX, IDC_RADIO_INTERFACE_CLASSIC, m_Btn_InterFace[1]);

	DDX_Control(pDX, IDC_RADIO_PARTS_P0, m_Btn_Parts[0]);
	DDX_Control(pDX, IDC_RADIO_PARTS_P1, m_Btn_Parts[1]);

	DDX_Control(pDX, IDC_RADIO_CHARA_P0, m_Btn_Chara[0]);
	DDX_Control(pDX, IDC_RADIO_CHARA_P1, m_Btn_Chara[1]);

	DDX_Control(pDX, IDC_CB_UIVERSION, m_UIVersion);
}

BEGIN_MESSAGE_MAP(CPBConfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CPBConfigDlg 메시지 처리기

void CPBConfigDlg::_SetWindowText(void)
{
	char szTitle[MAX_STRING_COUNT] = "";
	i3::snprintf( szTitle, MAX_STRING_COUNT, "%s Configuration", GAME_APP_NAME_A);

	this->SetWindowText( szTitle);
}

void CPBConfigDlg::_UpdateResList( I3G_IMAGE_FORMAT fmt)
{
	// 해상도를 나열한다.
	INT32 cnt;
	I3G_DISPLAY_MODE_INFO * pInfo;
	char conv[256];

	cnt = i3RenderContext::getDisplayModeCount( fmt);

	_RemoveAllResList();

	INT32 src_width = ::GetSystemMetrics( SM_CXSCREEN);
	INT32 src_height = ::GetSystemMetrics( SM_CYSCREEN);
	INT32 max_src_width = src_width, max_src_height = src_height;

	for( INT32 i = 0; i < cnt; i++)
	{
		pInfo = (I3G_DISPLAY_MODE_INFO *) i3MemAlloc( sizeof(I3G_DISPLAY_MODE_INFO));
		I3ASSERT( pInfo != nullptr);

		i3RenderContext::getDisplayMode( i, fmt, pInfo);

		if( (pInfo->m_Width * pInfo->m_Height) > ( max_src_width * max_src_height) ) //픽셀 수가 많으면 더 큰 해상도라고 판단함.
		{
			max_src_width = pInfo->m_Width;
			max_src_height = pInfo->m_Height;
		}

		i3MemFree( pInfo);
	}

	REAL32 ratioMonitor = (REAL32) max_src_width / max_src_height;

	for(INT32 i = 0; i < cnt; i++)
	{
		pInfo = (I3G_DISPLAY_MODE_INFO *) i3MemAlloc( sizeof(I3G_DISPLAY_MODE_INFO));
		I3ASSERT( pInfo != nullptr);

		i3RenderContext::getDisplayMode( i, fmt, pInfo);

		REAL32 ratio = (REAL32) pInfo->m_Width / pInfo->m_Height;

		bool bAdd = false;

		if( ratioMonitor > 1.5f || ( (ratioMonitor < 1.5f) && (ratio < 1.5f) ) )
			bAdd = true;

		// + 40hz 이하 삭제.
//		if ((pInfo->m_RefreshRate <= 40.0f))
//			bAdd = false;

		if( !bAdd || (pInfo->m_Width < 1024) || (pInfo->m_Height < 768) ) // || (INT32) pInfo->m_RefreshRate != 60)
		{
			i3MemFree( pInfo);
			continue;
		}

		sprintf_s( conv, "%d x %d  (%d Hz)", pInfo->m_Width, pInfo->m_Height, (INT32) pInfo->m_RefreshRate);

		INT32 idx = m_ResListCtrl.AddString( conv);

		m_ResListCtrl.SetItemData( idx, (DWORD_PTR) pInfo);
	}
}

void CPBConfigDlg::_RemoveAllResList(void)
{
	INT32 i;
	I3G_DISPLAY_MODE_INFO * pInfo;
	
	for( i = 0; i < m_ResListCtrl.GetCount(); i++)
	{
		pInfo = (I3G_DISPLAY_MODE_INFO *) m_ResListCtrl.GetItemData( i);

		i3MemFree( pInfo);
	}

	m_ResListCtrl.ResetContent();
}

BOOL CPBConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	
	// 게임 타이틀에 따라 다이얼로그 타이틀이 바뀐다.
	_SetWindowText();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// revision 57265 PBConfig Save 할 때 DefaultSHGL 파일을 Cache.i3GL 로 복사하는 기능 완료
	i3ShaderCache *	pDef = nullptr;
	i3ShaderCache *	pCur = nullptr;

	pDef = i3ShaderCache::Load( "Shader/DefaultSHGL");
	pCur = i3ShaderCache::Load( "Shader/Cache.i3GL");

	if( pDef == nullptr)
	{
	}
	else if( (pCur == nullptr) ||
		(pDef->getISSVersion() > pCur->getISSVersion()))
	{
		//	I3PRINTLOG(I3LOG_NOTICE, " - Shader Cache 파일이 덮어 씁니다.");

		::CopyFile( "Shader/DefaultSHGL", "Shader/Cache.i3GL", FALSE);
	}

	{
		m_pCtx = i3RenderContext::new_object(); //NewObject();

		i3VideoInfo info;

		info.m_Width = 100;
		info.m_Height = 100;
		info.m_bFullScreen = false;
		

		if( m_pCtx->Create( &info, m_hWnd) == false)
		{
			AfxMessageBox( "DirectX를 초기화할 수 없습니다. DirectX를 재설치해 보시기 바랍니다.");
			return FALSE;
		}
	}

	LoadEnv();
	SetControlValue();

	if(m_nLanguageID == 0)
	{
		m_nLanguageID = ::GetSystemDefaultLangID();
	}

	GetDlgItem(IDC_CHK_TEAM_BAND)->EnableWindow(FALSE);

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CPBConfigDlg::OnDestroy()
{
	I3_SAFE_RELEASE( m_pCtx);

	CDialog::OnDestroy();
}


// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CPBConfigDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CPBConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPBConfigDlg::OnBnClickedOk()
{
	GetControlValue();
	SaveEnv();

	OnOK();
}



namespace 
{
	void SetCheckArrayButton(CButton* btn, size_t sizeofBtn, size_t checked_btn_idx)
	{
		for(size_t i=0;i<sizeofBtn;++i)
		{
			btn[i].SetCheck( i == checked_btn_idx );
		}
	}

	size_t GetCheckArrayButton(CButton* btn, size_t sizeofBtn)
	{
		for(size_t i=0;i<sizeofBtn;++i)
		{
			if( btn[i].GetCheck() == TRUE)
				return i;
		}
		return (size_t)-1;
	}
}

void CPBConfigDlg::LoadEnv()
{
	i3RegistrySet* pReg = i3RegistrySet::new_object();

	if( pReg->LoadRegistrySet( m_EnvFileName ) != STREAM_ERR)
	{
		i3RegKey* pRoot = pReg->getRoot();

		INT32 nTemp = 0;
		FIND_REG_DATA(pRoot, "Language", &nTemp);
		m_nLanguageID = static_cast<LANGID>(nTemp);
		pRoot->GetData("UI_Version", &m_nUIVersion);


		pRoot->GetData(	"Full_Screen",			&m_bFullScreen);
		pRoot->GetData( "Screen_Width",			&m_ScreenWidth);
		pRoot->GetData( "Screen_Height",			&m_ScreenHeight);
		pRoot->GetData( "Screen_Hz",				&m_ScreenHz);
		pRoot->GetData( "Screen_Depth",			&m_ScreenDepth);
		pRoot->GetData( "AntiAliasing",			&m_AA);		
		pRoot->GetData( "AntiAliasingQ",			&m_AAQ);
		pRoot->GetData( "VSync",					&m_bVSync);
		pRoot->GetData( "TriLinearFilter",		&m_bTriLinearFilter);
		pRoot->GetData("Screen_Gamma_Value",		&m_Gamma);

		pRoot->GetData( "Speaker_Type",			&m_SpeakerType);
		pRoot->GetData( "Enable_Bullet_Trace",	&m_bBulletTrailer);
		pRoot->GetData( "Enable_DynamicLight",	&m_bDynamicLight);
		pRoot->GetData( "Enable_NormalMap",		&m_bNormalMap);
		pRoot->GetData( "Enable_SpecularMap",	&m_bSpecularMap);
		pRoot->GetData( "Enable_TerrainEffect",	&m_bTerrainEffect);
		pRoot->GetData( "Shadow_Quality_Type",	&m_ShadowQuality);
		pRoot->GetData( "Tex_Quality_Type",		&m_TexQuality);
		pRoot->GetData( "16BitsDepth",			&m_b16BitsDepth);

		pRoot->GetData( "GameInterface",			&m_InterFace);
		pRoot->GetData( "Enable_TeamBand",			&m_TeamBand);
		pRoot->GetData( "Display_Parts",			&m_Parts);
		pRoot->GetData( "Display_Chara",			&m_Chara);
	}

	I3_MUST_RELEASE(pReg);
}
bool FindDirectory(char *path, char* directoryName)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	BOOL bResult = TRUE;
	char drive[_MAX_DRIVE] = { 0, };
	char dir[MAX_PATH] = { 0, };
	char newpath[MAX_PATH] = { 0, };

	hSrch = FindFirstFile(path, &wfd);

	if (hSrch == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	_splitpath(path, drive, dir, nullptr, nullptr);

	while (bResult)
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // 폴더형식이라면.
		{
			if (strcmp(wfd.cFileName, ".") && strcmp(wfd.cFileName, "..")) // 이 2개는 제외한다.
			{
				if (strcmp(wfd.cFileName, directoryName) == 0)
				{
					::FindClose(hSrch);
					return true;
				}

				sprintf(newpath, "%s%s%s\\*.*", drive, dir, wfd.cFileName);
				
				if (FindDirectory(newpath, directoryName))
				{
					::FindClose(hSrch);
					return true;
				}
			}
		}
		bResult = ::FindNextFile(hSrch, &wfd);
	}
	FindClose(hSrch);

	return false;
}

void CPBConfigDlg::SetControlValue()
{
	I3G_IMAGE_FORMAT fmt = I3G_IMAGE_FORMAT_BGRX_8888;

	if( m_ScreenDepth == 16)
		fmt = I3G_IMAGE_FORMAT_BGR_565;

	// 해상도
	_UpdateResList( fmt);

	// 현재 해상도 선택
	{
		INT32 i;

		for( i = 0; i < m_ResListCtrl.GetCount(); i++)
		{
			I3G_DISPLAY_MODE_INFO * pInfo = (I3G_DISPLAY_MODE_INFO *) m_ResListCtrl.GetItemData( i);

			if(	(pInfo->m_Width == m_ScreenWidth) &&
				(pInfo->m_Height == m_ScreenHeight) &&
				(pInfo->m_RefreshRate == m_ScreenHz))
			{
				m_ResListCtrl.SetCurSel( i);
			}
		}
	}

	//UI 버젼 선택
	char OldPath[MAX_PATH];
	GetCurrentDirectory(sizeof(OldPath) - 1, OldPath);
	
	i3::string strV10, strV11, strV15, strV12, strLocale;
	
	bool bUsed_V10 = false, bUsed_V12 = false, bUsed_V11 = false, bUsed_V15 = false;
	INT32 idx = 0;

	strV10 = OldPath;
	i3::generic_append_range(strV10, "\\UI_V10");

	strV12 = OldPath;
	i3::generic_append_range(strV12, "\\UI_V12");

	strV11 = OldPath;
	i3::generic_append_range(strV11, "\\UI_V11");

	strV15 = OldPath;
	i3::generic_append_range(strV15, "\\UI_V15");

	strLocale = OldPath;
	i3::generic_append_range(strLocale, "\\Locale");

	//@eunil. 북미 1.1 마이그레이션용으로 다시 살립니다. 16.12.08 
	//북미 패치 생성 이후 다시 주석처리 16. 12. 09
	//기존 Release와 충돌방지를 위해 마이그레이션용 코드 추가합니다. 17. 01. 04 마이그레이션 완료 후 제거예정
	
	i3::string strNorthAmerica; //북미 마이그용 코드
	strNorthAmerica = strLocale;
	i3::generic_append_range(strNorthAmerica, "\\NorthAmerica");

	 //필리핀 예외처리 이 것은 강제적으로 바꿉니다.
	 //define이 없어....(jinsik.kim)
	 //고민이 필요해 보입니다.
	 //필리핀이 적용되었기 때문에 다시 돌립니다.
	if (FindDirectory((char*)strNorthAmerica.c_str(), "UI_V11"))
	{
		m_nUIVersion = 1;

		m_UIVersion.AddString("1.1");
		m_UIVersion.SetItemData(idx, 1);
		bUsed_V11 = true;
		idx++;
	}
	else
	{
		if (i3System::IsDirectory(strV15.c_str()) || FindDirectory((char*)strLocale.c_str(), "UI_V15"))
		{
			m_UIVersion.AddString("1.5");
			m_UIVersion.SetItemData(idx, 0);
			bUsed_V15 = true;
			idx++;
		}

		if (i3System::IsDirectory(strV11.c_str()) || FindDirectory((char*)strLocale.c_str(), "UI_V11"))
		{
			m_UIVersion.AddString("1.1");
			m_UIVersion.SetItemData(idx, 1);
			bUsed_V11 = true;
			idx++;
		}

		if (i3System::IsDirectory(strV10.c_str()) || FindDirectory((char*)strLocale.c_str(), "UI_V10"))
		{
			m_UIVersion.AddString("1.0");
			m_UIVersion.SetItemData(idx, 2);
			bUsed_V10 = true;
			idx++;
		}

		if (i3System::IsDirectory(strV10.c_str()) || FindDirectory((char*)strLocale.c_str(), "UI_V12"))
		{
			m_UIVersion.AddString("1.2");
			m_UIVersion.SetItemData(idx, 3);
			bUsed_V12 = true;
			idx++;
		}
	}

	if (m_UIVersion.GetCount() > 0)
	{
		// 인덱스에 맞춰 유동적인 셀을 선택해야함...
		INT32 i;
		for (i = 0; i < m_UIVersion.GetCount(); i++)
		{
			if ((INT32)m_UIVersion.GetItemData(i) == m_nUIVersion)
			{
				m_UIVersion.SetCurSel(i);
			}
		}
	}
	else
	{
		m_UIVersion.SetCurSel(0);
	}

	// 색상
	if( m_ScreenDepth != 16)
		m_ColorBitsCtrl.SetCurSel( 0);
	else
		m_ColorBitsCtrl.SetCurSel( 1);

	if( m_bFullScreen)
		m_CHK_WindowMode.SetCheck( BST_UNCHECKED);
	else
		m_CHK_WindowMode.SetCheck( BST_CHECKED);

	// Dynamic Light
	if( m_bDynamicLight)
		m_DynamicLightCtrl.SetCheck( BST_CHECKED);
	else
		m_DynamicLightCtrl.SetCheck( BST_UNCHECKED);

	// Normal Map
	if( m_bNormalMap)
		m_NormalMapCtrl.SetCheck( BST_CHECKED);
	else
		m_NormalMapCtrl.SetCheck( BST_UNCHECKED);

	// Specular Map
	if( m_bSpecularMap)
		m_SpecularMapCtrl.SetCheck( BST_CHECKED);
	else
		m_SpecularMapCtrl.SetCheck( BST_UNCHECKED);

	// Tri-Linear Filter
	if( m_bTriLinearFilter)
		m_TriLinearCtrl.SetCheck( BST_CHECKED);
	else
		m_TriLinearCtrl.SetCheck( BST_UNCHECKED);

	// Vertical-Sync
	if( m_bVSync)
		m_VSyncCtrl.SetCheck( BST_CHECKED);
	else
		m_VSyncCtrl.SetCheck( BST_UNCHECKED);

	// Texture Quality
	m_TexQualityCtrl.SetCurSel( m_TexQuality);

	// Shadow Quality
	m_ShadowQualityCtrl.SetCurSel( m_ShadowQuality);

	m_Btn_TeamBand.SetCheck( m_TeamBand );

	SetCheckArrayButton(m_Btn_InterFace, 2, m_InterFace );
	SetCheckArrayButton(m_Btn_Parts, 2, m_Parts );
	SetCheckArrayButton(m_Btn_Chara, 2, m_Chara );
}


void CPBConfigDlg::GetControlValue()
{
	// 해상도
	INT32 idx = m_ResListCtrl.GetCurSel();

	if( idx != -1)
	{
		I3G_DISPLAY_MODE_INFO * pInfo = (I3G_DISPLAY_MODE_INFO *) m_ResListCtrl.GetItemData( idx);

		m_ScreenWidth		= pInfo->m_Width;
		m_ScreenHeight		= pInfo->m_Height;
		m_ScreenHz			= (INT32) pInfo->m_RefreshRate;
	}

	//UI 버젼 선택
	m_nUIVersion = m_UIVersion.GetItemData(m_UIVersion.GetCurSel());
	if (m_nUIVersion < 0)		m_nUIVersion = 0;

	// 색상
	if( m_ColorBitsCtrl.GetCurSel() != 0)
		m_ScreenDepth = 16;
	else
		m_ScreenDepth = 32;

	if( m_CHK_WindowMode.GetCheck() == BST_CHECKED)
		m_bFullScreen = false;
	else
		m_bFullScreen = true;

	// Dynamic Light
	if( m_DynamicLightCtrl.GetCheck() == BST_CHECKED)
		m_bDynamicLight = true;
	else
		m_bDynamicLight = false;

	// normal Map
	if( m_NormalMapCtrl.GetCheck() == BST_CHECKED)
		m_bNormalMap = true;
	else
		m_bNormalMap = false;

	// Specular Map
	if( m_SpecularMapCtrl.GetCheck() == BST_CHECKED)
		m_bSpecularMap = true;
	else
		m_bSpecularMap = false;

	// Tri-Linear Filter
	if( m_TriLinearCtrl.GetCheck() == BST_CHECKED)
		m_bTriLinearFilter = true;
	else
		m_bTriLinearFilter = false;

	// Vsync
	// Tri-Linear Filter
	if( m_VSyncCtrl.GetCheck() == BST_CHECKED)
		m_bVSync = true;
	else
		m_bVSync = false;

	// Texture Quality
	m_TexQuality = m_TexQualityCtrl.GetCurSel();
	if( m_TexQuality < 0)			m_TexQuality = 1;

	// Shadow Quality
	m_ShadowQuality = m_ShadowQualityCtrl.GetCurSel();
	if( m_ShadowQuality < 0)		m_ShadowQuality = 0;

	m_TeamBand = m_Btn_TeamBand.GetCheck();
	m_InterFace = GetCheckArrayButton(m_Btn_InterFace, 2);
	m_Parts = GetCheckArrayButton(m_Btn_Parts, 2);
	m_Chara = GetCheckArrayButton(m_Btn_Chara, 2);
}

void CPBConfigDlg::SaveEnv()
{
	CFileFind finder;
	char szCurDir[MAX_STRING_COUNT] = { 0, };
	char szSaveDirectory[MAX_STRING_COUNT] = { 0, };

	// 만약 세이브할 디렉토리가 없다면 디렉토리 생성합니다.
	::GetCurrentDirectory(MAX_STRING_COUNT, szCurDir);
	i3::safe_string_copy(szSaveDirectory, szCurDir, MAX_STRING_COUNT);
	i3::generic_string_cat(szSaveDirectory, "\\EnvSet\\*.*");
	BOOL bWorking = finder.FindFile(szSaveDirectory);
	finder.Close();
	if (!bWorking)
	{
		i3mem::FillZero(szSaveDirectory, sizeof(char)*MAX_STRING_COUNT);
		i3::safe_string_copy(szSaveDirectory, szCurDir, MAX_STRING_COUNT);
		i3::generic_string_cat(szSaveDirectory, "\\EnvSet");

		::CreateDirectory(szSaveDirectory, NULL);
	}

	i3RegistrySet* pReg = i3RegistrySet::new_object();
	i3RegKey* pRoot = pReg->getRoot();

	pRoot->AddData( "Language",				m_nLanguageID);
	pRoot->AddData( "UI_Version",			m_nUIVersion);
	pRoot->AddData( "Full_Screen",			m_bFullScreen);
	pRoot->AddData( "Screen_Width",			m_ScreenWidth);
	pRoot->AddData( "Screen_Height",			m_ScreenHeight);
	pRoot->AddData( "Screen_Hz",				m_ScreenHz);
	pRoot->AddData( "Screen_Depth",			m_ScreenDepth);
	pRoot->AddData( "AntiAliasing",			m_AA);
	pRoot->AddData( "AntiAliasingQ",			m_AAQ);
	pRoot->AddData( "VSync",					m_bVSync);
	pRoot->AddData( "TriLinearFilter",		m_bTriLinearFilter);

	pRoot->AddData("Screen_Gamma_Value",		m_Gamma);
	pRoot->AddData("Speaker_Type",			m_SpeakerType);
	pRoot->AddData("Enable_Bullet_Trace",		m_bBulletTrailer);
	pRoot->AddData("Enable_DynamicLight",		m_bDynamicLight);
	pRoot->AddData("Enable_NormalMap",		m_bNormalMap);
	pRoot->AddData("Enable_SpecularMap",		m_bSpecularMap);
	pRoot->AddData("Enable_TerrainEffect",	m_bTerrainEffect);
	pRoot->AddData("Shadow_Quality_Type",		m_ShadowQuality);
	pRoot->AddData("Tex_Quality_Type",		m_TexQuality);
	pRoot->AddData( "16BitsDepth",			m_b16BitsDepth);

	pRoot->AddData( "Enable_TeamBand",			m_TeamBand);
	pRoot->AddData( "GameInterface",			m_InterFace);
	pRoot->AddData( "Display_Parts",			m_Parts);
	pRoot->AddData( "Display_Chara",			m_Chara);

	pReg->SaveRegistrySet(m_EnvFileName);
	I3_MUST_RELEASE(pReg);

	OnOK();
}
