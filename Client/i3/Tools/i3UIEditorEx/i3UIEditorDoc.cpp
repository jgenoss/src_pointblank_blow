// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3UIEditorDoc.cpp : implementation of the Ci3UIEditorDoc class
//

#include "stdafx.h"
#include "i3UIEditor.h"

#include "i3UIEditorDoc.h"
#include "i3UIEditorView.h"
#include "MainFrm.h"
#include "GlobalVariables.h"

#include "UndoSystem2/UIUndoSystem.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/make_relative_path.h"

#include "i3Framework/i3UI/i3VTFileFormat.h"
#include "i3Framework/i3UI/i3UIFilePathMgr.h"

#include	<algorithm>
#include	<cctype>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3UIEditorDoc

IMPLEMENT_DYNCREATE(Ci3UIEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(Ci3UIEditorDoc, CDocument)
END_MESSAGE_MAP()


// Ci3UIEditorDoc construction/destruction

Ci3UIEditorDoc::Ci3UIEditorDoc()
{
	// TODO: add one-time construction code here
	m_pScene	= NULL;
	m_bLoaded	= false;
	m_bFirst	= true;
}

Ci3UIEditorDoc::~Ci3UIEditorDoc()
{
	I3_SAFE_RELEASE( m_pScene);
}

//NewScene을 한경우에 scene의 해상도를 mainframe의 해상도로 설정해준다.
void Ci3UIEditorDoc::_AdjustResolution( void)
{
	//Scene의 해상도를 설정해 준다. 
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	if( pMainFrm != NULL)
	{
		UINT32 uID = pMainFrm->getResolutionID();
		
		INT32 width = 0;
		INT32 height = 0;
		switch( uID)
		{
			case ID_SCR_RES_800X600:	width = 800;	height = 600;	break;
			case ID_SCR_RES_1024X768:	width = 1024;	height = 768;	break;
			case ID_SCR_RES_1280X800:	width = 1280;	height = 800;	break;
			case ID_SCR_RES_1280X1024:	width = 1280;	height = 1024;	break;
			case ID_SCR_RES_1600X1080:	width = 1600;	height = 1080;	break;
			case ID_SCR_RES_1920X1080:	width = 1920;	height = 1080;	break;
			case ID_SCR_RES_1920X1200:	width = 1920;	height = 1200;	break;
			default:	I3ASSERT_0;		break;
		}
		
		//pMainFrm->SetResolution( width, height);
		
		//pMainFrm에서 scene의 해상도를 설정하지 않고 return하는 경우가 있다.
		//그 경우에 대비하여 한번 더 콜.. 
		SetResolution( width, height);	

		//pMainFrm->_CreateRenderTargets( width, height);
	}
}

BOOL Ci3UIEditorDoc::OnNewDocument()
{
	I3TRACE("Ci3UIEditorDoc::OnNewDocument\n");

	m_bLoaded = false;
	
	if (!CDocument::OnNewDocument())
	{
		return FALSE;
	}

	if( m_bFirst)
	{
		m_bFirst = false;
		m_bLoaded = true;
		return TRUE;
	}

	_CloseUIScene();


	//New
	i3UIScene * pScene = i3UIScene::new_object_ref();
	pScene->SetName( "NewScene");

	setCurrentScene( pScene, "NewScene.tmp");
	UIUndo::Init( pScene);

	_AdjustResolution();

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if( pMainFrm != NULL)
		pMainFrm->SetResolution( pScene->getWidth(), pScene->getHeight());

	m_bLoaded = true;
	
	theApp.SwitchView();

	return TRUE;
}

static void sRecSceneToEditMode( i3GameNode * pNode)
{
	if( i3::kind_of<i3UIControl*>(pNode)) //->IsTypeOf( i3UIControl::static_meta()) )
	{
		((i3UIControl*)pNode)->OnEditMode();
	}

	i3GameNode * pChild = pNode->getFirstChild();
	while( pChild != NULL)
	{
		sRecSceneToEditMode( pChild);

		pChild = pChild->getNext();
	}	
}

void Ci3UIEditorDoc::_CloseUIScene(void)
{
	i3UI::setUILibrary( NULL);
	i3UI::CloseVirtualTexture();

	i3UIManager* pUI = i3UI::getManager();
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	if (pUI)
	{
	//	pUI->SetLocale("");
		i3UIFilePathMgr::i()->SetCurrentLanguageID(-1);
	//	pUI->SetUIResourceFolder("");
		pMainFrm->m_nNationSel = ID_LANGUAGE_DEV;
	}

	if (pMainFrm)
	{
		i3ResourceFile::SetWorkingFolder((char*)pMainFrm->getWorkingFolder());
		CMFCRibbonButton * pBtn = (CMFCRibbonButton*)pMainFrm->GetRibbonBar()->GetCategory(2)->GetPanel(7)->GetElement(1);

		pBtn->SetText("기본Locale은 Media입니다");
	}

	m_bLoaded = false;	
}

namespace detail
{
	// 아래 주석은 기존 것 참고용...이름이 좀 틀려먹은 부분도 있기 때문에 참고만 하고...직접 폴더를 뒤져보는게 좋을 것임...
	/*
	const char*		CMainFrame::GetNationString(int id) const
	{
		if (i3::generic_is_iequal(i3UI::getManager()->GetUIVersionName()->c_str(), "uire"))
		{
			switch (id)
			{
			case ID_LANGUAGE_MIDDLEEAST:		return "MiddleEast";	break;
			case ID_LANGUAGE_THAILAND:			return "Thailand";		break;
			case ID_LANGUAGE_ENGLISH:			return "English";		break;
			case ID_LANGUAGE_PHILIPPINES:		return "English/Philippines";	break;
			case ID_LANGUAGE_SINGMAL:			return "English/Singmal";		break;
			case ID_LANGUAGE_NORTHAMERICA:		return "English/USA";	break;
			case ID_LANGUAGE_INDONESIA:			return "Indonesia";		break;
			case ID_LANGUAGE_ITALY:				return "Italian";			break;
			case ID_LANGUAGE_KOREAN:			return "Korean";			break;
			case ID_LANGUAGE_BRAZIL:			return "Brazil";		break;
			case ID_LANGUAGE_RUSSIA:			return "Russian";		break;
			case ID_LANGUAGE_LATINAMERICA:		return "Spanish/Spanish_Chile";	break;
			case ID_LANGUAGE_THAIWAN:			return "Thaiwan";		break;
			case ID_LANGUAGE_TURKY:				return "Turkish";			break;
			case ID_LANGUAGE_VIETNAM:			return "Vietnamese";		break;
			case ID_LANGUAGE_JAPAN:				return "Japanese";		break;
			}
		}
		else
		{
			switch (id)
			{
			case ID_LANGUAGE_THAILAND:			return "Thailand";		break;
			case ID_LANGUAGE_PHILIPPINES:		return "Philippines";	break;
			case ID_LANGUAGE_SINGMAL:			return "Singmal";		break;
			case ID_LANGUAGE_NORTHAMERICA:		return "NorthAmerica";	break;
			case ID_LANGUAGE_INDONESIA:			return "Indonesia";		break;
			case ID_LANGUAGE_ITALY:				return "Italy";			break;
			case ID_LANGUAGE_KOREAN:			return "Korea";			break;
			case ID_LANGUAGE_BRAZIL:			return "Brazil";		break;
			case ID_LANGUAGE_RUSSIA:			return "Russia";		break;
			case ID_LANGUAGE_LATINAMERICA:		return "LatinAmerica";	break;
			case ID_LANGUAGE_THAIWAN:			return "Thaiwan";		break;
			case ID_LANGUAGE_TURKY:				return "Turky";			break;
			case ID_LANGUAGE_VIETNAM:			return "Vietnam";		break;
			case ID_LANGUAGE_SPANISH:			return "Spanish";		break;
			case ID_LANGUAGE_CHINA:				return "China";			break;
			case ID_LANGUAGE_ARABIC:			return "Arabic";		break;
			case ID_LANGUAGE_JAPAN:				return "Japan";		break;
			case ID_LANGUAGE_ENGLISH:				return "English";		break;
			}
		}

		return "Dev";
	}
	*/

	
	static void AddLanguageSubItemAndSetting(i3Language_ID_Str_Set& inout, CMainFrame* pFrame, INT32 nID, const i3::literal_range& rngLanguage)
	{
		pFrame->AddSubItem(new CMFCRibbonButton(nID, rngLanguage.c_str()));
		vt_util::i3Language_ID_Str_AddLanguage(inout, nID, rngLanguage);
	}
	
	static void MakeUpLanguageSubItemAndLanguageSetting_OldUI(CMainFrame* pFrame)
	{
		i3Language_ID_Str_Set setLanguage;

		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_DEV, "Dev");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_PHILIPPINES, "English/Philippines");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_SINGMAL, "English/Singmal");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_NORTHAMERICA, "English/USA");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_ITALY, "Italian");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_KOREAN, "Korean");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_VIETNAM, "Vietnamese");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_JAPAN, "Japanese");

		i3UIFilePathMgr::i()->SetLanguageSetting(setLanguage, false);
	}

	static void MakeUpLanguageSubItemAndLanguageSetting_NewUI(CMainFrame* pFrame)
	{
		i3Language_ID_Str_Set setLanguage;
		
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_DEV, "Dev");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_BRAZIL, "Brazil");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_INDONESIA, "Indonesia");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_ITALY, "Italy");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_KOREAN, "Korea");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_LATINAMERICA, "Latin_America");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_NORTHAMERICA, "NorthAmerica");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_PHILIPPINES, "Philippines");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_RUSSIA, "Russia");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_SINGMAL, "Singmal");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_TURKY, "Turkey");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_VIETNAM, "Vietnam");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_THAILAND, "Thai");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_JAPAN, "Japan");
		AddLanguageSubItemAndSetting(setLanguage, pFrame, ID_LANGUAGE_ENGLISH, "English");

		i3UIFilePathMgr::i()->SetLanguageSetting(setLanguage, true);
	}
}


