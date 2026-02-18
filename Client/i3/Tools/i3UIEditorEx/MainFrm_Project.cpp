#include "stdafx.h"
#include "MainFrm.h"
#include "UIGlobalRes.h"
#include "resource.h"
#include "i3UIEditor.h"

#include	"GameStringTable.h"
#include "i3Base/string/ext/make_relative_path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void CMainFrame::CreateRibbon_ProjectCategory(void)
{
	// Add "Project" category
	CMFCRibbonCategory* pCategoryHome = m_wndRibbonBar.AddCategory( LPCTSTR("Project"), IDB_TB_SM, IDB_TB_LA);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 디렉토리
	CMFCRibbonPanel * pPanelDirectory = pCategoryHome->AddPanel( "Directory");

	CMFCRibbonButton * pBtnWorkingFolder = new CMFCRibbonButton( ID_PROJECT_WORKINGFOLDER, "Working Folder", 1, 1, TRUE);
	pPanelDirectory->Add( pBtnWorkingFolder);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// module
	CMFCRibbonPanel * pPanelOption = pCategoryHome->AddPanel( "Option");

	CMFCRibbonButton * pBtnModule = new CMFCRibbonButton(ID_OPTION_EDITOROPTION, "Module", 1, 0, TRUE);
	pPanelOption->Add( pBtnModule);

	//////////////////////////////////////////////////////////
	// Clear Color
	CMFCRibbonPanel * pPanelBGColor = pCategoryHome->AddPanel( "Color Option");
	{
		m_pCOL_ClearColor = new i3TDKRibbonColor( ID_CLEARCOLOR, "Clear Color", TRUE, -1,-1);
		pPanelBGColor->Add( m_pCOL_ClearColor);

		_LoadClearColor();

		m_pCOL_ClearColor->setColor( &m_ClearColor);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 해상도 Panel
	CMFCRibbonPanel * pPanelScreenConfig = pCategoryHome->AddPanel( "해상도");

	CMFCRibbonButton* pBtnResolution = new CMFCRibbonButton( (UINT)-1, "Resolution", 2, 0);
	pBtnResolution->SetMenu( IDR_SCREEN_RESOLUTION, FALSE, FALSE);
	pBtnResolution->SetAlwaysLargeImage();

	pPanelScreenConfig->Add( pBtnResolution);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// virtual Texture Panel
	CMFCRibbonPanel * pPanelVirtualT = pCategoryHome->AddPanel( "Virtual Texture");

	{
		//CMFCRibbonButton * pBtn = new CMFCRibbonButton( ID_PROJECT_ADJUSTVIRTUALTEXTURE, "Adjust Virtual Texture", -1, 1, TRUE);

		//pPanelVirtualT->Add( pBtn);

		//CMFCRibbonButton * pBtn = new CMFCRibbonButton( ID_PROJECT_ADJUSTVTEXLIST, "Adjust VTexList (NewFormat)", -1, 1, TRUE);

		//pPanelVirtualT->Add( pBtn);

	}

	{
		CMFCRibbonButton * pBtn = new CMFCRibbonButton( ID_PROJECT_OPENVIRTUALTEXTURE, "Open Virtual Texture", 3, -1, TRUE);

		pPanelVirtualT->Add( pBtn);
	}

	{
		CMFCRibbonButton * pBtn = new CMFCRibbonButton( ID_PROJECT_CREATEVIRTUALTEXTURE, "Create Virtual Texture", 3, -1, TRUE);

		pPanelVirtualT->Add( pBtn);
	}

	{
		CMFCRibbonButton * pBtn = new CMFCRibbonButton(ID_PROJECT_CREATEVTEXLIST, "Create VTexList (NewFormat)", 3, -1, TRUE);

		pPanelVirtualT->Add(pBtn);
	}


	{
		CMFCRibbonButton * pBtn = new CMFCRibbonButton( ID_PROJECT_MODIFYVIRTUALTEXTURE, "Modify Virtual Texture", 3, -1, TRUE);

		pPanelVirtualT->Add( pBtn);
	}

	{
		pPanelVirtualT->Add( new CMFCRibbonButton( ID_PROJECT_EXTRACTREFTEXTURE, "Extract All Ref. Textures", -1, -1, TRUE));
	}

	///////////////////////////////////////////////////
	// Script 
	CMFCRibbonPanel * pPanelTest = pCategoryHome->AddPanel( "Script");

	{
		CMFCRibbonButton * pBtnTest = new CMFCRibbonButton( ID_PROJECT_SAVESCRIPT, "Save", -1, 1, TRUE);

		pPanelTest->Add( pBtnTest);
	}

	{
		CMFCRibbonButton * pBtnTest = new CMFCRibbonButton( ID_PROJECT_COMPILESCRIPT, "Compile", 1, 1, FALSE);
		pBtnTest->SetToolTipText( "Compile");	// Ugly Tooltip.. TT
		pBtnTest->SetDescription( "Compile files (Ctrl+F7)");
		pPanelTest->Add( pBtnTest);
	}

	//////////////////////////////////////////////////////////
	// select scene
	CMFCRibbonPanel * pPanelScene = pCategoryHome->AddPanel( "Scene");
	{
		CMFCRibbonButton * pBtn = new CMFCRibbonButton( ID_PROJECT_SELECTSCENEFROMCLIENT, "Select Scene", -1, 3, TRUE);

		pPanelScene->Add( pBtn);
	}

	//////////////////////////////////////////////////////////
	// select nation
	CMFCRibbonPanel * pPanelNation = pCategoryHome->AddPanel( "Nation");
	{
		m_pNationControl = new CMFCRibbonButton( (UINT)-1, "Dev", 2, 0);
		m_pNationControl->SetMenu( IDR_NATION, FALSE, FALSE);
		m_pNationControl->SetAlwaysLargeImage();

		pPanelNation->Add( m_pNationControl);

		CMFCRibbonLabel * pCurrentNation = new CMFCRibbonLabel("기본 Locale은 Media 입니다.");
		
		pPanelNation->Add( pCurrentNation);
	}


}

void CMainFrame::OnProjectWorkingfolder()
{
	i3::stack_string strPrevWorkingDirectory = i3ResourceFile::GetWorkingFolder();


	// TODO: Add your command handler code here
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, LPCTSTR(strPrevWorkingDirectory.c_str()), BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT, 0, 0, LPCTSTR(strPrevWorkingDirectory.c_str())))
	{
		SetWorkingFolder( dlg.GetSelectedFolderPath());
	
		//실질적으로는 안바뀐 경우, 그냥 리턴
		// 아래코드들 어이가 없는 구현들로 추정되어 일단 모두 주석...
		/*
		i3::stack_string strModifiedWorkingDirectory = i3ResourceFile::GetWorkingFolder();

		if( i3::generic_is_iequal(strPrevWorkingDirectory, strModifiedWorkingDirectory) )
			return;

		//User가 작업 중에 Working Folder를 바꾸는 경우이므로
		//Template Library, Virtual Texture, Scene Script의 이름도 상대경로로 같이 바꿔준다.
		/*
		char szPrevPath[MAX_PATH];
		char conv[MAX_PATH];
		
		//Template Library
		
		i3UILibrary* pLib = i3UI::getUILibrary();
		
		if( pLib != NULL)
		{
			if( pLib->hasName())
			{
				i3::snprintf( szPrevPath, MAX_PATH, "%s/%s", szPrevWorkingDirectory, pLib->GetName());
				i3::make_relative_path( m_WorkingDirectory, szPrevPath, conv  );
				pLib->SetName( conv);
			}

			const char * pszScriptPath = pLib->getScriptFile();
			if( pszScriptPath != NULL)
			{
				i3::snprintf( szPrevPath, MAX_PATH, "%s/%s", szPrevWorkingDirectory, pszScriptPath);
				i3::make_relative_path( m_WorkingDirectory, szPrevPath, conv  );
				pLib->setScriptFile( conv);
			}
		}
		*/

		//Virtaul Texture
		/*
		i3VirtualTexture* pVTex = i3UI::getVirtualTexture();
		if( pVTex != NULL)
		{
			if( pVTex->hasName())
			{
				i3::snprintf( szPrevPath, MAX_PATH, "%s/%s", szPrevWorkingDirectory, pVTex->GetName());
				i3::make_relative_path( m_WorkingDirectory, szPrevPath, conv );
				pVTex->SetName( conv);

				// Virtual Texture Ref Texture의 Working Directory 수정
				for( INT32 i=0; i<pVTex->getRefTextureCount(); i++ ) {
					I3VT_TEXINFO* pTexInfo = pVTex->getRefTexture( i );
					if( pTexInfo != NULL ) {
						i3::snprintf( szPrevPath, MAX_PATH, "%s/%s", szPrevWorkingDirectory, pTexInfo->m_strName );
						i3::make_relative_path( m_WorkingDirectory, szPrevPath, conv );
						pTexInfo->m_strName = conv;
					}
				}
			}
		}
		*/

		//Scene Script
		/*
		Ci3UIEditorDoc* pDoc = (Ci3UIEditorDoc*)GetActiveDocument();
		
		if( pDoc != NULL)
		{	
			i3UIScene* pScene = pDoc->getCurrentScene();
			if( pScene != NULL)
			{
				const char * pszScriptPath = pScene->getScriptFile();
				if( pszScriptPath != NULL)
				{
					i3::snprintf( szPrevPath, MAX_PATH, "%s/%s", szPrevWorkingDirectory, pszScriptPath);
					i3::make_relative_path( m_WorkingDirectory, szPrevPath, conv );
					pScene->setScriptFile( conv);
				}
			}
		}
		*/
	}
}

void CMainFrame::SetWorkingFolder( const char * pszWorkingFolder)
{
	i3ResourceFile::SetWorkingFolder( const_cast<char*>(pszWorkingFolder));

	theApp.WriteProfileString( LPCTSTR("Settings"), LPCTSTR("Working Path"), LPCTSTR(pszWorkingFolder));

	// GAME STRING TABLE
	//theGameString.Init( pszWorkingFolder );
}

const char * CMainFrame::getWorkingFolder(void) 
{ 
	return i3ResourceFile::GetWorkingFolder();
}


void CMainFrame::SetVirtualTexturePath( const i3::rc_string& strPath)
{
	m_strVTexPath = strPath;

	theApp.WriteProfileString( LPCTSTR("Settings"), LPCTSTR("VirtualTexture Path"), LPCTSTR(m_strVTexPath.c_str()));
}

