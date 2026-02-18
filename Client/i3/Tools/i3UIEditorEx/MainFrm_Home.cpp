#include "stdafx.h"
#include "MainFrm.h"
#include "UIGlobalRes.h"
#include "resource.h"
#include "i3UIEditor.h"

#include "i3UIEditorView.h"

#include "UndoSystem2/UIUndoSystem.h"
#include "i3Base/string/ext/atoi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void CMainFrame::CreateRibbon_Home(void)
{
	CMFCRibbonPanel * pPanel;

	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory( LPCTSTR("Home"), IDB_TB_SM, IDB_TB_LA);

	//	File
	{
		pPanel = pCategory->AddPanel( "File");

		pPanel->Add( new CMFCRibbonButton( ID_FILE_EXPORT, "Export", -1, -1));
		pPanel->Add( new CMFCRibbonButton( ID_FILE_EXPORTLIBRARY, "Export Library", -1, -1));

	}

	// Edit
	{
		pPanel = pCategory->AddPanel( "Edit");

		//CMFCRibbonButton( UINT nID, LPCTSTR lpszText, int nSmallImageIndex=-1, int nLargeImageIndex=-1, BOOL bAlwaysShowDescription=FALSE);
		pPanel->Add( new CMFCRibbonButton( ID_EDIT_COPY, "&Copy", 2, 2));
		pPanel->Add( new CMFCRibbonButton( ID_EDIT_CUT, "Cu&t", 1, 1));
		pPanel->Add( new CMFCRibbonButton( ID_EDIT_PASTE, "&Paste", 0, 0));

		pPanel->Add( new CMFCRibbonSeparator());

		pPanel->Add( new CMFCRibbonButton( ID_EDIT_UNDO, "&Undo", 10, 10));
		pPanel->Add( new CMFCRibbonButton( ID_EDIT_REDO, "&Redo", 10, 10));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// play mode
	{
		pPanel = pCategory->AddPanel( "Mode");

		{
			CMFCRibbonButton * pBtn = new CMFCRibbonButton( ID_PROJECT_RUN, "Preview", -1, 2, TRUE);

			pPanel->Add( pBtn);
		}

		{
			CMFCRibbonButton * pBtn = new CMFCRibbonButton( ID_PROJECT_STOP, "Edit mode", -1, 3, TRUE);

			pPanel->Add( pBtn);
		}
	}

	// Align
	{
		pPanel = pCategory->AddPanel( "Align");

		pPanel->Add( new CMFCRibbonButton( ID_EDIT_ALIGNLEFT, "Align Left", -1, -1));
		pPanel->Add( new CMFCRibbonButton( ID_EDIT_ALIGNRIGHT, "Align Right", -1, -1));
		pPanel->Add( new CMFCRibbonButton( ID_EDIT_ALIGNTOP, "Align Top", -1, -1));
		pPanel->Add( new CMFCRibbonButton( ID_EDIT_ALIGNBOTTOM, "Align Bottom", -1, -1));
		pPanel->Add( new CMFCRibbonButton( ID_EDIT_ALIGNCENTER, "Align Center", -1, -1));
		//pPanel->Add( new CMFCRibbonButton( ID_EDIT_ALIGNHORZCENTER, "Align Horizontal Center", -1, -1));
		//pPanel->Add( new CMFCRibbonButton( ID_EDIT_ALIGNVERTCENTER, "Align Vertical Center", -1, -1));

		pPanel->Add( new CMFCRibbonSeparator());

		pPanel->Add( new CMFCRibbonButton( ID_EDIT_RELOCATIONHORZ, "Horizontal Cascade", -1, -1));
		pPanel->Add( new CMFCRibbonButton( ID_EDIT_RELOCATIONVERT, "Vertical Cascade", -1, -1));

		pPanel->Add( new CMFCRibbonSeparator());

		pPanel->Add( new CMFCRibbonButton( ID_EDIT_SAMEWIDTH, "Resize with the same width", -1, -1));
		pPanel->Add( new CMFCRibbonButton( ID_EDIT_SAMEHEIGHT, "Resize with the same height", -1, -1));

		pPanel->Add( new CMFCRibbonSeparator());

		m_pEditSnapSize = new CMFCRibbonEdit( ID_EDIT_SNAPSIZE, 60, "Snap size");
		m_pEditSnapSize->SetEditText( "8");
		pPanel->Add( m_pEditSnapSize);
	}

	
}

void CMainFrame::OnExport( void)
{
	CView* pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			Ci3UIEditorDoc * pDoc = pEView->GetDocument();
			if( pDoc != NULL)
			{
				i3ResourceFile File;
				char szPath[MAX_PATH] = {0,};

				i3UIScene * pScene = pDoc->getCurrentScene();

				if( pScene != NULL)
				{
					TCHAR szFilters[] = "i3UIs ÆÄÀÏ(*.i3UIs)|*.i3UIs|";

					CFileDialog fileDlg( FALSE, _T("i3UIs"), LPCTSTR( pScene->GetName()), OFN_HIDEREADONLY, szFilters);

					if( fileDlg.DoModal() == IDOK)
					{
						i3::safe_string_copy( szPath, (LPCTSTR) fileDlg.GetPathName(), MAX_PATH );
					}

					File.setKeyObject( pScene);

					if( File.Save( szPath) == STREAM_ERR )
					{
						I3PRINTLOG(I3LOG_WARN, "failed save file: %s\n", szPath);
					}
					else
					{
						::AfxMessageBox( "Export OK!");
					}
				}
			}
		}
	}

	
}

