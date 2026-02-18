#include "stdafx.h"
#include "MainFrm.h"

void CMainFrame::OnFont_Style()
{
	if( m_pCBFont != NULL)
	{
		i3List list;

		m_Pane_DesignView.getSelectedShape( &list);

		int idx = m_pCBFont->GetCurSel();

		INT32 i;
		i3GICShape * pShape;

		if( idx != -1)
		{
			for( i = 0; i < list.GetCount(); i++)
			{
				pShape = (i3GICShape*) list.Items[ i];

				if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta() ) )
				{
					((i3GICShapeTextRect*)pShape)->setFont( (const char *) m_pCBFont->GetItem( idx));
				}
			}

			m_Pane_DesignView.redrawView();
		}
	}
}

static int s_EnumFontCallBackProc( const LOGFONT *lplf, const TEXTMETRIC *lptm, DWORD dwType, LPARAM lpData)
{
	if( (lplf != NULL) && (lptm != NULL))
	{
		return 1;
	}
	else
		return 0;
}



void CMainFrame::OnUpdateFont_Style( CCmdUI * pCmdUI)
{
	i3List list;
	
	m_Pane_DesignView.getSelectedShape( &list);

	if( list.GetCount() > 0)
	{
		INT32 i;
		i3GICShape * pShape = NULL;

		for( i = 0; i < list.GetCount(); i++)
		{
			pShape = (i3GICShape*) list.Items[ i];

			if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta() ) )
			{
				break;
			}

			pShape = NULL;
		}

		if( pShape != NULL)
		{
			const char * pszFont = ((i3GICShapeTextRect*)pShape)->getFont();
			pCmdUI->SetText( pszFont);

		//	pCmdUI->m_nIndex = ::EnumFonts( m_Pane_DesignView.getDrawDC(), pszFont, NULL, NULL);//(FONTENUMPROC )s_EnumFontCallBackProc, NULL );
			return;
		}
	}
	
}

void CMainFrame::OnFont_Size()
{
	if( m_pCBFontSize != NULL)
	{
		i3List list;

		m_Pane_DesignView.getSelectedShape( &list);

		//int idx = m_pCBFontSize->GetCurSel();

		//if( idx != -1)
		{	
			INT32 iSize = i3String::ToInt( (const char *) m_pCBFontSize->GetEditText().GetString() );

			INT32 i;
			i3GICShape * pShape;

			for( i = 0; i < list.GetCount(); i++)
			{
				pShape = (i3GICShape*) list.Items[ i];

				if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta() ) )
				{
					((i3GICShapeTextRect*)pShape)->setFontSize( (UINT32) iSize );
				}
			}

			m_Pane_DesignView.redrawView();
		}
	}
}

void CMainFrame::OnUpdateFont_Size( CCmdUI * pCmdUI)
{
	i3List list;
	
	m_Pane_DesignView.getSelectedShape( &list);

	if( list.GetCount() > 0)
	{
		INT32 i;
		i3GICShape * pShape = NULL;

		for( i = 0; i < list.GetCount(); i++)
		{
			pShape = (i3GICShape*) list.Items[ i];

			if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta() ) )
			{
				break;
			}

			pShape = NULL;
		}

		if( pShape != NULL)
		{
			UINT32 uSize = ((i3GICShapeTextRect*)pShape)->getFontSize();

			char szTemp[16];

			i3String::ftoa( (REAL32) uSize, szTemp, COUNTOF( szTemp));
			pCmdUI->SetText( szTemp);

		//	pCmdUI->m_nIndex = ::EnumFonts( m_Pane_DesignView.getDrawDC(), pszFont, NULL, NULL);//(FONTENUMPROC )s_EnumFontCallBackProc, NULL );
			return;
		}
	}
}

void CMainFrame::OnFont_Underline()
{
	INT32 i;
	i3GICShape * pShape;
	i3List list;

	m_Pane_DesignView.getSelectedShape( &list);

	for( i = 0; i < list.GetCount(); i++)
	{
		pShape = (i3GICShape*) list.Items[ i];

		if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta() ) )
		{
			((i3GICShapeTextRect*)pShape)->setUnderLine( !m_bUnderline);
		}
	}

	m_bUnderline = !m_bUnderline;

	m_Pane_DesignView.redrawView();
}

void CMainFrame::OnUpdateFont_Underline( CCmdUI * pCmdUI)
{
	INT32 i;
	i3List list;
	i3GICShape * pShape;

	bool bEnable = false;

	m_Pane_DesignView.getSelectedShape( &list);

	for( i = 0; i < list.GetCount(); i++)
	{
		pShape = (i3GICShape*) list.Items[ i];

		if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta()))
		{
			m_bUnderline = ((i3GICShapeTextRect*)pShape)->isUnderLine();
			pCmdUI->SetCheck( m_bUnderline);
			bEnable = true;
			break;
		}
	}

	pCmdUI->Enable( (BOOL) bEnable);
}

