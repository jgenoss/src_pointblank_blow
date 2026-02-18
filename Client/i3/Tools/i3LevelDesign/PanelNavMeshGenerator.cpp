#include "pch.h"
#include "i3LevelDesign.h"
#include "PanelNavMeshGenerator.h"

#include "DlgNavmeshGenerator.h"

#include "i3Level/i3NavMesh.h"


IMPLEMENT_DYNAMIC(CPanelNavMeshGenerator, i3TDKDialogBase)

CPanelNavMeshGenerator::CPanelNavMeshGenerator(CWnd* pParent /*=NULL*/)
: i3TDKDialogBase(CPanelNavMeshGenerator::IDD, pParent)
{
	for( int i = 0; i < MAX_CONFIG_DATA_TYPE; ++i )
	{
		m_data[ i  ] = NULL;
	}

	// init datas
	{
		m_cfg.setDefaultConfig();

		m_data[ CFG_CELL_SIZE ]			= &m_cfg.cellSize;
		m_min[ CFG_CELL_SIZE ]			= 0.1f;
		m_max[ CFG_CELL_SIZE ]			= 1.0f;
		m_tick[ CFG_CELL_SIZE ]			= 0.01f;
		m_bNumber[ CFG_CELL_SIZE ]		= false;
		
		m_data[ CFG_CELL_HEIGHT ]		= &m_cfg.cellHeight;
		m_min[ CFG_CELL_HEIGHT ]		= 0.1f;
		m_max[ CFG_CELL_HEIGHT ]		= 1.0f;
		m_tick[ CFG_CELL_HEIGHT ]		= 0.01f;
		m_bNumber[ CFG_CELL_HEIGHT ]	= false;

		m_data[ CFG_EDGE_MAXLENGTH ]	= &m_cfg.edgeMaxLen;
		m_min[ CFG_EDGE_MAXLENGTH ]		= 0.0f;
		m_max[ CFG_EDGE_MAXLENGTH ]		= 50.0f;
		m_tick[ CFG_EDGE_MAXLENGTH ]	= 1.0f;
		m_bNumber[ CFG_EDGE_MAXLENGTH ]	= true;

		m_data[ CFG_EDGE_MAXERROR ]		= &m_cfg.edgeMaxError;
		m_min[ CFG_EDGE_MAXERROR ]		= 0.1f;
		m_max[ CFG_EDGE_MAXERROR ]		= 3.0f;
		m_tick[ CFG_EDGE_MAXERROR ]		= 0.01f;
		m_bNumber[ CFG_EDGE_MAXERROR ]	= false;

		m_data[ CFG_REGION_MINSIZE ]	= &m_cfg.regionMinSize;
		m_min[ CFG_REGION_MINSIZE ]		= 0.0f;
		m_max[ CFG_REGION_MINSIZE ]		= 150.0f;
		m_tick[ CFG_REGION_MINSIZE ]	= 1.0f;
		m_bNumber[ CFG_REGION_MINSIZE ]	= true;

		m_data[ CFG_REGION_MERGESIZE ]	= &m_cfg.regionMergeSize;
		m_min[ CFG_REGION_MERGESIZE ]	= 0.0f;
		m_max[ CFG_REGION_MERGESIZE ]	= 150.0f;
		m_tick[ CFG_REGION_MERGESIZE ]	= 1.0f;
		m_bNumber[ CFG_REGION_MERGESIZE ]	= true;
	}
}

CPanelNavMeshGenerator::~CPanelNavMeshGenerator()
{
}

