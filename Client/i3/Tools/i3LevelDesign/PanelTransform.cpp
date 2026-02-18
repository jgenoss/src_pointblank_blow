// PanelTransform.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "PanelTransform.h"
#include ".\paneltransform.h"
#include "i3Base/string/ext/ftoa.h"

// CPanelTransform dialog

IMPLEMENT_DYNAMIC(CPanelTransform, CDialog)
CPanelTransform::CPanelTransform(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelTransform::IDD, pParent)
{
}

CPanelTransform::~CPanelTransform()
{
}

void CPanelTransform::Reset(void)
{
	BOOL bEnablePos		= FALSE;
	BOOL bEnableRot		= FALSE;
	BOOL bEnableScale	= FALSE;

	i3Vector::Set( &m_Pos, 0.0f, 0.0f, 0.0f);
	i3Vector::Set( &m_Rot, 0.0f, 0.0f, 0.0f);
	i3Vector::Set( &m_Scale, 1.0f, 1.0f, 1.0f);

	m_SelList.clear();

	switch( theApp.GetLevelEditMode() )
	{
	case	LEM_NORMAL:
			{
				if( g_pScene != NULL)
				{
					g_pScene->GetSelectedElements( m_SelList );
					g_pScene->SaveMatrix();

					bEnableScale = bEnableRot = bEnablePos = (0 < m_SelList.size());					
				}
			}
			break;

	case	LEM_NAVMESH_POINT:
			{
				i3::vector< int > selList;
				
				if( i3Level::GetNavMesh() )
				{
					i3Level::GetNavMesh()->GetSelectedVertexList( selList );
					bEnablePos = (0 < selList.size() );
				}
			}
			break;

	case	LEM_NAVMESH_POLYGON:
			{
				i3::vector< int > selList;

				if( i3Level::GetNavMesh() )
				{
					i3Level::GetNavMesh()->GetSelectedVertexList( selList );
					bEnablePos = (0 < selList.size() );
				}
			}
			break;
	}

	{
		m_XCtrl.EnableWindow( bEnablePos );
		m_YCtrl.EnableWindow( bEnablePos );
		m_ZCtrl.EnableWindow( bEnablePos );
		
		m_RotXCtrl.EnableWindow( bEnableRot );
		m_RotYCtrl.EnableWindow( bEnableRot );
		m_RotZCtrl.EnableWindow( bEnableRot );

		m_ScaleXCtrl.EnableWindow( bEnableScale );
		m_ScaleYCtrl.EnableWindow( bEnableScale );
		m_ScaleZCtrl.EnableWindow( bEnableScale );
	}
}

