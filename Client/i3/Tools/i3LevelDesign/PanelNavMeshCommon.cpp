#include "pch.h"
#include "i3LevelDesign.h"
#include "PanelNavMeshCommon.h"

#include <NavMesh.h>
#include <algorithm>
#include <cctype>


IMPLEMENT_DYNAMIC(CPanelNavMeshCommon, i3TDKDialogBase)

CPanelNavMeshCommon::CPanelNavMeshCommon(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelNavMeshCommon::IDD, pParent)
{
	m_pBlankTex			= NULL;

	i3Color::Set( &m_crBlankTex, 0.3f, 0.3f, 0.3f, 1.0f );
}

CPanelNavMeshCommon::~CPanelNavMeshCommon()
{
	I3_SAFE_RELEASE( m_pBlankTex );
}

void	CPanelNavMeshCommon::DoDataExchange( CDataExchange* pDX )
{
	i3TDKDialogBase::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_NAVMESH_TEXT_FILEPATH,		m_filePath );
	DDX_Control( pDX, IDC_NAVMESH_CHK_BLANKTEXTURE,		m_chkBtnDisableTex );
	DDX_Control( pDX, IDC_NAVMESH_CHK_WIREFRAME,		m_chkBtnWireframe );
	DDX_Control( pDX, IDC_NAVMESH_BLANKTEXTURE_COLOR,	m_btnBlankTexColor );
	DDX_Control( pDX, IDC_NAVMESH_CHK_VISIBLE,			m_chkBtnVisible );
}


BEGIN_MESSAGE_MAP( CPanelNavMeshCommon, i3TDKDialogBase )
	ON_BN_CLICKED( IDC_NAVMESH_SAVE,	&CPanelNavMeshCommon::OnBnClickedNavmeshNmsave )
	ON_BN_CLICKED( IDC_NAVMESH_SAVE_AS,	&CPanelNavMeshCommon::OnBnClickedNavmeshNmsaveAs )

	ON_BN_CLICKED( IDC_NAVMESH_LOAD, &CPanelNavMeshCommon::OnBnClickedNavmeshNmload )
	ON_BN_CLICKED( IDC_NAVMESH_CHK_BLANKTEXTURE,	&CPanelNavMeshCommon::OnBnClickedNavmeshChkBlankTexture )
	ON_BN_CLICKED( IDC_NAVMESH_CHK_WIREFRAME,		&CPanelNavMeshCommon::OnBnClickedNavmeshChkWireframe )
	ON_BN_CLICKED( IDC_NAVMESH_CHK_VISIBLE,			&CPanelNavMeshCommon::OnBnClickedNavmeshChkVisible )
	ON_BN_CLICKED( IDC_NAVMESH_BLANKTEXTURE_COLOR,	&CPanelNavMeshCommon::OnBnClickedNavmeshBlankTextureColor )
	ON_BN_CLICKED( IDC_NAVMESH_RESET,				&CPanelNavMeshCommon::OnBnClickedNavmeshReset )
END_MESSAGE_MAP()

BOOL	CPanelNavMeshCommon::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		COLORREF cr;

		cr = RGB( 255 * m_crBlankTex.r, 255 * m_crBlankTex.g, 255 * m_crBlankTex.b );
		m_btnBlankTexColor.SetFaceColor( cr );
		m_btnBlankTexColor.EnableWindowsTheming( FALSE );
	}

	{
		int nCheck = 1;
		
		if( i3Level::GetNavMesh() )
			nCheck = i3Level::GetNavMesh()->IsVisible() ? 1: 0 ;

		m_chkBtnVisible.SetCheck( nCheck );
	}

	return TRUE;
}

void	CPanelNavMeshCommon::OnBnClickedNavmeshNmsave()
{
	if( m_path.empty() )
		OnBnClickedNavmeshNmsaveAs();
	else
	{
		if( "ase" == m_ext )
		{
			i3Level::GetNavMesh()->SaveASE( m_path );
		}
		else
		{
			i3Level::GetNavMesh()->SaveNM( m_path );
		}
	}
}