BOOL Ci3UIEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	I3TRACE("Ci3UIEditorDoc::OnOpenDocument\n");

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	if( NULL == pMainFrm )
	{
		return FALSE;
	}

	// working folder sync
	{
		i3::literal_range strInputPathName = lpszPathName;
		i3::stack_string outFoundWorkDir;
		i3::stack_string outFoundUIVer;
		bool			 out_isNewUI;

		bool bWorkingDirMatched = vt_util::Check_WorkDirectory_And_Version_With_UISPath(lpszPathName, outFoundWorkDir, outFoundUIVer, out_isNewUI);

		if (outFoundUIVer.empty()) // UI버전을 전혀 못찾았다면 그냥 오류이다..
		{
			AfxMessageBox("UI루트 경로를 찾을수가 없네요.\n UIRe/ UI_VXX가 루트디렉토리가 되어야 합니다.", MB_OK);
			return FALSE;
		}

		pMainFrm->ClearNationControl();
		i3UIFilePathMgr::i()->SetUIVersionString(outFoundUIVer);		// UIRe or UI_VXX 등을 매니저에 입력..

		if (out_isNewUI == false)  // 구버전 UI
		{
			detail::MakeUpLanguageSubItemAndLanguageSetting_OldUI(pMainFrm);
		}
		else // V3 버전..
		{
			detail::MakeUpLanguageSubItemAndLanguageSetting_NewUI(pMainFrm);
		}
		
		if (bWorkingDirMatched == false)			// 설정된 WorkingDirectory와 다르면 변경해줄 것..
		{
			int nRes = AfxMessageBox("워킹 폴더 경로가 맞지 않습니다.\n워킹 폴더 경로를 파일에 맞추어 변경 하시겠습니까?", MB_YESNO);

			if (IDYES == nRes)
			{
				pMainFrm->SetWorkingFolder(outFoundWorkDir.c_str());
			}
			else
			{
				return FALSE;
			}
		}

		AddFontList(outFoundWorkDir.c_str());

		/*
		std::string path = lpszPathName;
		std::string ChildWorkDir;
		size_t _npos1 = path.find("\\UI");
		size_t _npos2 = path.find("\\", _npos1+1);

		path.assign(path, 0, _npos2);

		char szRoot[MAX_PATH];
		i3::extract_filename(path, szRoot);

		if (!i3::generic_is_iequal(szRoot, "uire") && !i3::generic_is_iequal(szRoot, "UI_Temp"))
			path += "\\VirtualTexture";

		std::string checker = pMainFrm->getWorkingFolder();

		std::transform( path.begin(), path.end(), path.begin(), std::tolower );
		std::transform( checker.begin(), checker.end(), checker.begin(), std::tolower );


		if (!i3::generic_is_iequal(szRoot, "uire") && !i3::generic_is_iequal(szRoot, "UI_Temp"))
		{
			char szDir[MAX_PATH];
			i3::extract_directoryname(path, szDir);
			i3::extract_filename(szDir, szRoot);
		}

		//i3::extract_filename(path, szRoot);

		pMainFrm->ClearNationControl();
		i3UIManager* pUI = i3UI::getManager();
		pUI->SetUIVersionName(szRoot);
		if( i3::generic_is_iequal( szRoot, "uire") )
		{
			detail::MakeUpLanguageSubItemAndLanguageSetting_OldUI(pMainFrm);

		}
		else if( i3::generic_is_iequal( szRoot, "ui_v10") ||
			i3::generic_is_iequal( szRoot, "ui_v11") ||
			i3::generic_is_iequal( szRoot, "ui_v15") ||
			i3::generic_is_iequal(szRoot, "VirtualTexture"))
		{
			detail::MakeUpLanguageSubItemAndLanguageSetting_NewUI(pMainFrm);
		}

		if( 0 != path.compare( checker ) )
		{
			int nRes = AfxMessageBox( "워킹 폴더 경로가 맞지 않습니다.\n워킹 폴더 경로를 파일에 맞추어 변경 하시겠습니까?", MB_YESNO );

			if( IDYES == nRes )
			{
				pMainFrm->SetWorkingFolder( path.c_str() );
				//else			//후에 폴더가 있느느지로 체크 해야함
				//{
				//	char sz[1024];
				//	i3::snprintf( sz, 1024, "UIRe 폴더를 찾지 못해 워킹 폴더 변경에 실패 했습니다.\n'%s' 파일을 여시겠습니까?", lpszPathName );

				//	if( IDNO == AfxMessageBox(  sz, MB_YESNO ) )
				//	{
				//		return FALSE;
				//	}
				//}
			}
			else
			{
				return FALSE;
			}
		}
		//else
		//{
		//	//매치는 하지만, 국가별 로케일이 다를 때

		//	const std::string& uire = "uire";

		//	unsigned int ps_uire = path.rfind( uire );

		//	if( std::string::npos != ps_uire )
		//	{
		//		std::string sz = path.substr( 0, ps_uire - 1 );
		//		//UIRe까지 떼낸 스트링과 워킹 폴더가 같지 않을 때 변경한다.
		//		if(sz.compare(checker) != 0)
		//		{
		//			int nRes = AfxMessageBox( "워킹 폴더 경로가 맞지 않습니다.\n워킹 폴더 경로를 파일에 맞추어 변경 하시겠습니까?", MB_YESNO );
		//			if( IDYES == nRes )
		//			{
		//				pMainFrm->SetWorkingFolder(sz.c_str());
		//			}
		//			else
		//			{
		//				return FALSE;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		char sz[1024];
		//		i3::snprintf( sz, 1024, "국가별 UIRe 폴더를 찾지 못했습니다.\n'%s' 파일을 여시겠습니까?", lpszPathName );

		//		if( IDNO == AfxMessageBox(  sz, MB_YESNO ) )
		//		{
		//			return FALSE;
		//		}
		//	}
		//}


		AddFontList(path);
		*/
	}

	{
		i3XMLFile File;
		i3::rc_string strCommonUIFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(true);

		i3::stack_string strFullPath;
		i3::sprintf(strFullPath, "%s\\VirtualTexture\\filelist.i3Animation", strCommonUIFolder);
		if (i3System::IsFile(strFullPath.c_str()))
		{
			if (File.Load(strFullPath.c_str()) == false)
			{
				I3PRINTLOG(I3LOG_NOTICE, "2DAnimation file load failed");
			}
			else
			{
				i3Framework * pFramework = i3Framework::getCurrentFramework();
				pFramework->setUIAnimationMgr((i3UIAnimationMgr*)File.getKeyObject());
			}
		}
	}

	i3UIManager * pUI = i3UI::getManager();
	I3ASSERT( pUI != NULL);

	_CloseUIScene();

	INT32 nWidth = pUI->getWidth();
	INT32 nHeight = pUI->getHeight();

	//LoadUIScene 안에서 UIManager의 해상도(width, height)가 바뀐다.
	i3UIScene * pScene = pUI->LoadUIScene((char*)lpszPathName);

	if( pScene != NULL)
	{
		// 국가정보를 읽는다.
		pMainFrm->UpdateNationFromVTex();
		// 기본 image폴더의 정보를 읽는다.
		pMainFrm->UpdateDefaultImageInfo();

		if (!CDocument::OnOpenDocument(lpszPathName))
			return FALSE;

		//해상도가 다른 경우 경고 메시지
		//if( (pUI->getWidth() != pScene->getWidth()) || (pUI->getHeight() != pScene->getHeight()) )
		if( (pUI->getWidth() != nWidth) || (pUI->getHeight() != nHeight) )
		{
			I3TRACE( "%d, %d, %d, %d\n", pUI->getWidth(), pScene->getWidth(), pUI->getHeight(), pScene->getHeight());
			::MessageBox( NULL, "해상도가 다릅니다.", "i3UIEditor", MB_OK);

			// Manager의 해상도는 Scene에 맞춰진다.(LoadUIScene)
			// MainFrame의 해상도를 Manager에 맞춘다. 
			pMainFrm->SetResolution( pUI->getWidth(), pUI->getHeight());
		}

		theApp.SwitchView();

		// i3UIManager::LoadUIScene() 함수 내에서 UI Manager에 로드된 Scene을 등록하기
		// 때문에 여기에서는 추가로 등록하지 않도록...
		setCurrentScene( pScene, lpszPathName, false);
		setUILibrary( i3UI::getUILibrary());

		// ListView Item은 EditMode로 설정해야된다.
		if( pScene != NULL)
			sRecSceneToEditMode( pScene);

		m_bLoaded = true;

		UIUndo::Init( pScene);

		pMainFrm->CloseAllScript();

		return TRUE;
	}

	m_bLoaded = true;

	return FALSE;
}