void CMainFrame::OnFont_Italic()
{
	INT32 i;
	i3GICShape * pShape;
	i3List list;

	m_Pane_DesignView.getSelectedShape( &list);

	for( i = 0; i < list.GetCount(); i++)
	{
		pShape = (i3GICShape*) list.Items[ i];

		if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta() ) )
		{
			((i3GICShapeTextRect*)pShape)->setItalic( !m_bItalic);
		}
	}

	m_bItalic = !m_bItalic;

	m_Pane_DesignView.redrawView();
}

void CMainFrame::OnUpdateFont_Italic( CCmdUI * pCmdUI)
{
	INT32 i;
	i3List list;
	i3GICShape * pShape;

	bool bEnable = false;

	m_Pane_DesignView.getSelectedShape( &list);

	for( i = 0; i < list.GetCount(); i++)
	{
		pShape = (i3GICShape*) list.Items[ i];

		if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta()))
		{
			m_bItalic = ((i3GICShapeTextRect*)pShape)->isItalic();
			pCmdUI->SetCheck( m_bItalic);
			bEnable = true;
			break;
		}
	}

	pCmdUI->Enable( (BOOL) bEnable);
}

void CMainFrame::OnFont_Color()
{
	if( m_pBtnFontColor != NULL)
	{
		INT32 i;
		i3List list;
		i3GICShape * pShape;

		m_Pane_DesignView.getSelectedShape( &list);

		for( i = 0; i < list.GetCount(); i++)
		{
			pShape = (i3GICShape*) list.Items[ i];

			if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta()))
			{
				((i3GICShapeTextRect*) pShape)->setFontColor( m_pBtnFontColor->GetColor());
			}
		}
	}

	m_Pane_DesignView.redrawView();
}

void CMainFrame::OnUpdateFont_Color( CCmdUI * pCmdUI)
{
	INT32 i;
	i3List list;
	i3GICShape * pShape;

	bool bEnable = false;

	m_Pane_DesignView.getSelectedShape( &list);

	for( i = 0; i < list.GetCount(); i++)
	{
		pShape = (i3GICShape*) list.Items[ i];

		if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta()))
		{	
			bEnable = true;
			break;
		}
	}

	pCmdUI->Enable( (BOOL) bEnable);
}

void CMainFrame::OnFont_BkColor()
{
	if( m_pBtnFontBkColor != NULL)
	{
		INT32 i;
		i3List list;
		i3GICShape * pShape;

		m_Pane_DesignView.getSelectedShape( &list);

		for( i = 0; i < list.GetCount(); i++)
		{
			pShape = (i3GICShape*) list.Items[ i];

			if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta()))
			{
				((i3GICShapeTextRect*) pShape)->setFontBkColor( m_pBtnFontBkColor->GetColor());
			}
		}
	}

	m_Pane_DesignView.redrawView();
}

void CMainFrame::OnUpdateFont_BkColor( CCmdUI * pCmdUI)
{
	INT32 i;
	i3List list;
	i3GICShape * pShape;

	bool bEnable = false;

	m_Pane_DesignView.getSelectedShape( &list);

	for( i = 0; i < list.GetCount(); i++)
	{
		pShape = (i3GICShape*) list.Items[ i];

		if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta()))
		{	
			bEnable = true;
			break;
		}
	}

	pCmdUI->Enable( (BOOL) bEnable);
}

void CMainFrame::OnFont_Stretch()
{
	INT32 i;
	i3List list;
	i3GICShape * pShape;

	m_Pane_DesignView.getSelectedShape( &list);

	for( i = 0; i < list.GetCount(); i++)
	{
		pShape = (i3GICShape*) list.Items[ i];

		if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta()))
		{
			((i3GICShapeTextRect*) pShape)->setStretchMode( !m_bStretch);
		}
	}

	m_bStretch = !m_bStretch;

	m_Pane_DesignView.redrawView();
}

void CMainFrame::OnUpdateFont_Stretch( CCmdUI * pCmdUI)
{
	INT32 i;
	i3List list;
	i3GICShape * pShape;

	bool bEnable = false;

	m_Pane_DesignView.getSelectedShape( &list);

	for( i = 0; i < list.GetCount(); i++)
	{
		pShape = (i3GICShape*) list.Items[ i];

		if( pShape->IsTypeOf( i3GICShapeTextRect::GetClassMeta()))
		{
			m_bStretch = ((i3GICShapeTextRect*)pShape)->isStretchMode();
			pCmdUI->SetCheck( m_bStretch);
			bEnable = true;
			break;
		}
	}

	pCmdUI->Enable( (BOOL) bEnable);
}