void 	CPanelNavMeshCommon::OnBnClickedNavmeshNmsaveAs()
{
	char szFilter[] = "NM 颇老(*.NM) |*.NM|ASE 颇老(*.ASE) |*.ASE| All Files(*.*)|*.*||";
	CFileDialog dlg( FALSE, "NM", NULL, OFN_OVERWRITEPROMPT, szFilter );

	if( IDOK != dlg.DoModal() )
	{
		return;
	}

	if( i3Level::GetNavMesh() )
	{
		bool bRes = false;

		std::string ext = dlg.GetFileExt();
		std::transform( ext.begin(), ext.end(), ext.begin(), std::tolower );

		if( "ase" == ext )
		{
			bRes = i3Level::GetNavMesh()->SaveASE( dlg.GetPathName().GetString() );
		}
		else
		{
			bRes = i3Level::GetNavMesh()->SaveNM( dlg.GetPathName().GetString() );
		}

		if( bRes )
		{
			m_filePath.SetWindowText( dlg.GetPathName() );
			m_path = dlg.GetPathName().GetString();
			m_ext	= ext;
		}
	}
}

void	CPanelNavMeshCommon::OnBnClickedNavmeshNmload()
{
	i3LevelFramework* pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	if( pFramework )
	{
		pFramework->PendingUpdate();
		pFramework->SaveMatrix();
	}

	char szFilter[] = "NM 颇老(*.NM) |*.NM|ASE 颇老(*.ASE) |*.ASE| All Files(*.*)|*.*||";
	CFileDialog dlg( TRUE, "NM", NULL, OFN_OVERWRITEPROMPT, szFilter );

	if( IDOK == dlg.DoModal() )
	{
		if( i3Level::GetNavMesh() )
		{


			bool bRes = false;

			std::string ext = dlg.GetFileExt();
			std::transform( ext.begin(), ext.end(), ext.begin(), std::tolower );

			if( "ase" == ext )
			{
				bRes = i3Level::GetNavMesh()->LoadASE( dlg.GetPathName().GetString() );
			}
			else
			{
				bRes = i3Level::GetNavMesh()->LoadNM( dlg.GetPathName().GetString() );
			}

			if( bRes )
				m_filePath.SetWindowText( dlg.GetPathName() );


		}
	}

	if( pFramework )
		pFramework->RestoreUpdate();
}

void	CPanelNavMeshCommon::OnBnClickedNavmeshChkBlankTexture()
{
	_UnSetBlankTexture();

	if( 0 != m_chkBtnDisableTex.GetCheck() )
	{
		_SetBlankTexture();
	}
}