BOOL Ci3UIEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	I3TRACE("Ci3UIEditorDoc::OnSaveDocument\n");

	m_bLoaded = false;

	if( m_pScene->getLuaContext() != NULL)
	{
		I3PRINTLOG(I3LOG_WARN,"미리보기 모드에서는 저장 할 수 없습니다.\n");
		return FALSE;
	}

	//Scene의 이름과 I3UI_SCENE_INFO의 filename을 바꿔줍니다.
	char szName[MAX_PATH];
//	i3String::SplitFileName( lpszPathName, szName, FALSE);
	i3::extract_filetitle(lpszPathName, szName);

	m_pScene->SetName( szName);

	i3UIManager * pUI = i3UI::getManager();
	I3ASSERT( pUI != NULL);
	pUI->ChangeSceneFile( m_pScene, lpszPathName);

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	CUIPaneScreen* pPane = pMainFrm->getScreenPane();
	pPane->SetScene( m_pScene);		//바뀐 이름을 업데이트 해준다. (현재 적절한 함수가 없음)

	i3XMLFile File;
	File.setKeyObject( m_pScene);

	if( File.Save( lpszPathName) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "failed save file: %s\n", lpszPathName);
		return FALSE;
	}

	//AutoSaveTemplateLibrary();
	//SaveTemplateLibrary();

	m_bLoaded = true;

	return TRUE;
}