void CMainFrame::OnUpdateExportUI( CCmdUI * pCmdUI)
{
	bool bEnable = false;

	CView* pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			Ci3UIEditorDoc * pDoc = pEView->GetDocument();
			if( pDoc != NULL)
			{
				bEnable = true;
			}
		}
	}

	if( bEnable)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
	
}

void CMainFrame::OnExportLib( void)
{
	CView* pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			Ci3UIEditorDoc * pDoc = pEView->GetDocument();

			bool isNewFormat = false;

			i3UILibrary* pUILib = i3UI::getUILibrary();

			if (pUILib)
			{
				isNewFormat = pUILib->IsNewTemplateLibraryFormat();
			}
			
			pDoc->ExportTemplateLibrary(isNewFormat);
		}
	}
}

void CMainFrame::OnUpdateExportLIbUI( CCmdUI * pCmdUI)
{
	if( i3UI::getUILibrary() != NULL)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnCopy( void)
{
	CView* pView = GetActiveView();

	if( pView != NULL && GetFocus() == pView)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			pEView->OnCopy();
		}
		/*else if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template* pTView = (Ci3UIEditorView_Template*)pView;
			pTView->OnCopy();
		}*/
	}	
}

void CMainFrame::OnUpdateCopyUI( CCmdUI * pCmdUI)
{
	CView* pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			if( pEView->getSelectedObjCount() > 0)
				pCmdUI->Enable();
			else
				pCmdUI->Enable( FALSE);
		}
		else
			pCmdUI->Enable( FALSE);
		/*if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template* pTView = (Ci3UIEditorView_Template*)pView;
			if( pTView->getSelectedObjCount() > 0)
				pCmdUI->Enable();
			else
				pCmdUI->Enable( FALSE);
		}*/
	}
}

void CMainFrame::OnCut( void)
{
	CView* pView = GetActiveView();

	if( pView != NULL && GetFocus() == pView)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			pEView->OnCut();
		}
	}	
}

void CMainFrame::OnUpdateCutUI( CCmdUI * pCmdUI)
{
	CView* pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			if( pEView->getSelectedObjCount() > 0)
				pCmdUI->Enable();
			else
				pCmdUI->Enable( FALSE);
		}
		else
			pCmdUI->Enable( FALSE);
	}
}

void CMainFrame::OnPaste( void)
{
	CView* pView = GetActiveView();

	if( pView != NULL && GetFocus() == pView)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			pEView->OnPaste();
		}
	}
}