void	CPanelNavMeshCommon::_SetBlankTexture()
{
	static bool bInit = true;

	if( bInit )
	{
		const int nLockLevel = 0;

		I3_SAFE_RELEASE( m_pBlankTex );

		m_pBlankTex = i3Texture::new_object();
		m_pBlankTex->Create( 16, 16, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0 );
		m_pBlankTex->Lock( nLockLevel );

		for ( int x = 0; x < static_cast<int>(m_pBlankTex->GetWidth()); ++x )
		{
			for ( int y = 0; y < static_cast<int>(m_pBlankTex->GetHeight()); ++y )
				m_pBlankTex->SetPixel( x, y, &m_crBlankTex );
		}

		m_pBlankTex->Unlock( nLockLevel );
		bInit = false;
	}

	i3Node* pNode = NULL;

	i3LevelScene* pScene = i3Level::GetScene();

	if( NULL == pScene )
	{
		return;
	}

	const int nCnt = pScene->GetElementCount();

	for( int i = 0; i < nCnt; ++i )
	{
		pNode = NULL;

		i3LevelElement3D* elm = pScene->GetElement( i );

		if(  i3::kind_of<i3LevelWorld*>(elm)) //->IsTypeOf( i3LevelWorld::static_meta() ) )
		{
			if( NULL != ((i3LevelWorld*) elm)->getWorld() )
			{
				pNode = ((i3LevelWorld*) elm)->getWorld()->getWorldRoot();
			}
		}
		else if(  i3::kind_of<i3LevelObject*>(elm)) //->IsTypeOf( i3LevelObject::static_meta() ) )
		{
			pNode = ((i3LevelObject*) elm)->GetRoot();
		}

		if( NULL == pNode )
		{
			continue;
		}

		i3::vector< i3RenderAttr* > attrList;
		i3Scene::FindAttrByExactType( pNode, i3TextureBindAttr::static_meta(), attrList );

		_BindTexture bt;

		for each( i3RenderAttr* att in attrList )
		{
			i3TextureBindAttr* pbind = static_cast< i3TextureBindAttr* >( att );		

			switch( pbind->GetBindType() )
			{
			case	I3G_TEXTURE_BIND_DIFFUSE:
			case	I3G_TEXTURE_BIND_DIFFUSE1:
			case	I3G_TEXTURE_BIND_DIFFUSE2:
			case	I3G_TEXTURE_BIND_DIFFUSE3:
					{
						bt.pbind	= pbind;
						bt.ptex		= pbind->GetTexture();

						if( bt.ptex )
							bt.ptex->AddRef();

						pbind->SetTexture( m_pBlankTex );

						m_BindTextureList.push_back( bt );
					}
					break;
			}
		}
	}
}

void	CPanelNavMeshCommon::_UnSetBlankTexture()
{
	for each( _BindTexture bt in m_BindTextureList )
	{
		bt.pbind->SetTexture( bt.ptex );

		if( bt.ptex )
			bt.ptex->Release();
	}

	m_BindTextureList.clear();
}

void	CPanelNavMeshCommon::OnBnClickedNavmeshBlankTextureColor()
{
	i3TDKColorPicker dlg;

	if( false == dlg.Execute( &m_crBlankTex ) )
	{
		return;
	}

	if( TRUE == i3Color::IsSame( &m_crBlankTex, dlg.getSelectedColor() ) )
	{
		return;
	}

	i3Color::Set( &m_crBlankTex, dlg.getSelectedColor() );
	
	COLORREF cr;
	
	cr = RGB( 255 * m_crBlankTex.r, 255 * m_crBlankTex.g, 255 * m_crBlankTex.b );
	m_btnBlankTexColor.SetFaceColor( cr );
	
	if( m_pBlankTex )
	{
		const int nLockLevel = 0;

		m_pBlankTex->Lock( nLockLevel );

		for ( int x = 0; x < static_cast<int>(m_pBlankTex->GetWidth()); ++x )
		{
			for ( int y = 0; y < static_cast<int>(m_pBlankTex->GetHeight()); ++y )
				m_pBlankTex->SetPixel( x, y, &m_crBlankTex );
		}

		m_pBlankTex->Unlock( nLockLevel );
	}
}

void 	CPanelNavMeshCommon::OnBnClickedNavmeshChkWireframe()
{
	if( i3Level::GetScene() )
	{
		if( 0 == m_chkBtnWireframe.GetCheck() )
		{
			i3Level::GetScene()->SetRenderMode( I3G_RENDER_POLYGON );
		}
		else
		{
			i3Level::GetScene()->SetRenderMode( I3G_RENDER_LINE );
		}
	}
}

void 	CPanelNavMeshCommon::OnBnClickedNavmeshChkVisible()
{
	if( i3Level::GetNavMesh() )
	{
		if( 0 == m_chkBtnVisible.GetCheck() )
		{
			i3Level::GetNavMesh()->SetVisible( false );
		}
		else
		{
			i3Level::GetNavMesh()->SetVisible( true );
		}
	}
}

void 	CPanelNavMeshCommon::OnBnClickedNavmeshReset()
{
	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->ClearData();
}