void Ci3UIEditorDoc::OnCloseDocument()
{
	I3TRACE("Ci3UIEditorDoc::OnCloseDocument\n");
	CDocument::OnCloseDocument();
}

void Ci3UIEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

//OnNewDocument, OnOpenDocument 하기 전에 반드시 호출된다.
//원래 작업 중이던 Scene을 제거한다. 
void Ci3UIEditorDoc::DeleteContents()
{
	UIUndo::Close();

	CheckSaveTemplateLibrary();
	

	// EditorView와 EditorView_Template을 모두 inactivate 시킨다. 
	// OnActivateView에서 각각 DisableUI, DetachScene, SetPrevMode(False), Invalidate()등이 호출된다. 

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if( pMainFrm != NULL)
		pMainFrm->SetActiveView( NULL);

 	if( m_pScene != NULL)
	{
		i3UIManager* pManager = i3UI::getManager();

		if( pManager != NULL)
		{
			pManager->RemoveScene( m_pScene);
		}

		//m_pScene의 child control들 제거하기? 
		
		m_pScene->Detach();
		pMainFrm->setEnableOtherScene( false);
	}

	setCurrentScene( NULL, NULL);
	
	setUILibrary( NULL);

	CDocument::DeleteContents();
}

//CanCloseFrame => OnCloseDocument => PreCloseFrame => DeleteContents
BOOL Ci3UIEditorDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	// TODO: Add your specialized code here and/or call the base class
	//INT32 nAns = ::MessageBox( NULL, "Return TRUE?", "Test", MB_YESNO);

	/*if( nAns == IDYES)
	{
		return TRUE;
	}
	else 
	{
		SetTitle( "ABCDE");
		return FALSE;
	}*/
		//*/

	CheckSaveTemplateLibrary();

	return CDocument::CanCloseFrame(pFrame);
}

