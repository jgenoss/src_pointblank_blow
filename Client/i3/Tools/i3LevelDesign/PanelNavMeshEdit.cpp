#include "pch.h"
#include "i3LevelDesign.h"
#include "PanelNavMeshEdit.h"

#include "MainFrm.h"


IMPLEMENT_DYNAMIC(CPanelNavMeshEdit, i3TDKDialogBase)

CPanelNavMeshEdit::CPanelNavMeshEdit(CWnd* pParent /*=NULL*/)
: i3TDKDialogBase(CPanelNavMeshEdit::IDD, pParent)
{
	m_fMinVScale = 0.1f;
	m_fMaxVScale = 3.0f;
}

CPanelNavMeshEdit::~CPanelNavMeshEdit()
{
}

void	CPanelNavMeshEdit::DoDataExchange( CDataExchange* pDX )
{
	i3TDKDialogBase::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_NAVMESH_RADIO_VERTEX, m_radioVertex );
	DDX_Control( pDX, IDC_NAVMESH_RADIO_POLY,	m_radioPoly );

	DDX_Control( pDX, IDC_NAVMESH_EDIT_ADD,				m_btnEditAdd );
	DDX_Control( pDX, IDC_NAVMESH_EDIT_DEL,				m_btnEditDel );
	DDX_Control( pDX, IDC_NAVMESH_EDIT_CREATEPOLY,		m_btnCreatePoly );
	DDX_Control( pDX, IDC_NAVMESH_EDIT_CLEARNOUSE,		m_btnClearNoUse );	
	DDX_Control( pDX, IDC_NAVMESH_EDIT_INVERTNORMAL,	m_btnInvertNormal );	
	DDX_Control( pDX, IDC_NAVMESH_EDIT_CUTPOLY,			m_btnCutPoly );	

	DDX_Control( pDX, IDC_NAVMESH_SLIDE_VERTEXSCALE,	m_sliderVScale );	
	DDX_Control( pDX, IDC_NAVMESH_TEXT_VERTEXSCALE,		m_txtVScale);
}


BEGIN_MESSAGE_MAP(CPanelNavMeshEdit, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_NAVMESH_RADIO_VERTEX, &CPanelNavMeshEdit::OnBnClickedNavmeshRadioVertex)
	ON_BN_CLICKED(IDC_NAVMESH_RADIO_POLY, &CPanelNavMeshEdit::OnBnClickedNavmeshRadioPoly)

	ON_BN_CLICKED(IDC_NAVMESH_EDIT_ADD, &CPanelNavMeshEdit::OnBnClickedNavmeshEditAdd)
	ON_BN_CLICKED(IDC_NAVMESH_EDIT_DEL, &CPanelNavMeshEdit::OnBnClickedNavmeshEditDel)
	ON_BN_CLICKED(IDC_NAVMESH_EDIT_CREATEPOLY,		&CPanelNavMeshEdit::OnBnClickedNavmeshEditCreatePoly)
	ON_BN_CLICKED(IDC_NAVMESH_EDIT_CUTPOLY,			&CPanelNavMeshEdit::OnBnClickedNavmeshEditCutPoly)
	ON_BN_CLICKED(IDC_NAVMESH_EDIT_CLEARNOUSE,		&CPanelNavMeshEdit::OnBnClickedNavmeshEditClearNoUse)
	ON_BN_CLICKED(IDC_NAVMESH_EDIT_INVERTNORMAL,	&CPanelNavMeshEdit::OnBnClickedNavmeshEditInvertNormal)

	ON_WM_HSCROLL()
END_MESSAGE_MAP()


BOOL	CPanelNavMeshEdit::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();
	return TRUE;
}

