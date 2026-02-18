// Prop_LvElm_Prim_Respawn.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvElm_Prim_Respawn.h"
/*
static char * s_TeamType[2] = 
{
	"Red Team", 
	"Blue Team"
};
*/
// CProp_LvElm_Prim_Respawn dialog

IMPLEMENT_DYNAMIC(CProp_LvElm_Prim_Respawn, i3TDKDialogBase)
CProp_LvElm_Prim_Respawn::CProp_LvElm_Prim_Respawn(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvElm_Prim_Respawn::IDD, pParent)
{
	m_pRespawn = NULL;
}

CProp_LvElm_Prim_Respawn::~CProp_LvElm_Prim_Respawn()
{
}

void CProp_LvElm_Prim_Respawn::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_TEAM, m_TeamCtrl);
}

void CProp_LvElm_Prim_Respawn::SetObject( i3ElementBase * pObj)
{
	m_pRespawn = (i3LevelRespawn *) pObj; 

	if( m_pRespawn != NULL)
	{
		UINT32 nTeamType = m_pRespawn->getRespawnStyle();

		m_TeamCtrl.SetCurSel( nTeamType);
	}
}

BEGIN_MESSAGE_MAP(CProp_LvElm_Prim_Respawn, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_CB_TEAM, OnCbnSelchangeCbTeam)
END_MESSAGE_MAP()


// CProp_LvElm_Prim_Respawn message handlers

void CProp_LvElm_Prim_Respawn::OnOK()
{
}

void CProp_LvElm_Prim_Respawn::OnCancel()
{
}

BOOL CProp_LvElm_Prim_Respawn::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	//m_TeamCtrl.AddString( (LPCTSTR)s_TeamType[0]);
	//m_TeamCtrl.AddString( (LPCTSTR)s_TeamType[1]);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvElm_Prim_Respawn::OnCbnSelchangeCbTeam()
{
	// TODO: Add your control notification handler code here
	INT32 nStyle = m_TeamCtrl.GetCurSel();
	INT32 nCurStyle = -1;

	i3::vector<i3LevelElement3D*> list;

	g_pScene->GetSelectedElements( list);

	i3LevelUndo::ADD_Edit( g_pScene);

	if( list.size() > 1)	i3LevelUndo::BeginBatch();
	
	for( size_t i = 0;i < list.size(); i++)
	{
		i3LevelElement3D * pElm = list[i];
		if( i3::kind_of<i3LevelRespawn*>(pElm)) //->IsTypeOf( i3LevelRespawn::static_meta()))
		{
			i3LevelRespawn * pRespawn = (i3LevelRespawn*)pElm;

			nCurStyle = pRespawn->getRespawnStyle();
			if( nCurStyle != nStyle)
			{
				i3LevelUndo::ADD_Edit( (i3LevelElement3D*)pRespawn);

				pRespawn->setRespawnStyle( nStyle);
			}
		}
	}

	if( list.size() > 1)	i3LevelUndo::EndBatch();
}