void Ci3UIEditorDoc::PreCloseFrame( CFrameWnd* pFrame)
{
	CDocument::PreCloseFrame( pFrame);
}

BOOL Ci3UIEditorDoc::IsModified( void)
{
	return CDocument::IsModified();
}

void Ci3UIEditorDoc::SetModifiedFlag( BOOL bModified)
{
	CDocument::SetModifiedFlag( bModified);
}

BOOL Ci3UIEditorDoc::SaveModified()
{
	return CDocument::SaveModified();
}

void Ci3UIEditorDoc::SetActiveScene()
{
	//CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	//I3ASSERT( pFrame != NULL);

	//pFrame->SetResolution( m_pProj->getViewWidth(), m_pProj->getViewHeight());
}

void Ci3UIEditorDoc::SetResolution( UINT32 width, UINT32 height)
{
	if( m_pScene != NULL)
	{
		m_pScene->setWidth( width);
		m_pScene->setHeight( height);
	}

//	if( m_pProj->getViewWidth() == width || m_pProj->getViewHeight() == height)
//		return;
//
//	m_pProj->setViewWidth( width);
//	m_pProj->setViewHeight( height);
}

// Ci3UIEditorDoc diagnostics

#ifdef _DEBUG
void Ci3UIEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ci3UIEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void Ci3UIEditorDoc::setCurrentScene( i3UIScene * pScene, const char * pszFileName, bool bAdd)
{
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();

	// register to ui manager
	if( (pScene != NULL) && (bAdd))
	{
		// set info
		I3UI_SCENE_INFO * pInfo =	new I3UI_SCENE_INFO;	//	(I3UI_SCENE_INFO*)i3MemAlloc( sizeof( I3UI_SCENE_INFO));
		
		pInfo->m_pScene = pScene;
		pScene->AddRef();
		
		pInfo->m_bEnabled = true;
		
		pInfo->m_strSceneFile = pszFileName;
		
		// addscene
		i3UIManager * pUI = i3UI::getManager();
		I3ASSERT( pUI != NULL);

		pUI->AddScene( pInfo);
	}

	I3_REF_CHANGE( m_pScene, pScene);

	if( pFrame != NULL)
	{
		pFrame->SetCurrentScene( m_pScene);	//Build Tree
	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_SET, m_pScene, i3UIScene::static_meta());
}

void Ci3UIEditorDoc::setUILibrary( i3UILibrary * pLib)
{
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();

	if( pFrame != NULL)
	{	
		pFrame->SetCurrentLibrary( pLib);	//Build Tree
	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_SET, pLib, i3UILibrary::static_meta());
}