void	CPanelNavMeshEdit::LevelEditModeChanged( LEVEL_EDIT_MODE editMode )
{
	// radio button state
	{
		switch( editMode )
		{
		case	LEM_NORMAL:
				m_radioVertex.SetCheck( BST_UNCHECKED );
				m_radioPoly.SetCheck( BST_UNCHECKED );
				break;

		case	LEM_NAVMESH_POINT_ADD:
		case	LEM_NAVMESH_POINT:
				m_radioVertex.SetCheck( BST_CHECKED );
				m_radioPoly.SetCheck( BST_UNCHECKED );
				break;

		case	LEM_NAVMESH_POLYGON:
				m_radioVertex.SetCheck( BST_UNCHECKED );
				m_radioPoly.SetCheck( BST_CHECKED );
				break;
		}
	}	

	// button enable
	{
		m_radioVertex.EnableWindow( LEM_NORMAL != editMode );
		m_radioPoly.EnableWindow( LEM_NORMAL != editMode );

		m_btnEditAdd.EnableWindow( FALSE );
		m_btnEditDel.EnableWindow( FALSE );
		m_btnCreatePoly.EnableWindow( FALSE );
		m_btnClearNoUse.EnableWindow( FALSE );
		m_btnInvertNormal.EnableWindow( FALSE );
		m_btnCutPoly.EnableWindow( FALSE );

		switch( editMode )
		{
		case	LEM_NAVMESH_POINT:
				m_btnEditAdd.EnableWindow( TRUE );
				m_btnEditDel.EnableWindow( TRUE );
				m_btnCreatePoly.EnableWindow( TRUE );
				m_btnClearNoUse.EnableWindow( TRUE );
				m_btnCutPoly.EnableWindow( TRUE );
				break;

		case	LEM_NAVMESH_POLYGON:
				m_btnEditDel.EnableWindow( TRUE );
				m_btnClearNoUse.EnableWindow( TRUE );
				m_btnInvertNormal.EnableWindow( TRUE );
				m_btnCutPoly.EnableWindow( TRUE );
				break;

		case	LEM_NAVMESH_POINT_ADD:
				m_btnEditAdd.EnableWindow( TRUE );				
				break;

		case	LEM_NAVMESH_POLYGON_CUT:
				m_btnCutPoly.EnableWindow( TRUE );
				break;
		}
	}

	// slider 
	{

		float fscale = 2.0f;

		if( i3Level::GetNavMesh() )
			fscale = i3Level::GetNavMesh()->GetVertexScale();

		{
			float fratio = (( fscale - m_fMinVScale) / (m_fMaxVScale - m_fMinVScale));
			int npos = static_cast<int>(fratio * 100.0f);
			
			m_sliderVScale.SetRange( 0, 100 );
			m_sliderVScale.SetTicFreq( 10 );
			m_sliderVScale.SetLineSize(1);
			m_sliderVScale.SetPos( npos );

			CString sz;
			sz.Format( "%0.2f", fscale );
			m_txtVScale.SetWindowText( sz );
		}		

		m_sliderVScale.EnableWindow( LEM_NORMAL != editMode );
	}

	// add vertex
	m_btnEditAdd.SetCheck( (editMode == LEM_NAVMESH_POINT_ADD) ? 1 : 0 );

	// cut poly
	m_btnCutPoly.SetCheck( (editMode == LEM_NAVMESH_POLYGON_CUT) ? 1 : 0 );
}


void	CPanelNavMeshEdit::OnBnClickedNavmeshRadioVertex()
{
	theApp.SetLevelEditMode( LEM_NAVMESH_POINT );
}

void	CPanelNavMeshEdit::OnBnClickedNavmeshRadioPoly()
{
	theApp.SetLevelEditMode( LEM_NAVMESH_POLYGON );
}

void	CPanelNavMeshEdit::OnBnClickedNavmeshEditAdd()
{
	if( 0 == m_btnEditAdd.GetCheck() )
	{
		theApp.SetLevelEditMode( LEM_NAVMESH_POINT );
	}
	else
	{
		theApp.SetLevelEditMode( LEM_NAVMESH_POINT_ADD );
	}
}

void	CPanelNavMeshEdit::OnBnClickedNavmeshEditDel()
{
	i3NavMesh* pNav = i3Level::GetNavMesh();
	i3LevelFramework* pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	if( NULL == pFramework || NULL == pNav )
	{
		return;
	}

	switch( pFramework->GetLevelEditMode() )
	{
	case	LEM_NAVMESH_POINT:
			pNav->RemoveSelectedVertex();
			break;

	case	LEM_NAVMESH_POLYGON:
			pNav->RemoveSelectedPolygon();
			break;
	}
}

void	CPanelNavMeshEdit::OnBnClickedNavmeshEditCreatePoly()
{
	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->CreatePolygon();
}

void 	CPanelNavMeshEdit::OnBnClickedNavmeshEditCutPoly()
{
	i3NavMesh* pNav = i3Level::GetNavMesh();
	i3LevelFramework* pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	if( NULL == pFramework || NULL == pNav )
	{
		return;
	}

	static LEVEL_EDIT_MODE _prev; 

	if( 0 == m_btnCutPoly.GetCheck() )
	{
		theApp.SetLevelEditMode( _prev );
	}
	else
	{
		_prev = pFramework->GetLevelEditMode();
		theApp.SetLevelEditMode( LEM_NAVMESH_POLYGON_CUT );
	}
}

void	CPanelNavMeshEdit::OnBnClickedNavmeshEditClearNoUse()
{
	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->ClearNoUseVertex();
}

void 	CPanelNavMeshEdit::OnBnClickedNavmeshEditInvertNormal()
{
	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->InvertSelectedPolyNormal();
}

void	CPanelNavMeshEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar)
	{
		if( pScrollBar == (CScrollBar*)&m_sliderVScale )
		{
			float fratio = static_cast<float>(m_sliderVScale.GetPos()) / 100.0f;
			float fscale = ((m_fMaxVScale - m_fMinVScale) * fratio) + m_fMinVScale;

			i3NavMesh* pNav = i3Level::GetNavMesh();

			if( pNav )
				pNav->SetVertexScale( fscale );

			if( m_txtVScale.GetSafeHwnd() )
			{
				CString sz;
				sz.Format( "%0.2f", fscale );
				m_txtVScale.SetWindowText( sz );
			}
		}
	}

	i3TDKDialogBase::OnHScroll(nSBCode, nPos, pScrollBar);
}