void CPanelTransform::Update(void)
{
	VEC3D* pPos = &m_Pos;
	VEC3D* pRot = &m_Rot;
	VEC3D* pScale = &m_Scale;

	bool bSetText = false;

	switch( theApp.GetLevelEditMode() )
	{
	case	LEM_NORMAL:
			{
				if( 0 < m_SelList.size() )
				{
					i3LevelElement3D* pElm;

					if( m_SelList.size() == 1)
					{
						pElm = m_SelList[0];

						pPos = pElm->getPos();

						//pElm->GetRotateAngle( &m_Rot);
						i3Vector::Set( &m_Rot, 0.0f, 0.0f, 0.0f);
						pRot = &m_Rot;

						pElm->GetScale( &m_Scale);
						pScale = &m_Scale;

						if(i3::kind_of<i3LevelPrimitive_Capsule *>(pElm))
						{
							m_ScaleZCtrl.EnableWindow(FALSE);
						}
						else
						{
							m_ScaleZCtrl.EnableWindow(TRUE);
						}
					}
					else
					{
						for(size_t i = 0; i < m_SelList.size(); i++ )
						{
							pElm	= m_SelList[i];

							i3Vector::Add( &m_Pos, &m_Pos, pElm->getPos() );
							//i3Vector::Add( &m_Rot, &m_Rot, pElm->GetRotate() );
						}
						i3Vector::Scale( &m_Pos, &m_Pos, 1.f / (REAL32)m_SelList.size() );
						//i3Vector::Scale( &m_Rot, &m_Rot, 1.f / m_SelList.GetCount() );						
					}

					bSetText = true;
				}
			}
			break;

	case	LEM_NAVMESH_POINT:
			{
				if( i3Level::GetNavMesh() )
					bSetText = i3Level::GetNavMesh()->GetSelectedCenterPosition( true, m_Pos );
			}
			break;

	case	LEM_NAVMESH_POLYGON:
			{
				if( i3Level::GetNavMesh() )
					bSetText = i3Level::GetNavMesh()->GetSelectedCenterPosition( false, m_Pos );
			}
			break;
	}

	if( false == bSetText )
	{
		m_XCtrl.SetWindowText( "" );
		m_YCtrl.SetWindowText( "" );
		m_ZCtrl.SetWindowText( "" );

		m_RotXCtrl.SetWindowText( "" );
		m_RotYCtrl.SetWindowText( "" );
		m_RotZCtrl.SetWindowText( "" );

		m_ScaleXCtrl.SetWindowText( "" );
		m_ScaleYCtrl.SetWindowText( "" );
		m_ScaleZCtrl.SetWindowText( "" );
	}
	else
	{
		char conv[256];

		// Position
		i3::ftoa( i3Vector::GetX( pPos), conv, 2);
		m_XCtrl.SetWindowText( conv);

		i3::ftoa( i3Vector::GetY( pPos), conv, 2);
		m_YCtrl.SetWindowText( conv);

		i3::ftoa( i3Vector::GetZ( pPos), conv, 2);
		m_ZCtrl.SetWindowText( conv);

		if( LEM_NORMAL == theApp.GetLevelEditMode() )
		{
			// Rotate
			i3::ftoa( I3_RAD2DEG( i3Vector::GetX( pRot)), conv, 1);
			m_RotXCtrl.SetWindowText( conv);

			i3::ftoa( I3_RAD2DEG( i3Vector::GetY( pRot)), conv, 1);
			m_RotYCtrl.SetWindowText( conv);

			i3::ftoa( I3_RAD2DEG( i3Vector::GetZ( pRot)), conv, 1);
			m_RotZCtrl.SetWindowText( conv);

			// Scale
			i3::ftoa( i3Vector::GetX( pScale), conv, 2);
			m_ScaleXCtrl.SetWindowText( conv);

			i3::ftoa( i3Vector::GetY( pScale), conv, 2);
			m_ScaleYCtrl.SetWindowText( conv);

			i3::ftoa( i3Vector::GetZ( pScale), conv, 2);
			m_ScaleZCtrl.SetWindowText( conv);
		}
	}
}

void CPanelTransform::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_POS_X, m_XCtrl);
	DDX_Control(pDX, IDC_ED_POS_Y, m_YCtrl);
	DDX_Control(pDX, IDC_ED_POS_Z, m_ZCtrl);
	DDX_Control(pDX, IDC_ED_ROT_X, m_RotXCtrl);
	DDX_Control(pDX, IDC_ED_ROT_Y, m_RotYCtrl);
	DDX_Control(pDX, IDC_ED_ROT_Z, m_RotZCtrl);
	DDX_Control(pDX, IDC_ED_SCALE_X, m_ScaleXCtrl);
	DDX_Control(pDX, IDC_ED_SCALE_Y, m_ScaleYCtrl);
	DDX_Control(pDX, IDC_ED_SCALE_Z, m_ScaleZCtrl);
}


BEGIN_MESSAGE_MAP(CPanelTransform, CDialog)
END_MESSAGE_MAP()


// CPanelTransform message handlers