// Ci3UIEditorDoc commands
void Ci3UIEditorDoc::ImportTemplateLibrary(const char * pszLibPath)
{
	MessageBox(NULL, "현재 버그가 있는 기능이며 기능 폐기 가능성이 있습니다.", "알림", MB_OK);

	i3XMLFile File;
	char conv[MAX_PATH];
	i3UIManager * pUI = i3UI::getManager();

	i3UILibrary * pUILib = i3UI::getUILibrary();
	if( pUILib != NULL)
	{
		int rt = MessageBox(NULL, "이미 설정된 Library가 있습니다. 기존 Library와 병합하겠습니까?", "알림", MB_YESNOCANCEL);
		if( rt == IDCANCEL)
			return;

		if( rt == IDYES)
		{
			if( pszLibPath != NULL)
			{
				pUI->MergeTemplateLibrary( pszLibPath);
				pUI->getVirtualTexture()->GetFileFormat()->Flush();
				setUILibrary( i3UI::getUILibrary());
			}
			else
			{
				TCHAR szFilters[] = "i3Uil 파일(*.i3Uil)|*.i3Uil|";

				CFileDialog fileDlg(TRUE, _T("i3Uil"), _T(""), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

				if( fileDlg.DoModal() == IDOK)
				{
					i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), (LPCTSTR)fileDlg.GetPathName(), conv );

					pUI->MergeTemplateLibrary( conv);
					pUI->getVirtualTexture()->GetFileFormat()->Flush();
					setUILibrary( i3UI::getUILibrary());
				}
			}
			return;
		}
	}

	i3UI::setUILibrary( NULL);
	i3UI::CloseVirtualTexture();

	if( pszLibPath != NULL)
	{
		pUI->LoadTemplateLibrary( pszLibPath);
	}
	else
	{
		TCHAR szFilters[] = "i3Uil 파일(*.i3Uil)|*.i3Uil|";

		CFileDialog fileDlg(TRUE, _T("i3Uil"), _T(""), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

		if( fileDlg.DoModal() == IDOK)
		{
			i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), (LPCTSTR)fileDlg.GetPathName(), conv );

			pUI->LoadTemplateLibrary( conv);
		}
	}

	setUILibrary( i3UI::getUILibrary());
}

i3UIScene * Ci3UIEditorDoc::ImportScene( const char * pszPath)
{
	char conv[ MAX_PATH];

	if( pszPath == NULL)
	{
		TCHAR szFilters[] = "i3UIe 파일(*.i3UIe)|*.i3UIe|";

		CFileDialog fileDlg(TRUE, _T("i3UIe"), _T(""), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

		if( fileDlg.DoModal() == IDOK)
		{
			//i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), (LPCTSTR)fileDlg.GetPathName(), conv);

			i3::safe_string_copy( conv, (LPCTSTR) fileDlg.GetPathName(), MAX_PATH );
		}
	}
	else
	{
		i3::safe_string_copy( conv, pszPath, MAX_PATH );
	}

	i3XMLFile File;
	if( File.Load( conv) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "failed load file: %s\n", conv);
		return NULL;
	}

	i3UIScene * pScene = (i3UIScene*)File.getKeyObject();
	I3ASSERT( pScene != NULL);

	// 이미 Scene이 있다면
	// 하위의 Control을 붙여준다.
	INT32 i;

	if( m_pScene != NULL)
	{
		// 검사
		// TemplateLibrary file을 공유하는가.?

		for( i = 0; i < pScene->GetChildCount(); i++)
		{
			if( i3::kind_of<i3UIFrameWnd*>(pScene->GetChildByIndex( i))) //->IsTypeOf( i3UIFrameWnd::static_meta()))
			{
				m_pScene->AddChild( (i3UIFrameWnd*)pScene->GetChildByIndex( i));
				break;
			}
		}
	}
	else
	{
		// Scene이 없으면 그대로 Scene을 추가
		this->setCurrentScene( pScene, conv);
	}

	return m_pScene;
}

bool Ci3UIEditorDoc::ExportScene( i3UIControl * pRoot, const char * pszPath /*= NULL*/)
{
	char conv[ MAX_PATH];

	I3ASSERT( pRoot != NULL);

	if( pszPath == NULL)
	{
		TCHAR szFilters[] = "i3UIe 파일(*.i3UIe)|*.i3UIe|";

		CFileDialog fileDlg( FALSE, _T("i3UIe"), _T(""), OFN_HIDEREADONLY, szFilters);

		if( fileDlg.DoModal() == IDOK)
		{
			//i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), (LPCTSTR)fileDlg.GetPathName(), conv);

			i3::safe_string_copy( conv, (LPCTSTR) fileDlg.GetPathName(), MAX_PATH );
		}
		else
			return false;
	}
	else
	{
		i3::safe_string_copy( conv, pszPath, MAX_PATH );
	}

	// 원래 Parent를 임시로 
	i3GameNode * pParent = pRoot->getParent();

	if( !i3::kind_of<i3UIScene*>(pParent)) //->IsTypeOf( i3UIScene::static_meta()) == FALSE)
	{
		::AfxMessageBox( "Export할 수 없는 Node입니다.");
		return false;
	}

	i3UIScene * pNewScene = i3UIScene::new_object();
	pNewScene->Create( NULL);
	m_pScene->CopyTo( pNewScene, I3_COPY_INSTANCE);

	char fileName[ MAX_PATH];
//	i3String::SplitFileName( conv, fileName, FALSE);
	i3::extract_filetitle(conv, fileName);

	// Parent에서 띄어낸다.
	pParent->RemoveChild( pRoot);

	// Export할 부분을 Scene에 등록
	pNewScene->AddControl( pRoot);
	pNewScene->SetName( fileName);

	i3XMLFile File;
	File.setKeyObject( pNewScene);

	if( File.Save( conv) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "failed save file: %s\n", conv);
		I3_SAFE_RELEASE( pNewScene);

		pParent->AddChild( pRoot);
		return false;
	}

	I3_SAFE_RELEASE( pNewScene);
	pParent->AddChild( pRoot);

	char message[ 260];
	
	i3::snprintf( message, sizeof( message), "%s를 %s.i3UIe에 저장했습니다.", pRoot->GetName(), fileName);
	::AfxMessageBox( message);

	return true;
}

