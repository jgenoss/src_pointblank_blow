
// LiveManager.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "LiveManager.h"
#include "GlobalInstance.h"

#include "LiveManagerDoc.h"
#include "LiveManagerView.h"
#include "DlgList.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLiveManagerApp

BEGIN_MESSAGE_MAP(CLiveManagerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CLiveManagerApp::OnAppAbout)
	ON_COMMAND(ID_MENU_PEFPATH, &CLiveManagerApp::MediaSetting)
	ON_COMMAND(ID_MENU_SAVE, &CLiveManagerApp::MediaSave)
	ON_COMMAND(ID_PEF_MERGE, &CLiveManagerApp::MergePEF)
	ON_COMMAND(ID_PEF_CHECK, &CLiveManagerApp::CheckPEF)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)

	
	ON_COMMAND(ID_ADDWEAPON, &CLiveManagerApp::OnAddWeapon)	
	ON_COMMAND(ID_DELETEWEAPON, &CLiveManagerApp::OnDeleteWeapon)	
	ON_COMMAND(ID_SAFEWEAPONPEF, &CLiveManagerApp::OnSaveWeaponPef)
END_MESSAGE_MAP()


// CLiveManagerApp 생성

CLiveManagerApp::CLiveManagerApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CLiveManagerApp 개체입니다.

CLiveManagerApp theApp;


// CLiveManagerApp 초기화

BOOL CLiveManagerApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CLiveManagerDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CLiveManagerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	return TRUE;
}



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CLiveManagerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();

	
}

// CLiveManagerApp 사용자 지정 로드/저장 메서드

void CLiveManagerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);

	FILE* pSaveSetting = fopen("./MediaPath.ini","r");
	if(pSaveSetting)
	{
		fscanf(pSaveSetting,"%s\n%s",m_V1_MediaPath,m_V2_MediaPath);
		CGlobalInstance::getSingleton().GetWeaponPane(ID_WEAPON_PANE)->InitPropList(m_V2_MediaPath);

		fclose(pSaveSetting);
	}
}

void CLiveManagerApp::LoadCustomState()
{
}

void CLiveManagerApp::SaveCustomState()
{
}

// CLiveManagerApp 메시지 처리기
int CLiveManagerApp::Run()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ASSERT_VALID(this);
	_AFX_THREAD_STATE* pState = AfxGetThreadState();


	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;

	// acquire and dispatch messages until a WM_QUIT message is received.

	for (;;)
	{
		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
		{
			// call OnIdle while in bIdle state
			if (!OnIdle(lIdleCount++))
				bIdle = FALSE; // assume "no idle" state
		}

		//// phase2: pump messages while available
		while(::PeekMessage(&pState->m_msgCur, NULL, NULL, NULL, PM_NOREMOVE)) 
		{
			if (!PumpMessage())
				return ExitInstance();

			if (IsIdleMessage(&pState->m_msgCur))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}
		}
		CGlobalInstance::getSingleton().Update();
	}
	return CWinAppEx::Run();
}


void CLiveManagerApp::MediaSetting()
{
	CString str;

	CMediaPathDlg MediaPathDlg;

	MediaPathDlg.DoModal();

	
	
	if( MediaPathDlg.GetState() )
	{

		str = MediaPathDlg.GetV1Path();
		ZeroMemory(m_V1_MediaPath, 512);
		CGlobalInstance::getSingleton().StringToChar(str, m_V1_MediaPath, 512);

		str = MediaPathDlg.GetV2Path();
		ZeroMemory(m_V2_MediaPath, 512);
		CGlobalInstance::getSingleton().StringToChar(str, m_V2_MediaPath, 512);
		CGlobalInstance::getSingleton().GetWeaponPane(ID_WEAPON_PANE)->InitPropList(m_V2_MediaPath);

		FILE* pSaveSetting = fopen("./MediaPath.ini","w");
		if(pSaveSetting)
		{
			fprintf(pSaveSetting,"%s\n%s",m_V1_MediaPath,m_V2_MediaPath);
			fclose(pSaveSetting);
		}
	}
}