BOOL CPanelTransform::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_XCtrl.EnableWindow( FALSE);
		m_YCtrl.EnableWindow( FALSE);
		m_ZCtrl.EnableWindow( FALSE);

		m_RotXCtrl.EnableWindow( FALSE);
		m_RotYCtrl.EnableWindow( FALSE);
		m_RotZCtrl.EnableWindow( FALSE);

		m_ScaleXCtrl.EnableWindow( FALSE);
		m_ScaleYCtrl.EnableWindow( FALSE);
		m_ScaleZCtrl.EnableWindow( FALSE);
	}

	i3Level::RegisterUpdateNotify( m_hWnd, i3LevelElement3D::static_meta(), 
		I3_TDK_UPDATE_EDIT | I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_FREEZE | 
		I3_TDK_UPDATE_MOVE | I3_TDK_UPDATE_ROTATE | I3_TDK_UPDATE_SCALE | 
		I3_TDK_UPDATE_SCENE);

	i3TDK::RegisterUpdate( m_hWnd, i3LevelScene::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelTransform::OnOK()
{
	char conv[256];
	REAL32 x, y, z;
	i3LevelElement3D * pElm;
	BOOL bPos = FALSE, bRot = FALSE, bScale = FALSE; 
	
	if(	(GetFocus() == &m_XCtrl) || (GetFocus() == &m_YCtrl) || (GetFocus() == &m_ZCtrl))
		bPos = TRUE;

	if( (GetFocus() == &m_RotXCtrl) || (GetFocus() == &m_RotYCtrl) || (GetFocus() == &m_RotZCtrl))
		bRot = TRUE;

	if( (GetFocus() == &m_ScaleXCtrl) || (GetFocus() == &m_ScaleYCtrl) || (GetFocus() == &m_ScaleZCtrl))
		bScale = TRUE;

	// Position
	if( bPos)
	{
		m_XCtrl.GetWindowText( conv, sizeof(conv) - 1);
		x = (REAL32) atof( conv);

		m_YCtrl.GetWindowText( conv, sizeof(conv) - 1);
		y = (REAL32) atof( conv);
			
		m_ZCtrl.GetWindowText( conv, sizeof(conv) - 1);
		z = (REAL32) atof( conv);

		switch( theApp.GetLevelEditMode()  )
		{
		case	LEM_NORMAL:
				{
					if( m_SelList.empty() )
					{
						return;
					}

					if( m_SelList.size() == 1)
					{
						pElm = m_SelList[0];

						pElm->setPos( x, y, z);
						i3Level::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_UPDATE_MOVE, pElm);
					}
					else
					{
						g_pScene->Moving( I3LV_SPACE::WORLD, x, y, z, false);
					}
				}
				break;

		case	LEM_NAVMESH_POINT:
		case	LEM_NAVMESH_POLYGON:
				{
					VEC3D vMove;
					
					i3Vector::Set( &vMove, x, y, z );
					i3Vector::Sub( &vMove, &vMove, &m_Pos );

					if( i3Level::GetNavMesh() )
					{
						i3Level::GetNavMesh()->MoveSelectedVertex( vMove );
					}
				}
				break;
		}
	}

	// Rotation
	if( bRot)
	{
		m_RotXCtrl.GetWindowText( conv, sizeof(conv) - 1);
		x = (REAL32) atof( conv);

		m_RotYCtrl.GetWindowText( conv, sizeof(conv) - 1);
		y = (REAL32) atof( conv);

		m_RotZCtrl.GetWindowText( conv, sizeof(conv) - 1);
		z = (REAL32) atof( conv);

		x = I3_DEG2RAD( x);
		y = I3_DEG2RAD( y);
		z = I3_DEG2RAD( z);

		if( m_SelList.size() == 1)
		{
			i3LevelElement3D * pElm2;

			pElm2 = m_SelList[0];

			pElm2->SetRotate( x, y, z);

			i3Level::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_UPDATE_ROTATE, pElm2);
		}
		else
		{
			g_pScene->Rotating( I3LV_SPACE::WORLD, x, y, z, false);
		}
	}

	// Scaling
	if( bScale)
	{
		m_ScaleXCtrl.GetWindowText( conv, sizeof(conv) - 1);
		x = (REAL32) atof( conv);

		m_ScaleYCtrl.GetWindowText( conv, sizeof(conv) - 1);
		y = (REAL32) atof( conv);

		m_ScaleZCtrl.GetWindowText( conv, sizeof(conv) - 1);
		z = (REAL32) atof( conv);

		if( m_SelList.size() == 1)
		{
			i3LevelElement3D * pElm2;

			pElm2 = m_SelList[0];
			pElm2->SetScale( x, y, z);

			i3Level::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_UPDATE_SCALE, pElm2);
		}
		else
		{
			g_pScene->Scaling( I3LV_SPACE::WORLD, x, y, z, false);
		}
	}

	if( LEM_NORMAL == theApp.GetLevelEditMode() )
	{
		i3LevelUndo::ADD_Transform( g_pScene );
	}

}

void CPanelTransform::OnCancel()
{
}

void CPanelTransform::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{	
	if( i3::kind_of< i3LevelElement3D *>( pInfo->m_pMeta))
	{
		switch( pInfo->m_Event)
		{
			case I3_TDK_UPDATE_SELECT :
			case I3_TDK_UPDATE_FREEZE :
			case I3_TDK_UPDATE_SCENE :
				Reset();
				// break;		의도적으로 없앤 것.

			case I3_TDK_UPDATE_EDIT :
			case I3_TDK_UPDATE_MOVE :
			case I3_TDK_UPDATE_ROTATE :
			case I3_TDK_UPDATE_SCALE :
				Update();
				break;
		}
	}
	else if( i3::kind_of< i3LevelScene *>( pInfo->m_pMeta))
	{
		switch( pInfo->m_Event)
		{
			case I3_TDK_UPDATE_REMOVE :
				Update();
				break;
		}
	}
}

LRESULT CPanelTransform::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE) 
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