void Ci3UIEditorDoc::CreateTemplateLibrary(void)
{
	i3XMLFile File;
	i3UILibrary * pUILib = i3UI::getUILibrary();
	if( pUILib != NULL)
	{
		if( MessageBox(NULL, "이미 설정된 라이브러리가 있습니다. 새로 만드시겠습니까?", "알림", MB_OKCANCEL) == IDCANCEL)
			return;
	}

	pUILib = i3UILibrary::new_object_ref();
	pUILib->Create();

	i3UI::setUILibrary( pUILib);

	setUILibrary( pUILib);
}

void Ci3UIEditorDoc::AutoSaveTemplateLibrary()
{
	i3UILibrary * pUILib = i3UI::getUILibrary();

	if( pUILib == NULL)
	{
		MessageBox(NULL, "설정된 라이브러리가 없습니다.", "알림", MB_OK);
		return;
	}

	/*
	pUILib->SetName("D:\PB_Working\PB_1.5\PointBlank\Media\UIRe\PointBlankRe.i3UILib");

	i3ResourceFile binFile;
	binFile.setKeyObject( pUILib);
	if( binFile.Save("D:\PB_Working\PB_1.5\PointBlank\Media\UIRe\PointBlankRe.i3UILib"))
	{
		pUILib->setEdited( false);
	}
	*/
}

void Ci3UIEditorDoc::ExportTemplateLibrary(bool isNewFormat, const char * pszLibPath)
{
	i3UILibrary * pUILib = i3UI::getUILibrary();
	if (pUILib == NULL)
	{
		MessageBox(NULL, "설정된 라이브러리가 없습니다.", "알림", MB_OK);
		return;
	}

	if (pszLibPath == NULL)
	{
		TCHAR szFilters[] = "i3Uil 파일(*.i3Uil)|*.i3Uil| i3UILib 파일(*.i3UILib)|*.i3UILib|";

		CFileDialog fileDlg(FALSE, _T("i3Uil|i3UILib"), _T("Noname"), OFN_HIDEREADONLY, szFilters);

		if (fileDlg.DoModal() == IDOK)
		{
			char conv[MAX_PATH];
			i3::make_relative_path(i3ResourceFile::GetWorkingFolder(), (LPCTSTR)fileDlg.GetPathName(), conv);

			i3::stack_string strUniquePath;
			i3UIFilePathMgr::i()->ObtainUniquePath(conv, strUniquePath);

			pUILib->SetName(strUniquePath);		// SetName은 unique스타일로 변경해준다..

			i3UIManager* pUIManager = i3UI::getManager();

			if (isNewFormat)
				pUIManager->SaveTemplateLibrary_TplVersion(pUILib->GetNameString());
			else
				pUIManager->SaveTemplateLibrary_UilVersion(pUILib->GetNameString());

		}
	}
	else
	{
		i3UIManager* pUIManager = i3UI::getManager();

		if (isNewFormat)
			pUIManager->SaveTemplateLibrary_TplVersion(pszLibPath);
		else
			pUIManager->SaveTemplateLibrary_UilVersion(pszLibPath);
	}
}

void Ci3UIEditorDoc::ExportTemplateLibrary_Depricated(const char * pszLibPath)
{
	i3XMLFile File;
	i3UILibrary * pUILib = i3UI::getUILibrary();
	if( pUILib == NULL)
	{
		MessageBox(NULL, "설정된 라이브러리가 없습니다.", "알림", MB_OK);
		return;
	}

	if( pszLibPath == NULL)
	{
		TCHAR szFilters[] = "i3Uil 파일(*.i3Uil)|*.i3Uil| i3UILib 파일(*.i3UILib)|*.i3UILib|";

		CFileDialog fileDlg(FALSE, _T("i3Uil|i3UILib"), _T("Noname"), OFN_HIDEREADONLY, szFilters);

		if( fileDlg.DoModal() == IDOK)
		{
			char conv[MAX_PATH];
			i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), (LPCTSTR)fileDlg.GetPathName(), conv );

			i3::stack_string strUniquePath;
			i3UIFilePathMgr::i()->ObtainUniquePath(conv, strUniquePath);

			pUILib->SetName(strUniquePath);		// SetName은 unique스타일로 변경해준다..

			char ext[MAX_PATH];
			i3::extract_fileext(conv, ext);

			if( i3::generic_is_iequal( ext, "i3UILib") )
			{
				i3ResourceFile binFile;
				binFile.setKeyObject( pUILib);
				if( binFile.Save( (LPCTSTR)fileDlg.GetPathName()))
				{
					I3TRACE("Export Bin: %s Done\n", (LPCTSTR)fileDlg.GetPathName());
					pUILib->setEdited( false);
					::AfxMessageBox( "Export Library OK!");
				}

			}
			else
			{
				File.setKeyObject( pUILib);
				if( File.Save((LPCTSTR)fileDlg.GetPathName()))
				{
					I3TRACE("Export: %s Done\n", (LPCTSTR)fileDlg.GetPathName());
					pUILib->setEdited( false);

					::AfxMessageBox( "Export Library OK!");
				}
			}
		}
	}
	else
	{
		File.setKeyObject( pUILib);
		if( File.Save(pszLibPath))
		{
			I3TRACE("Export: %s Done\n", pszLibPath);
			pUILib->setEdited( false);
		}
	}
}