void CMainFrame::OnUpdatePasteUI( CCmdUI * pCmdUI)
{
	CView* pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			if( pEView->GetCopiedControlCount() > 0)
				pCmdUI->Enable();
			else
				pCmdUI->Enable( FALSE);
		}
		else
			pCmdUI->Enable( FALSE);
	}
}

void CMainFrame::OnUndo( void)
{
	CView* pView = GetActiveView();
	
	if( pView != NULL && GetFocus() == pView)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;

			pEView->UnSelectAll();

			UIUndo::Undo();
		}
	}
}

void CMainFrame::OnRedo( void)
{
	CView* pView = GetActiveView();

	if( pView != NULL && GetFocus() == pView)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;

			pEView->UnSelectAll();

			UIUndo::Redo();
		}
	}
}

void CMainFrame::OnUpdateUndoButton( CCmdUI *pCmdUI)
{
	pCmdUI->Enable( UIUndo::GetUndoCount() > 0);
}

void CMainFrame::OnUpdateRedoButton( CCmdUI *pCmdUI)
{
	pCmdUI->Enable( UIUndo::GetRedoCount() > 0);
}

void CMainFrame::OnUpdateAlignButton( CCmdUI * pCmdUI)
{
	CView* pView = GetActiveView();

	if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
	{
		Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;

		if( pEView->getSelectedObjCount() > 1)
			pCmdUI->Enable();
		else
			pCmdUI->Enable( FALSE);
	}
	else
		pCmdUI->Enable( FALSE);
}

void CMainFrame::OnUpdateAlignCenterButton( CCmdUI * pCmdUI)
{
	CView* pView = GetActiveView();

	if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
	{
		Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;

		if( pEView->getSelectedObjCount() > 0)
			pCmdUI->Enable();
		else
			pCmdUI->Enable( FALSE);
	}
	else
		pCmdUI->Enable( FALSE);
}



void CMainFrame::OnAlignLeft( void)
{
	CView* pView = GetActiveView();

	if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
	{
		Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
		pEView->AlignLeft( pEView->getSelectedItemList());
	}
}

void CMainFrame::OnAlignRight( void)
{
	CView* pView = GetActiveView();

	if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
	{
		Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
		pEView->AlignRight( pEView->getSelectedItemList());
	}
}

void CMainFrame::OnAlignTop( void)
{
	CView* pView = GetActiveView();

	if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
	{
		Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
		pEView->AlignTop( pEView->getSelectedItemList());
	}
}

void CMainFrame::OnAlignBottom( void)
{
	CView* pView = GetActiveView();

	if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
	{
		Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
		pEView->AlignBottom( pEView->getSelectedItemList());
	}
}

void CMainFrame::OnAlignCenter( void)
{
	// New
	CView* pView = GetActiveView();

	if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
	{
		Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
		pEView->AlignCenter( pEView->getSelectedItemList());
	}
}

void CMainFrame::OnCascadeHorz( void)
{
	// New
	CView* pView = GetActiveView();

	if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
	{
		Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
		pEView->CascadeHorz( pEView->getSelectedItemList());
	}
}

void CMainFrame::OnCascadeVert( void)
{
	// New
	CView* pView = GetActiveView();

	if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
	{
		Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
		pEView->CascadeVert( pEView->getSelectedItemList());
	}
}


void CMainFrame::OnSnapSize( void)
{
	if( m_pEditSnapSize != NULL)
	{
		INT32 size = i3::atoi( (const char*) m_pEditSnapSize->GetEditText().GetString());

		if( size < 1)
			size = 1;

		CView* pView = GetActiveView();

		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			pEView->SetSnapSize( size);
		}
	}
}

void CMainFrame::OnUpdateSnapSize( CCmdUI * pCmdUI)
{
	char conv[ 32];

	if( m_pEditSnapSize != NULL)
	{
		CView* pView = GetActiveView();

		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			::itoa( pEView->GetSnapSize(), conv, 0);

			m_pEditSnapSize->SetEditText( conv);
		}
		else
			m_pEditSnapSize->SetEditText( "8");
	}
}