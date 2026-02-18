// ParticleSpecEmitter.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ParticleSpecEmitter.h"
#include ".\particlespecemitter.h"
#include "Util.h"
#include "GlobalVariable.h"
#include "i3EffectEditorDlg.h"

// CParticleSpecEmitter dialog

IMPLEMENT_DYNAMIC(CParticleSpecEmitter, i3TDKDialogBase)
CParticleSpecEmitter::CParticleSpecEmitter(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CParticleSpecEmitter::IDD, pParent)
	, m_Type(0)
{
	m_pInfo = NULL;
}

CParticleSpecEmitter::~CParticleSpecEmitter()
{
}

void CParticleSpecEmitter::SetParticle( i3ParticleInfo * pInfo)
{
	char conv[256];

	m_pInfo = pInfo;
	if( m_pInfo == NULL)
	{
		m_Type = 0;

		m_OffsetXCtrl.SetWindowText( "");
		m_OffsetYCtrl.SetWindowText( "");
		m_OffsetZCtrl.SetWindowText( "");
		m_DirXCtrl.SetWindowText( "");
		m_DirYCtrl.SetWindowText( "");
		m_DirZCtrl.SetWindowText( "");
		m_UpXCtrl.SetWindowText( "");
		m_UpYCtrl.SetWindowText( "");
		m_UpZCtrl.SetWindowText( "");
		m_AngleCtrl.SetWindowText( "");
		m_UnitCtrl.SetWindowText( "");

		MeshControlEnable(FALSE);
		m_MeshNameCtrl.SetWindowText("");
	}
	else
	{
		// Emitter Type
		switch( m_pInfo->GetEmitType())
		{
			case i3ParticleInfo::EMIT_CONE :			m_Type = 0;	break;
			case i3ParticleInfo::EMIT_SPHERE :			m_Type = 1;	break;
			case i3ParticleInfo::EMIT_DISK :			m_Type = 2;	break;
			case i3ParticleInfo::EMIT_RECT_PLANE :		m_Type = 3;	break;
			case i3ParticleInfo::EMIT_CIRCLE_PLANE :	m_Type = 4;	break;
			case i3ParticleInfo::EMIT_MESH :			m_Type = 5; break;
		}

		if(m_pInfo->GetMeshParticle() != NULL)
		{
			MeshControlEnable(TRUE);
			m_MeshNameCtrl.SetWindowText(m_pInfo->GetMeshParticle()->GetMeshPath());

			if(m_pInfo->GetMeshParticle()->IsRandomMesh())
			{
				m_RandomMeshCtrl.SetCheck(BST_CHECKED);
			}
			else
			{
				m_RandomMeshCtrl.SetCheck(BST_UNCHECKED);
			}
		}
		
		REAL2STR( i3Vector::GetX( pInfo->GetEmitPosition()), conv);
		m_OffsetXCtrl.SetWindowText( conv);

		REAL2STR( i3Vector::GetY( pInfo->GetEmitPosition()), conv);
		m_OffsetYCtrl.SetWindowText( conv);

		REAL2STR( i3Vector::GetZ( pInfo->GetEmitPosition()), conv); 
		m_OffsetZCtrl.SetWindowText( conv);

		REAL2STR( i3Vector::GetX( pInfo->GetEmitDirection()), conv); 
		m_DirXCtrl.SetWindowText( conv);

		REAL2STR( i3Vector::GetY( pInfo->GetEmitDirection()), conv); 
		m_DirYCtrl.SetWindowText( conv);

		REAL2STR( i3Vector::GetZ( pInfo->GetEmitDirection()), conv);
		m_DirZCtrl.SetWindowText( conv);

		REAL2STR( i3Vector::GetX( pInfo->GetUpVector()), conv);
		m_UpXCtrl.SetWindowText( conv);

		REAL2STR( i3Vector::GetY( pInfo->GetUpVector()), conv);
		m_UpYCtrl.SetWindowText( conv);

		REAL2STR(  i3Vector::GetZ( pInfo->GetUpVector()), conv);
		m_UpZCtrl.SetWindowText( conv);

		REAL2STR( m_pInfo->GetEmitAngle(), conv);
		m_AngleCtrl.SetWindowText( conv);

		sprintf( conv, "%d", m_pInfo->GetEmitCount());
		m_UnitCtrl.SetWindowText( conv);

		REAL2STR( m_pInfo->GetEmitInterval() * 1000.0f, conv);
		m_IntervalCtrl.SetWindowText( conv);
	}

	UpdateData( FALSE);
}

