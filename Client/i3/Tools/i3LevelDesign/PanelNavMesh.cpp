#include "pch.h"
#include "i3LevelDesign.h"
#include "PanelNavMesh.h"

IMPLEMENT_DYNAMIC(CPanelNavMesh, i3TDKDialogBase)

CPanelNavMesh::CPanelNavMesh( CWnd* pParent )
	: i3TDKDialogBase(CPanelNavMesh::IDD, pParent)
{

}

CPanelNavMesh::~CPanelNavMesh()
{
}

void	CPanelNavMesh::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPanelNavMesh, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()

LRESULT		CPanelNavMesh::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( WM_TDK_UPDATE == message ) 
	{

	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

BOOL	CPanelNavMesh::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rc;

		GetClientRect( &rc );

		rc.left = 3;
		rc.top = 3;
		rc.right -= 3;
		rc.bottom -= 3;
		m_foldCtrl.Create( "Navigation Mesh", WS_VISIBLE | WS_CHILD, rc, this, 100 );

		m_common.Create( CPanelNavMeshCommon::IDD, &m_foldCtrl );
		m_foldCtrl.AddPane( &m_common, "Common");

		m_edit.Create( CPanelNavMeshEdit::IDD, &m_foldCtrl );
		m_foldCtrl.AddPane( &m_edit, "Edit");

		m_generator.Create( CPanelNavMeshGenerator::IDD, &m_foldCtrl );
		m_foldCtrl.AddPane( &m_generator, "Generator" );
	}
	

	return TRUE;
}

void	CPanelNavMesh::OnSize( UINT nType, int cx, int cy )
{
	i3TDKDialogBase::OnSize( nType, cx, cy );

	if( ::IsWindow( m_foldCtrl.GetSafeHwnd() ) )
		m_foldCtrl.SetWindowPos( NULL, 3,3, cx -6, cy -6, SWP_NOZORDER | SWP_NOACTIVATE );
}