void CMainFrame::OnView_Grid( void)
{
	m_Pane_DesignView.setGridEnable(m_Pane_DesignView.isGrid() ? false : true);
	m_Pane_DesignView.redrawView();
}

void CMainFrame::OnUpdateView_Grid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Pane_DesignView.isGrid());
}

void CMainFrame::OnView_GridSize()
{
	CMFCRibbonBar* pRibbon = ((CMainFrame*) GetTopLevelFrame())->GetRibbonBar();
	I3ASSERT( pRibbon);

	CMFCRibbonBaseElement * pBase = pRibbon->FindByID( IDC_SPIN_GRIDSIZE);

	CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pBase);
	
	CString strSize = pEdit->GetEditText();

	INT32 iSize = i3String::ToInt( strSize.GetBuffer());

	if( iSize > 0)
	{
		m_Pane_DesignView.setGridSize( iSize);
		m_Pane_DesignView.setGridEnable( true);
		m_Pane_DesignView.redrawView();
	}
	else
	{
		m_Pane_DesignView.setGridEnable( false);
	}
}

void CMainFrame::OnView_LinkSnap()
{
	m_Pane_DesignView.setLinkSnapEnable( m_Pane_DesignView.isLinkSnap() ? false : true);
}

void CMainFrame::OnUpdateView_LinkSnap( CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck( m_Pane_DesignView.isLinkSnap());
}

void CMainFrame::OnView_GridSnap()
{
	m_Pane_DesignView.setGridSnapEnable( m_Pane_DesignView.isGridSnap() ? false : true);
}

void CMainFrame::OnUpdateView_GridSnap( CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck( m_Pane_DesignView.isGridSnap());
}


void CMainFrame::OnTools_AlignLeft()
{
	m_Pane_DesignView.getDiagramContext()->AlignToLeft();
}

void CMainFrame::OnUpdateTools_AlignLeft( CCmdUI * pCmdUI)
{
	i3List list;
	pCmdUI->Enable( (BOOL)(m_Pane_DesignView.getSelectedShape( &list) > 0) );
}

void CMainFrame::OnTools_AlignCenter()
{
	m_Pane_DesignView.getDiagramContext()->AlignToCenter();
}

void CMainFrame::OnUpdateTools_AlignCenter( CCmdUI * pCmdUI)
{
	i3List list;
	pCmdUI->Enable( (BOOL)(m_Pane_DesignView.getSelectedShape( &list) > 0) );
}

void CMainFrame::OnTools_AlignRight()
{
	m_Pane_DesignView.getDiagramContext()->AlignToRight();
}

void CMainFrame::OnUpdateTools_AlignRight( CCmdUI * pCmdUI)
{
	i3List list;
	pCmdUI->Enable( (BOOL)(m_Pane_DesignView.getSelectedShape( &list) > 0) );
}

void CMainFrame::OnTools_AlignTop()
{
	m_Pane_DesignView.getDiagramContext()->AlignToTop();
}

void CMainFrame::OnUpdateTools_AlignTop( CCmdUI * pCmdUI)
{
	i3List list;

	pCmdUI->Enable( (BOOL)(m_Pane_DesignView.getSelectedShape( &list) > 0) );
}

void CMainFrame::OnTools_AlignMiddle()
{
	m_Pane_DesignView.getDiagramContext()->AlignToMiddle();
}

void CMainFrame::OnUpdateTools_AlignMiddle( CCmdUI * pCmdUI)
{
	i3List list;

	pCmdUI->Enable( (BOOL)(m_Pane_DesignView.getSelectedShape( &list) > 0) );
}

void CMainFrame::OnTools_AlignBottom()
{
	m_Pane_DesignView.getDiagramContext()->AlignToBottom();
}

void CMainFrame::OnUpdateTools_AlignBottom( CCmdUI * pCmdUI)
{
	i3List list;

	pCmdUI->Enable( (BOOL)(m_Pane_DesignView.getSelectedShape( &list) > 0) );
}

void CMainFrame::OnMode_Select()
{
	m_Pane_DesignView.getDiagramContext()->setMode( GIC_MODE_DESIGN);
	m_Pane_DesignView.RedrawWindow();
}

void CMainFrame::OnUpdateMode_Select( CCmdUI * pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( (BOOL)(m_Pane_DesignView.getDiagramContext()->getMode() == GIC_MODE_DESIGN) );
}

void CMainFrame::OnMode_Link()
{
	m_Pane_DesignView.getDiagramContext()->setMode( GIC_MODE_DESIGNTREENODE);
	m_Pane_DesignView.RedrawWindow();
}

void CMainFrame::OnUpdateMode_Link( CCmdUI * pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( (BOOL)(m_Pane_DesignView.getDiagramContext()->getMode() == GIC_MODE_DESIGNTREENODE) );
}