void CParticleSpecEmitter::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_CB_EMIT_TYPE, m_Type);
	DDX_Control(pDX, IDC_ED_EMIT_ANGLE, m_AngleCtrl);
	DDX_Control(pDX, IDC_SPIN_EMIT_ANGLE, m_AngleSpinCtrl);
	DDX_Control(pDX, IDC_ED_EMIT_ANGLE2, m_UnitCtrl);
	DDX_Control(pDX, IDC_SPIN_EMIT_ANGLE2, m_UnitSpinCtrl);
	DDX_Control(pDX, IDC_ED_EMIT_POSX, m_OffsetXCtrl);
	DDX_Control(pDX, IDC_ED_EMIT_POSY, m_OffsetYCtrl);
	DDX_Control(pDX, IDC_ED_EMIT_POSZ, m_OffsetZCtrl);
	DDX_Control(pDX, IDC_ED_EMIT_DIRX, m_DirXCtrl);
	DDX_Control(pDX, IDC_ED_EMIT_DIRY, m_DirYCtrl);
	DDX_Control(pDX, IDC_ED_EMIT_DIRZ, m_DirZCtrl);
	DDX_Control(pDX, IDC_ED_UP_DIRX, m_UpXCtrl);
	DDX_Control(pDX, IDC_ED_UP_DIRY, m_UpYCtrl);
	DDX_Control(pDX, IDC_ED_UP_DIRZ, m_UpZCtrl);
	DDX_Control(pDX, IDC_ED_EMIT_ANGLE3, m_IntervalCtrl);
	DDX_Control(pDX, IDC_BTN_MESH, m_MeshSelCtrl);
	DDX_Control(pDX, IDC_ED_EMIT_TARGET_MESH_PATH, m_MeshNameCtrl);
	DDX_Control(pDX, IDC_CHECK_RANDOM_MESH, m_RandomMeshCtrl);
}


BEGIN_MESSAGE_MAP(CParticleSpecEmitter, i3TDKDialogBase)

	ON_CBN_SELCHANGE(IDC_CB_EMIT_TYPE, OnCbnSelchangeCbEmitType)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EMIT_ANGLE,  &CParticleSpecEmitter::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EMIT_ANGLE2,  &CParticleSpecEmitter::OnDeltaPosSpin)

	ON_BN_CLICKED(IDC_CHECK_RANDOM_MESH, OnBnClickedMeshRandomCtrl)
	ON_BN_CLICKED(IDC_BTN_MESH, &CParticleSpecEmitter::OnBnClickedBtnMesh)

	ON_EN_KILLFOCUS(IDC_ED_EMIT_POSX, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_EMIT_POSY, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_EMIT_POSZ, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_EMIT_DIRX, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_EMIT_DIRY, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_EMIT_DIRZ, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_UP_DIRX, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_UP_DIRY, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_UP_DIRZ, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_EMIT_ANGLE, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_EMIT_ANGLE2, &CParticleSpecEmitter::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_EMIT_ANGLE3, &CParticleSpecEmitter::OnEnKillFocus)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


// CParticleSpecEmitter message handlers

