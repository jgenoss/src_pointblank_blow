#include	"pch.h"
#include	"i3LevelDesign.h"
#include	"DlgNavmeshGenerator.h"

CDlgNavmeshGenerator* g_pDlg = NULL;

void	_updateLoadingRatio( float f, const char* sz )
{
	if( g_pDlg )
		g_pDlg->UpdateLoadingRatio( f, sz );
}

UINT	_generatorThread( LPVOID arg )
{
	if( g_pDlg )
		g_pDlg->DoAutoGenerate();

	Sleep( 500 );
	return 0;
}


IMPLEMENT_DYNAMIC( CDlgNavmeshGenerator, CDialog )

CDlgNavmeshGenerator::CDlgNavmeshGenerator( CWnd* pParent ) : CDialog(CDlgNavmeshGenerator::IDD, pParent)
{
	g_pDlg = this;

	m_dataMgr	= NULL;
	m_pWorld	= NULL;
	m_pTrhead	= NULL;

	::InitializeCriticalSection( &m_cs );	
}

CDlgNavmeshGenerator::~CDlgNavmeshGenerator()
{
	g_pDlg = NULL;

	::DeleteCriticalSection( &m_cs );

	I3_SAFE_RELEASE( m_dataMgr );
	I3_SAFE_RELEASE( m_pWorld );
}

void	CDlgNavmeshGenerator::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_NAVMESH_GENERATOR_PROGRESS,		m_progress );
	DDX_Control( pDX, IDC_NAVMESH_GENERATOR_PROGRESS_TEXT,	m_txt );
}


BEGIN_MESSAGE_MAP( CDlgNavmeshGenerator, CDialog )
	ON_WM_TIMER()
END_MESSAGE_MAP()

INT_PTR		CDlgNavmeshGenerator::DoModal( i3NavMeshDataMgr* dataMgr, i3World* world, const navMeshConfig& cfg )
{
	if( NULL == dataMgr || NULL == world )
	{
		return IDCANCEL;
	}

	m_cfg = cfg;

	I3_REF_CHANGE( m_dataMgr, dataMgr );
	I3_REF_CHANGE( m_pWorld, world );

	return CDialog::DoModal();
}

bool	CDlgNavmeshGenerator::DoAutoGenerate()
{
	if( NULL == m_dataMgr )
	{
		return false;
	}

	return m_dataMgr->DoAutoGeneration( m_pWorld, m_cfg, _updateLoadingRatio );
}

BOOL	CDlgNavmeshGenerator::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pTrhead = AfxBeginThread( _generatorThread, NULL );

	m_progress.SetRange( 0, 100 );
	m_progress.SetPos( 0 );

	SetTimer( 0, 10, NULL );
	return TRUE;
}

void	CDlgNavmeshGenerator::OnTimer(UINT_PTR nIDEvent)
{
	if( NULL != m_pTrhead )
	{
		if( WAIT_TIMEOUT != ::WaitForSingleObject( m_pTrhead->m_hThread, 0 ) )
		{
			CDialog::OnOK();
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void	CDlgNavmeshGenerator::UpdateLoadingRatio( float f, const char* sz )
{
	I3NAVMESH::AUTO_CS( m_cs );

	m_pos = static_cast< int >(100.0f * f);
	m_progress.SetPos( m_pos );
	m_txt.SetWindowText( sz );
}