void CLiveManagerApp::MediaSave()
{
	//CGlobalInstance::getSingleton().GetWeaponPane()->Save();
}
void CLiveManagerApp :: CheckPEF()
{
	i3RegistrySet V1_Registry;
	i3RegistrySet V2_Registry;

	char MediaPath[512] = {0};

	sprintf_s(MediaPath, MAX_PATH, "%s\\Config\\Weapon.pef", m_V1_MediaPath);
	V1_Registry.SetCodePage( I3_LANG_CODE_ANSI );
	UINT32	Rv = V1_Registry.LoadRegistrySet( MediaPath, REGISTRY_FILE_BINARY);
	if( Rv == STREAM_ERR)
		return;

	sprintf_s(MediaPath, MAX_PATH, "%s\\Config\\Weapon.pef", m_V2_MediaPath);
	V1_Registry.SetCodePage( I3_LANG_CODE_ANSI );
	Rv = V2_Registry.LoadRegistrySet( MediaPath, REGISTRY_FILE_BINARY);
	if( Rv == STREAM_ERR)
		return;


	i3RegKey * pList_V1 = (i3RegKey *)i3TreeNode::FindNodeByName( V1_Registry.getRoot(), "WeaponList");
	I3ASSERT( pList_V1 != NULL );
	i3RegKey * pWeaponkey_V1 = (i3RegKey*) i3TreeNode::FindNodeByName( pList_V1, "Weapon");
	I3ASSERT( pWeaponkey_V1 != NULL );


	i3RegKey * pList_V2 = (i3RegKey *)i3TreeNode::FindNodeByName( V2_Registry.getRoot(), "WeaponList");
	I3ASSERT( pList_V2 != NULL );
	i3RegKey * pWeaponkey_V2 = (i3RegKey*) i3TreeNode::FindNodeByName( pList_V2, "Weapon");
	I3ASSERT( pWeaponkey_V2 != NULL );




	

	FILE* isContextSave = fopen("./Merge.log", "w");
	if(isContextSave)
	{
		i3::vector<i3RegKey*> V1_WeaponArray;
		i3::vector<i3RegKey*> V2_WeaponArray;
		i3::vector<i3RegKey*> V2_NotFindArray;
		

		for(int i = 0;i < pWeaponkey_V1->getChildCount();++i)
		{
			i3RegKey* pChildWeapon = (i3RegKey*)pWeaponkey_V1->getChild(i);
			for(int j = 0;j < pChildWeapon->getChildCount();++j)
			{
				i3RegKey* pChildWeapon2 = (i3RegKey*)pChildWeapon->getChild(j);
				V1_WeaponArray.push_back(pChildWeapon2);
			}
		}
		for(size_t Cnt = 0;Cnt < V1_WeaponArray.size();++Cnt)
		{
			i3RegKey* pV2_Weapon = NULL;
			for(int i = 0;i < pWeaponkey_V2->getChildCount();++i)
			{
				i3RegKey* pChildWeapon = (i3RegKey*)pWeaponkey_V2->getChild(i);
				for(int j = 0;j < pChildWeapon->getChildCount();++j)
				{
					i3RegKey* pChildWeapon2 = (i3RegKey*)pChildWeapon->getChild(j);

					if(pChildWeapon2->GetNameString() == V1_WeaponArray[Cnt]->GetNameString().c_str())
					{
						pV2_Weapon = pChildWeapon2;
						V2_WeaponArray.push_back(pV2_Weapon);
						break;
					}
				}
				if(pV2_Weapon)
					break;
			}
			if(!pV2_Weapon)
			{
				V2_NotFindArray.push_back(V1_WeaponArray[Cnt]);
				continue;
			}

			i3::vector<i3::string> V1_UseArray;
			i3::vector<i3::string> V2_UseArray;
			i3::vector<i3::string> Common_UseArray;
			for(int i = 0;i < pV2_Weapon->GetDataCount(); ++i)
			{
				bool UseFlag = false;
				for(int j = 0;j < V1_WeaponArray[Cnt]->GetDataCount(); ++j)
				{
					if(pV2_Weapon->GetData(i)->GetNameString() == V1_WeaponArray[Cnt]->GetData(j)->GetNameString())
					{
						UseFlag = true;
						break;
					}
				}
				if(UseFlag)
				{
					Common_UseArray.push_back(pV2_Weapon->GetData(i)->GetNameString());
				}
				else
				{
					V2_UseArray.push_back(pV2_Weapon->GetData(i)->GetNameString());
				}
			}
			for(int i = 0;i < V1_WeaponArray[Cnt]->GetDataCount(); ++i)
			{
				bool UseFlag = false;
				for(size_t j = 0;j < Common_UseArray.size(); ++j)
				{
					if(Common_UseArray[j] == V1_WeaponArray[Cnt]->GetData(i)->GetName())
					{
						UseFlag = true;
						break;
					}
				}
				if(!UseFlag)
				{
					V1_UseArray.push_back(V1_WeaponArray[Cnt]->GetData(i)->GetNameString());
				}
			}

			fprintf(isContextSave,"%s : 공용사용\n",V1_WeaponArray[Cnt]->GetNameString().c_str());
			for(size_t i = 0;i < Common_UseArray.size(); ++i)
			{
				fprintf(isContextSave,"%s\n",Common_UseArray[i].c_str());
			}
			fprintf(isContextSave,"\nV2만사용\n");
			for(size_t i = 0;i < V2_UseArray.size(); ++i)
			{
				fprintf(isContextSave,"%s\n",V2_UseArray[i].c_str());
			}
			fprintf(isContextSave,"\nV1만사용\n");
			for(size_t i = 0;i < V1_UseArray.size(); ++i)
			{
				fprintf(isContextSave,"%s\n",V1_UseArray[i].c_str());
			}
			fprintf(isContextSave,"\n\n########################################################\n\n");
		}

		fprintf(isContextSave,"\nV1만있고 V2은 없는 무기리스트 %d 개 \n", V2_NotFindArray.size());
		for(size_t i = 0;i < V2_NotFindArray.size(); ++i)
		{
			fprintf(isContextSave,"%s\n",V2_NotFindArray[i]->GetNameString().c_str());	
		}

		fprintf(isContextSave,"\nV2만있고 V1은 없는 무기리스트 \n");

		int SaveCnt = 0;
		for(int i = 0;i < pWeaponkey_V2->getChildCount();++i)
		{
			
			i3RegKey* pChildWeapon = (i3RegKey*)pWeaponkey_V2->getChild(i);
			for(int j = 0;j < pChildWeapon->getChildCount();++j)
			{
				i3RegKey* pV2_Weapon = NULL;
				i3RegKey* pChildWeapon2 = (i3RegKey*)pChildWeapon->getChild(j);

				for(size_t Cnt = 0;Cnt < V1_WeaponArray.size();++Cnt)
				{
					if(pChildWeapon2->GetNameString() == V1_WeaponArray[Cnt]->GetNameString().c_str())
					{
						pV2_Weapon = V1_WeaponArray[Cnt];
						break;
					}
				}
				if(!pV2_Weapon)
				{
					SaveCnt++;
					fprintf(isContextSave,"%s\n",pChildWeapon2->GetNameString().c_str());	
				}
			}
		}
		fprintf(isContextSave,"\n %d 개 \n", SaveCnt);
		fclose(isContextSave);

		isContextSave = NULL;
		isContextSave = fopen("./MergeV1.log", "w");
		if(isContextSave)
		{
			for(size_t i = 0;i < V1_WeaponArray.size();++i)
			{
				
				fprintf(isContextSave,"%s\n\n",V1_WeaponArray[i]->GetName());
				for(int j = 0;j < V1_WeaponArray[i]->GetDataCount();++j)
				{
					if(V1_WeaponArray[i]->GetData(j)->GetNameString() == "_UsageType")
						continue;
					fprintf(isContextSave,"%s\n", V1_WeaponArray[i]->GetData(j)->GetName());
					i3RegArray* pRegArray = (i3RegArray*)V1_WeaponArray[i]->GetData(j);
					for(size_t k = 0;k <pRegArray->getSize();++k)
					{
						i3::wstring Value;
						char ValueStr[256] = {0,};
						(*pRegArray)[k]->GetValueByString(Value);
						WideCharToMultiByte(CP_ACP, 0, Value.c_str(), -1, ValueStr, 256, NULL, NULL);
						fprintf(isContextSave,"%s ", ValueStr);
	
					}
					fprintf(isContextSave,"\n\n");
				}
				fprintf(isContextSave,"\n#########################################################################\n\n");
				
			}
			fclose(isContextSave);
		}

		isContextSave = NULL;
		isContextSave = fopen("./MergeV2.log", "w");
		if(isContextSave)
		{
			for(size_t i = 0;i < V2_WeaponArray.size();++i)
			{

				fprintf(isContextSave,"%s\n\n",V2_WeaponArray[i]->GetName());
				for(int j = 0;j < V2_WeaponArray[i]->GetDataCount();++j)
				{
					fprintf(isContextSave,"%s\n", V2_WeaponArray[i]->GetData(j)->GetName());
					i3RegArray* pRegArray = (i3RegArray*)V2_WeaponArray[i]->GetData(j);
					for(size_t k = 0;k <pRegArray->getSize();++k)
					{
						i3::wstring Value;
						char ValueStr[256] = {0,};
						(*pRegArray)[k]->GetValueByString(Value);
						WideCharToMultiByte(CP_ACP, 0, Value.c_str(), -1, ValueStr, 256, NULL, NULL);
						fprintf(isContextSave,"%s ", ValueStr);

					}
					fprintf(isContextSave,"\n\n");
				}
				fprintf(isContextSave,"\n#########################################################################\n\n");

			}
			fclose(isContextSave);
		}
	}
}