BOOL CParticleSpecEmitter::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	InitSpinCtrl();

	SetParticle(NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CParticleSpecEmitter::InitSpinCtrl()
{
	m_AngleSpinCtrl.SetBuddy( &m_AngleCtrl);
	m_AngleSpinCtrl.SetRange(0, SPINCTRL_MAX_COUNT);

	m_UnitSpinCtrl.SetBuddy( &m_UnitCtrl);
	m_UnitSpinCtrl.SetRange(0, SPINCTRL_MAX_COUNT);
}

void CParticleSpecEmitter::OnCancel()
{
}

void CParticleSpecEmitter::OnOK()
{
	VEC3D vec;
	REAL32 x, y, z;
	char conv[256];

	if( m_pInfo == NULL)
		return;

	UpdateData( TRUE);

	{
		m_OffsetXCtrl.GetWindowText( conv, sizeof(conv) - 1);
		x = (REAL32) atof( conv);

		m_OffsetYCtrl.GetWindowText( conv, sizeof(conv) - 1);
		y = (REAL32) atof( conv);

		m_OffsetZCtrl.GetWindowText( conv, sizeof(conv) - 1);
		z = (REAL32) atof( conv);

		m_pInfo->SetEmitPosition( x, y, z);
	}

	{
		m_DirXCtrl.GetWindowText( conv, sizeof(conv) - 1);
		x = (REAL32) atof( conv);

		m_DirYCtrl.GetWindowText( conv, sizeof(conv) - 1);
		y = (REAL32) atof( conv);

		m_DirZCtrl.GetWindowText( conv, sizeof(conv) - 1);
		z = (REAL32) atof( conv);

		i3Vector::Set( &vec, x, y, z);
		m_pInfo->SetEmitDirection( &vec);
	}

	{
		m_UpXCtrl.GetWindowText( conv, sizeof(conv) - 1);
		x = (REAL32) atof( conv);

		m_UpYCtrl.GetWindowText( conv, sizeof(conv) - 1);
		y = (REAL32) atof( conv);

		m_UpZCtrl.GetWindowText( conv, sizeof(conv) - 1);
		z = (REAL32) atof( conv);

		i3Vector::Set( &vec, x, y, z);
		m_pInfo->SetUpVector( &vec);
	}

	m_AngleCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetEmitAngle( (REAL32) atof( conv));

	m_UnitCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetEmitCount( atoi( conv));

	{
		m_IntervalCtrl.GetWindowText( conv, sizeof(conv) - 1);
		m_pInfo->SetEmitInterval( (REAL32)atof(conv) * 0.001f);  // 초단위로 환산
	}

	g_bNeedToCreate = TRUE;
	g_pMainWnd->StopRender();
}


void CParticleSpecEmitter::OnCbnSelchangeCbEmitType()
{
	UpdateData( TRUE);

	if(m_pInfo != NULL)
	{
		MeshControlEnable(FALSE);

		switch( m_Type)
		{
		case 0 :	m_pInfo->SetEmitType( i3ParticleInfo::EMIT_CONE);	break;
		case 1 :	m_pInfo->SetEmitType( i3ParticleInfo::EMIT_SPHERE);	break;
		case 2 :	m_pInfo->SetEmitType( i3ParticleInfo::EMIT_DISK);	break;
		case 3 :	m_pInfo->SetEmitType( i3ParticleInfo::EMIT_RECT_PLANE);	break;
		case 4 :	m_pInfo->SetEmitType( i3ParticleInfo::EMIT_CIRCLE_PLANE);	break;
		case 5 :	m_pInfo->SetEmitType( i3ParticleInfo::EMIT_MESH);	
					{
						MeshControlEnable(TRUE);
					}
					break;
		}

		if(m_Type != i3ParticleInfo::EMIT_MESH)
		{
			m_RandomMeshCtrl.SetCheck(BST_UNCHECKED);
			m_MeshNameCtrl.SetWindowText("");
			m_pInfo->DestroyMeshParticle();
		}
	}
}

void CParticleSpecEmitter::OnBnClickedBtnMesh()
{
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Scene File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() == IDCANCEL)
		return;

	{
		i3SceneFile file;

		if( file.Load( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
		{
			DebugBox( m_hWnd, "Could not open %s i3Engine scene file.", LPCTSTR( Dlg.GetPathName()));
			return;
		}

		i3SceneGraphInfo * pInfo = file.getSceneGraphInfo();

		char szRelPath[ MAX_PATH];

		i3::make_relative_path( g_pOption->m_szWorkPath, LPCTSTR( Dlg.GetPathName()), szRelPath  );

		pInfo->SetName( szRelPath);

		if(m_pInfo != NULL)
		{
			m_pInfo->BuildMeshParticle(pInfo->getInstanceSg());
			m_pInfo->GetMeshParticle()->SetMeshPath(szRelPath);

			m_MeshNameCtrl.SetWindowText(szRelPath);
		}
	}
}

void CParticleSpecEmitter::OnEnKillFocus()
{
	OnOK();
}

void CParticleSpecEmitter::OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnOK();
}

void CParticleSpecEmitter::MeshControlEnable(BOOL isEnable)
{
	m_MeshSelCtrl.EnableWindow(isEnable);
	m_MeshNameCtrl.EnableWindow(isEnable);
	m_RandomMeshCtrl.EnableWindow(isEnable);
}

void CParticleSpecEmitter::OnBnClickedMeshRandomCtrl()
{
	UpdateData(TRUE);

	if(m_pInfo != NULL)
	{
		if(m_RandomMeshCtrl.GetCheck() == BST_CHECKED)
		{
			m_pInfo->GetMeshParticle()->SetRandomMesh(TRUE);
		}
		else
		{
			m_pInfo->GetMeshParticle()->SetRandomMesh(FALSE);
		}
	}
}