void	CPanelNavMeshGenerator::DoDataExchange( CDataExchange* pDX )
{
	i3TDKDialogBase::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_NAVMESH_SLIDE_CELLSIZE,			m_slider[ CFG_CELL_SIZE ] );
	DDX_Control( pDX, IDC_NAVMESH_SLIDE_CELLHEIGHT,			m_slider[ CFG_CELL_HEIGHT ] );
	DDX_Control( pDX, IDC_NAVMESH_SLIDE_EDGEMAXLENGTH,		m_slider[ CFG_EDGE_MAXLENGTH ] );
	DDX_Control( pDX, IDC_NAVMESH_SLIDE_EDGEMAXERROR,		m_slider[ CFG_EDGE_MAXERROR ] );
	DDX_Control( pDX, IDC_NAVMESH_SLIDE_REGIONMINSIZE,		m_slider[ CFG_REGION_MINSIZE ] );
	DDX_Control( pDX, IDC_NAVMESH_SLIDE_REGIONMERGESIZE,	m_slider[ CFG_REGION_MERGESIZE ] );

	DDX_Control( pDX, IDC_NAVMESH_TEXT_CELLSIZE,			m_text[ CFG_CELL_SIZE ] );
	DDX_Control( pDX, IDC_NAVMESH_TEXT_CELLHEIGHT,			m_text[ CFG_CELL_HEIGHT ] );
	DDX_Control( pDX, IDC_NAVMESH_TEXT_EDGEMAXLENGTH,		m_text[ CFG_EDGE_MAXLENGTH ] );
	DDX_Control( pDX, IDC_NAVMESH_TEXT_EDGEMAXERROR,		m_text[ CFG_EDGE_MAXERROR ] );
	DDX_Control( pDX, IDC_NAVMESH_TEXT_REGIONMINSIZE,		m_text[ CFG_REGION_MINSIZE ] );
	DDX_Control( pDX, IDC_NAVMESH_TEXT_REGIONMERGESIZE,		m_text[ CFG_REGION_MERGESIZE ] );

	DDX_Control( pDX, IDC_RECREATE_NAVMESH,			m_btnReCreate );
	DDX_Control( pDX, IDC_NAVMESH_RESET_CFG,		m_btnReset );
	DDX_Control( pDX, IDC_NAVMESH_GENERATE_TYPE,	m_comboType );

	DDX_Control( pDX, IDC_NAVMESH_CHK_MONOTONEPARTITION, m_btnMonotonePartition );
	
}


BEGIN_MESSAGE_MAP( CPanelNavMeshGenerator, i3TDKDialogBase )
	ON_BN_CLICKED( IDC_RECREATE_NAVMESH, &CPanelNavMeshGenerator::OnBnClickedReCreateNavmesh )
	ON_BN_CLICKED( IDC_NAVMESH_RESET_CFG, &CPanelNavMeshGenerator::OnBnClickedNavmeshResetCfg )
	
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL	CPanelNavMeshGenerator::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		m_comboType.AddString( "Collidee" );
		m_comboType.AddString( "Mesh" );		
	}

	InitCtrls();
	return TRUE;
}

void	CPanelNavMeshGenerator::InitSliderCtrl()
{
	m_cfg.setDefaultConfig();

	for( int i = 0; i < MAX_CONFIG_DATA_TYPE; ++i )
	{
		if( NULL == m_data[i] )
		{
			continue;
		}

		CSliderCtrl& slider = m_slider[i];

		const float& _data	= *m_data[i];
		const float& _min	= m_min[i];
		const float& _max	= m_max[i];
		const float& _tick	= m_tick[i];

		const int nrange	= (int) ceilf( (_max - _min) / _tick );
		const float ratio	= ((_data - _min) / (_max - _min));

		slider.SetRange( 0, nrange );
		slider.SetLineSize( 1 );
		slider.SetTicFreq( 10 );			
		slider.SetPos( static_cast<int>(nrange * ratio) );

		{
			CString str;

			if( m_bNumber[i] )
				str.Format( "%d", (int) _data );
			else
				str.Format( "%0.2f", _data );

			m_text[i].SetWindowText( str );
		}
	}
}