void CLiveManagerApp :: MergePEF()
{
	i3RegistrySet_Impl V1_Registry;
	i3RegistrySet_Impl V2_Registry;

	char V1_MediaPath[512] = {0};
	char V2_MediaPath[512] = {0};


	sprintf_s(V1_MediaPath, MAX_PATH, "%s\\Config\\Weapon.pef", m_V1_MediaPath);
	V1_Registry.SetCodePage( I3_LANG_CODE_ANSI );
	UINT32	Rv = V1_Registry.LoadRegistrySet( V1_MediaPath, REGISTRY_FILE_BINARY);
	if( Rv == STREAM_ERR)
		return;

	i3RegKey * pList_V1 = (i3RegKey *)i3TreeNode::FindNodeByName( V1_Registry.getRoot(), "WeaponList");
	I3ASSERT( pList_V1 != NULL );
	i3RegKey * pWeaponkey_V1 = (i3RegKey*) i3TreeNode::FindNodeByName( pList_V1, "Weapon");
	I3ASSERT( pWeaponkey_V1 != NULL );


	sprintf_s(V2_MediaPath, MAX_PATH, "%s\\Config\\Weapon.pef", m_V2_MediaPath);
	V2_Registry.SetCodePage( I3_LANG_CODE_ANSI );
	Rv = V2_Registry.LoadRegistrySet( V2_MediaPath, REGISTRY_FILE_BINARY);
	if( Rv == STREAM_ERR)
		return;

	i3RegKey * pList_V2 = (i3RegKey *)i3TreeNode::FindNodeByName( V2_Registry.getRoot(), "WeaponList");
	I3ASSERT( pList_V2 != NULL );
	i3RegKey * pWeaponkey_V2 = (i3RegKey*) i3TreeNode::FindNodeByName( pList_V2, "Weapon");
	I3ASSERT( pWeaponkey_V2 != NULL );


// 	i3RegistrySet_Impl V1_Registry;
// 	i3RegistrySet_Impl V2_Registry;
// 
// 	char V1_MediaPath[512] = {0};
// 
// 	char V2_MediaPath[512] = {0};
// 
// 	sprintf_s(V1_MediaPath, MAX_PATH, "%s\\Config\\Weapon.pef", m_V1_MediaPath);
// 	V1_Registry.SetCodePage( I3_LANG_CODE_ANSI );
// 	UINT32	Rv = V1_Registry.LoadRegistrySet( V1_MediaPath, REGISTRY_FILE_BINARY);
// 	if( Rv == STREAM_ERR)
// 		return;
// 
// 	sprintf_s(V2_MediaPath, MAX_PATH, "%s\\Config\\Weapon.pef", m_V2_MediaPath);
// 	V1_Registry.SetCodePage( I3_LANG_CODE_ANSI );
// 	Rv = V2_Registry.LoadRegistrySet( V2_MediaPath, REGISTRY_FILE_BINARY);
// 	if( Rv == STREAM_ERR)
// 		return;
// 
// 	i3RegKey * pList_V1 = (i3RegKey *)i3TreeNode::FindNodeByName( V1_Registry.getRoot(), "WeaponList");
// 	I3ASSERT( pList_V1 != NULL );
// 	i3RegKey * pWeaponkey_V1 = (i3RegKey*) i3TreeNode::FindNodeByName( pList_V1, "Weapon");
// 	I3ASSERT( pWeaponkey_V1 != NULL );
// 
// 
// 	i3RegKey * pList_V2 = (i3RegKey *)i3TreeNode::FindNodeByName( V2_Registry.getRoot(), "WeaponList");
// 	I3ASSERT( pList_V2 != NULL );
// 	i3RegKey * pWeaponkey_V2 = (i3RegKey*) i3TreeNode::FindNodeByName( pList_V2, "Weapon");
// 	I3ASSERT( pWeaponkey_V2 != NULL );
// 
// 	i3::string V2_ParentName;
// 	INT32 ItemID = 0;
// 	i3RegKey* pV2_Weapon = NULL;
// 
//  	for(int i = 0;i < pWeaponkey_V2->getChildCount();++i)
// 	{
// 		i3RegKey* pChildWeapon = (i3RegKey*)pWeaponkey_V2->getChild(i);
// 		for(int j = 0;j < pChildWeapon->getChildCount();++j)
// 		{
// 			V2_ParentName = pChildWeapon->GetNameString();
// 			i3RegKey* pChildWeapon2 = (i3RegKey*)pChildWeapon->getChild(j);
// 
// 			if(pChildWeapon2->GetNameString() == CGlobalInstance::getSingleton().GetWeaponPane()->getBaseItemName().c_str())
// 			{
// 				pV2_Weapon = pChildWeapon2;
// 				break;
// 			}
// 		}
// 		if(pV2_Weapon)
// 			break;
// 	}
// 
// 	if(!pV2_Weapon)
// 		return;
// 
// 	for(int i = 0;i < pV2_Weapon->GetDataCount();++i)
// 	{
// 		if(pV2_Weapon->GetData(i)->GetNameString() == "ITEMID")
// 		{
// 			i3RegArray* pRegArray = (i3RegArray*)pV2_Weapon->GetData(i);
// 			if(pRegArray)
// 				(*pRegArray)[0]->GetValue(&ItemID);
// 			break;
// 		}
// 	}
// 	
// 	if(ItemID > 0)
// 	{
// 		INT32 Value = ItemID / 100000 - 1;
// 		//i3RegArray* RegArray;
// 		//i3RegData  RegData;
// 		
// 		//RegArray.set
// 		pV2_Weapon->AddData("_UsageType", Value);
// 	}
// 	else
// 		pV2_Weapon->AddData("_UsageType", 0);
// 
// 	
// 
// 	for(int i = 0;i < pWeaponkey_V1->getChildCount();++i)
// 	{
// 		bool BreakFlag = false;
// 		i3RegKey* pChildWeapon = (i3RegKey*)pWeaponkey_V1->getChild(i);
// 		for(int j = 0;j < pChildWeapon->getChildCount();++j)
// 		{
//  			if(V2_ParentName == pChildWeapon->GetName())
// 			{
// 				pChildWeapon->AddChild(pV2_Weapon);
// 				BreakFlag = true;
// 				break;
// 			}
// 		}
// 		if(BreakFlag)
// 			break;
// 	}

	//SavePEF(&V1_Registry, V1_MediaPath);

}