void Ci3UIEditorDoc::SaveTemplateLibrary(bool isNewFormat)
{
	i3UILibrary * pUILib = i3UI::getUILibrary();
	if (pUILib == NULL)
	{
		MessageBox(NULL, "설정된 라이브러리가 없습니다.", "알림", MB_OK);
		return;
	}

	if (pUILib->hasName())
	{
		i3UIManager* pUIManager = i3UI::getManager();

		if (isNewFormat)
			pUIManager->SaveTemplateLibrary_TplVersion(pUILib->GetNameString());
		else
			pUIManager->SaveTemplateLibrary_UilVersion(pUILib->GetNameString());
	}
	else
	{
		ExportTemplateLibrary(isNewFormat);
	}

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	pMainFrm->UpdateStatusBar(true);

}

void Ci3UIEditorDoc::SaveTemplateLibrary_Deprecated(void)
{
	i3UILibrary * pUILib = i3UI::getUILibrary();
	if( pUILib == NULL)
	{
		MessageBox(NULL, "설정된 라이브러리가 없습니다.", "알림", MB_OK);
		return;
	}

	if( pUILib->hasName())
	{
		i3XMLFile File;

		// Locale 선택 후 Adjust할 경우 UI Template이 깨져서 일단 롤백합니다... 2015.04.06 - sanghun.han

		i3::stack_string strUniquePath;
		i3UIFilePathMgr::i()->ObtainUniquePath(pUILib->GetName(), strUniquePath);

		i3::stack_string strExt;
		i3::extract_fileext(strUniquePath, strExt);

		if (i3::generic_is_iequal(strExt, "i3Uil"))
		{
			i3::change_file_ext(strUniquePath, "i3Uil");			// 확장자 대소문자 구별을 복원시킴..
		}
		
		i3::rc_string currVTexDir = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);
		i3::stack_string strFullPath = currVTexDir;
		strFullPath += '/';
		strFullPath += strUniquePath;

		File.setKeyObject( pUILib);
		if( File.Save(strFullPath.c_str()))
		{
			I3TRACE("Template Library Saved\n");
			pUILib->setEdited( false);
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN,"Couldn't save Template Library to %s\n", strFullPath);
		}
	}
	else
	{
		ExportTemplateLibrary(false);
	}
}

void Ci3UIEditorDoc::CheckSaveTemplateLibrary( void)
{
	i3UILibrary * pLib = i3UI::getUILibrary();

	if( pLib != NULL)
	{
		if( pLib->isEdited())
		{
			if( ::AfxMessageBox( "변경된 Template이 있습니다. 저장하시겠습니까?", MB_YESNO) == IDYES)
			{
				bool isNewTemplateFormat = pLib->IsNewTemplateLibraryFormat();

				SaveTemplateLibrary(isNewTemplateFormat);
			}
		}
	}
}

void Ci3UIEditorDoc::AddFontList(std::string path)
{
	std::string sFontdir;
	char szfontdir[MAX_PATH];
	i3::extract_directoryname(path, szfontdir);

	sFontdir = szfontdir;
	sFontdir.append("\\Font\\*.*");

	WIN32_FIND_DATA findFileData;			// File Data 구조 Structure
	HANDLE hFileHandle = 0;			// File Descriptor Handle

	hFileHandle = FindFirstFile(sFontdir.c_str(), &findFileData);
	if (hFileHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			char ext[MAX_PATH];
			i3::extract_fileext(findFileData.cFileName, ext);
			// 1. 자신/상위 폴더명은 제외
			if (!((strcmp(findFileData.cFileName, ".") == 0) ||
				strcmp(findFileData.cFileName, "..") == 0) &&
				(strcmp(ext, "ttf") == 0 || strcmp(ext, "ttc") == 0)
				)
			{
				std::string sfont;
				sfont = szfontdir;
				sfont.append("\\Font\\");
				sfont.append(findFileData.cFileName);
				if (::GetFileAttributes(sfont.c_str()) != INVALID_FILE_ATTRIBUTES)
				{
					AddFontResourceEx(sfont.c_str(), FR_PRIVATE, 0);
				}

			}
			// 3. 다음 File Handle로 접근
		} while (FindNextFile(hFileHandle, &findFileData));
	}

	FindClose(hFileHandle);
}