void	CPanelNavMeshGenerator::OnBnClickedReCreateNavmesh()
{
	i3LevelFramework* pFrameWork = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	if( NULL == pFrameWork )
	{
		return;
	}

	pFrameWork->PendingUpdate();
	pFrameWork->SaveMatrix();


	i3World* pWorld = NULL;
	i3LevelScene* pScene = i3Level::GetScene();

	if( NULL == pScene || NULL == i3Level::GetNavMesh() )
	{
		return;
	}

	const int nCnt = pScene->GetElementCount();

	for( int i = 0; i < nCnt; ++i )
	{
		i3LevelElement3D* elm = pScene->GetElement( i );

		if( i3::kind_of<i3LevelWorld*>(elm)) //->IsTypeOf( i3LevelWorld::static_meta() ) )
		{
			pWorld = ((i3LevelWorld*) elm)->getWorld();
			break;
		}
	}

	m_cfg.genType				= (NAVMESH_GENERATE_TYPE) m_comboType.GetCurSel();
	m_cfg.monotonePartitioning	= (0 != m_btnMonotonePartition.GetCheck());

	{
		i3NavMesh* pNav = i3Level::GetNavMesh();

		if( NULL == pNav )
		{
			return;
		}

		i3NavMeshDataMgr* dataMgr = i3NavMeshDataMgr::new_object();

		CDlgNavmeshGenerator dlg;
		if( IDOK == dlg.DoModal( dataMgr, pWorld, m_cfg ) )
		{
			//
			i3LevelUndo::ADD_NavMesh_GenerateMesh( pNav->GetDataMgr(), dataMgr );
			pNav->SetDataMgr( dataMgr );
		}

		I3_SAFE_RELEASE( dataMgr );
	}

	/*if( NULL != pWorld )
	{
		if( pScene->GetNavMeshRoot() )
			pScene->GetNavMeshRoot()->DoAutoGenration( pWorld, m_cfg );
	}*/

	pFrameWork->RestoreUpdate();
}

void	CPanelNavMeshGenerator::OnBnClickedNavmeshResetCfg()
{
	m_cfg.setDefaultConfig();

	InitCtrls();
}

void	CPanelNavMeshGenerator::InitCtrls()
{	
	InitSliderCtrl();
	m_comboType.SetCurSel( (int) m_cfg.genType );
	m_btnMonotonePartition.SetCheck( m_cfg.monotonePartitioning ? 1 : 0 );
}

void	CPanelNavMeshGenerator::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar )
	{
		int i = 0;
		for( ; i < MAX_CONFIG_DATA_TYPE; ++i )
		{
			if( pScrollBar == reinterpret_cast< CScrollBar* >(&m_slider[i]) )
			{
				break;
			}
		}

		if( MAX_CONFIG_DATA_TYPE != i )
		{
			if( NULL != m_data[i] )
			{
				*m_data[i] = (m_tick[i] * m_slider[i].GetPos()) + m_min[i];
				
				if( m_bNumber[i] )
				{
					CString str;
					str.Format( "%d", (int) *m_data[i] );
					m_text[i].SetWindowText( str );
				}
				else
				{
					CString str;
					str.Format( "%0.2f", *m_data[i] );
					m_text[i].SetWindowText( str );
				}
			}
		}
	}

	i3TDKDialogBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

void	CPanelNavMeshGenerator::LevelEditModeChanged( LEVEL_EDIT_MODE editMode )
{
	/*for( int i = 0; i < MAX_CONFIG_DATA_TYPE; ++i )
	{
		BOOL bEnable = TRUE;

		if( LEM_NORMAL == editMode )
		{
			bEnable = FALSE;
		}

		if( NULL == m_data[i] )
		{
			bEnable = false;
		}

		m_slider[i].EnableWindow( bEnable );
	}

	m_btnReCreate.EnableWindow( LEM_NORMAL != editMode );
	m_btnReset.EnableWindow( LEM_NORMAL != editMode );
	m_comboType.EnableWindow( LEM_NORMAL != editMode );*/
}