bool CLiveManagerApp :: SavePEF( i3RegistrySet* pRegistry, const char * pszPath)
{

	char szPefName[MAX_PATH] = {0,};
	char szXmlName[MAX_PATH] = {0,};
	i3::safe_string_copy( szPefName, pszPath, sizeof(szPefName));
	i3::safe_string_copy( szXmlName, pszPath, strlen(pszPath)-2);
	strcat(szXmlName,"i3RegXML");


	//	Binary로 저장
	if( pRegistry->SaveRegistrySet( szPefName, REGISTRY_FILE_BINARY ) == STREAM_ERR)
	{
		return false;
	}

	//	i3RegXML로 저장
	if( pRegistry->SaveRegistrySet( szXmlName, REGISTRY_FILE_XML_BY_UTF8 ) == STREAM_ERR)
	{
		return false;
	}

	return true;
}

// i3RegData * CLiveManagerApp::AddNewData( INT32 nType, INT32 nValue, INT32 nNationCount )
// {
//  	CString NewDataName = L"NEWDATA";
// 
//  	{
// 		char chName[256];
// 		ZeroMemory(chName, 256);
//  		i3RegArray * pNewArrayData = i3RegArray::new_object();
//  
//  		pNewArrayData->setElementType( nType );
// 		CGlobalInstance::getSingleton().StringToChar(NewDataName, chName, 256);
//  		pNewArrayData->SetName( (const char*)chName );
//  
//  		int size = CGlobalInstance::getSingleton().GetWeaponPane(ID_WEAPON_PANE)->GetNationCount();
//  		for(int i =0; i<size; i++)
// 		{
// 			pNewArrayData->addRegData( pNewArrayData->createRegData() );
// 		}
//  
//  
//  		return pNewArrayData;
//  	}
// 
// 	return NULL;
// }

void CLiveManagerApp::OnAddWeapon()
{
	CGlobalInstance::getSingleton().AddRegistry();
}

void CLiveManagerApp::OnDeleteWeapon()
{
	CGlobalInstance::getSingleton().DeleteRegistry();
}

void CLiveManagerApp::OnSaveWeaponPef()
{
	CGlobalInstance::getSingleton().StartSave();
}