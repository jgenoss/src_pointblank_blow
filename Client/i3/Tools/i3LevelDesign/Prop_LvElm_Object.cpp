// Prop_LvElm_Object.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvElm_Object.h"
#include ".\prop_lvelm_object.h"
#include "i3Base/string/ext/ftoa.h"
// Prop_LvElm_Object dialog

IMPLEMENT_DYNAMIC(Prop_LvElm_Object, i3TDKDialogBase)
Prop_LvElm_Object::Prop_LvElm_Object(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(Prop_LvElm_Object::IDD, pParent)
{
	m_pObj = NULL;
}

Prop_LvElm_Object::~Prop_LvElm_Object()
{
}

void Prop_LvElm_Object::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_OBJECTTYPE, m_cb_ObjType);
	DDX_Control(pDX, IDC_CB_OBJTEAM_TYPE, m_cb_TeamType);
	DDX_Control(pDX, IDC_CHK_INVISIBLE, m_btnInvisible);
	DDX_Control(pDX, IDC_EDIT_JUMPPOSX, m_ed_JumpPosX);
	DDX_Control(pDX, IDC_EDIT_JUMPPOSY, m_ed_JumpPosY);
	DDX_Control(pDX, IDC_EDIT_JUMPPOSZ, m_ed_JumpPosZ);
}


BEGIN_MESSAGE_MAP(Prop_LvElm_Object, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_CB_OBJECTTYPE, OnCbnSelchangeCbObjecttype)
	ON_CBN_SELCHANGE(IDC_CB_OBJTEAM_TYPE, OnCbnSelchangeCbObjteamType)
	ON_BN_CLICKED(IDC_CHK_INVISIBLE, OnBnClickedChkInvisible)
	ON_EN_KILLFOCUS(IDC_EDIT_JUMPPOSX, OnEnChangeEditJumpposx)
	ON_EN_KILLFOCUS(IDC_EDIT_JUMPPOSY, OnEnChangeEditJumpposy)
	ON_EN_KILLFOCUS(IDC_EDIT_JUMPPOSZ, OnEnChangeEditJumpposz)
END_MESSAGE_MAP()


// Prop_LvElm_Object message handlers
void	Prop_LvElm_Object::SetObject( i3ElementBase * pObj)
{
	m_pObj = (i3LevelObject *) pObj; 

	m_cb_ObjType.EnableWindow( TRUE);
	m_cb_TeamType.EnableWindow( TRUE);
	m_btnInvisible.EnableWindow( TRUE);

	_UpdateElementState();
}

void Prop_LvElm_Object::_UpdateElementState()
{
	if( m_pObj != NULL)
	{
		I3LV_OBJECT_TYPE Type = m_pObj->getObjType();

		m_cb_ObjType.EnableWindow( TRUE);
		m_cb_ObjType.SetCurSel( (UINT32)Type);
		if( Type == I3LV_OBJTYPE_NORMAL)
		{
			m_cb_TeamType.EnableWindow( FALSE);
			m_btnInvisible.EnableWindow( FALSE);
			m_ed_JumpPosX.EnableWindow( FALSE);
			m_ed_JumpPosY.EnableWindow( FALSE);
			m_ed_JumpPosZ.EnableWindow( FALSE);

			m_ed_JumpPosX.SetWindowText( "");
			m_ed_JumpPosY.SetWindowText( "");
			m_ed_JumpPosZ.SetWindowText( "");
		}
		else if( Type == I3LV_OBJTYPE_JUMPEROBJ)
		{
			char conv[ 256];

			m_cb_TeamType.EnableWindow( FALSE);
			m_btnInvisible.EnableWindow( TRUE);
			m_ed_JumpPosX.EnableWindow( TRUE);
			m_ed_JumpPosY.EnableWindow( TRUE);
			m_ed_JumpPosZ.EnableWindow( TRUE);

			VEC3D * pPos = m_pObj->getJumpPos();
			i3::ftoa( i3Vector::GetX( pPos), conv);
			m_ed_JumpPosX.SetWindowText( conv);
			i3::ftoa( i3Vector::GetY( pPos), conv);
			m_ed_JumpPosY.SetWindowText( conv);
			i3::ftoa( i3Vector::GetZ( pPos), conv);
			m_ed_JumpPosZ.SetWindowText( conv);

			m_btnInvisible.SetCheck( (m_pObj->isInvisible()) ? BST_CHECKED: BST_UNCHECKED);
		}
		else if( Type == I3LV_OBJTYPE_LADDER)
		{
			m_cb_TeamType.SelectString(0, "All");
			m_cb_TeamType.EnableWindow( FALSE);
			m_btnInvisible.EnableWindow( FALSE);
			m_ed_JumpPosX.EnableWindow( FALSE);
			m_ed_JumpPosY.EnableWindow( FALSE);
			m_ed_JumpPosZ.EnableWindow( FALSE);
		}
		else
		{
			m_cb_TeamType.EnableWindow( TRUE);
			m_btnInvisible.EnableWindow( TRUE);
			m_ed_JumpPosX.EnableWindow( FALSE);
			m_ed_JumpPosY.EnableWindow( FALSE);
			m_ed_JumpPosZ.EnableWindow( FALSE);

			m_ed_JumpPosX.SetWindowText( "");
			m_ed_JumpPosY.SetWindowText( "");
			m_ed_JumpPosZ.SetWindowText( "");

			I3LV_OBJECT_TEAM_TYPE	TeamType = m_pObj->getObjTeamType();

			m_cb_TeamType.SetCurSel( (UINT32)TeamType);
			m_btnInvisible.SetCheck( (m_pObj->isInvisible()) ? BST_CHECKED: BST_UNCHECKED);
		}
	}
	else
	{
		m_cb_ObjType.EnableWindow( FALSE);
		m_cb_TeamType.EnableWindow( FALSE);
		m_btnInvisible.EnableWindow( FALSE);
		m_ed_JumpPosX.EnableWindow( FALSE);
		m_ed_JumpPosY.EnableWindow( FALSE);
		m_ed_JumpPosZ.EnableWindow( FALSE);
	}
}

BOOL Prop_LvElm_Object::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void Prop_LvElm_Object::OnCbnSelchangeCbObjecttype()
{
	if( m_pObj == NULL)
		return;

	// TODO: Add your control notification handler code here
	I3LV_OBJECT_TYPE type		= m_pObj->getObjType();
	I3LV_OBJECT_TYPE nCurSel	= (I3LV_OBJECT_TYPE)m_cb_ObjType.GetCurSel();

	if( type != nCurSel)
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)m_pObj);
	
		m_pObj->setObjType( (I3LV_OBJECT_TYPE)nCurSel);

		_UpdateElementState();
	}
}

void Prop_LvElm_Object::OnCbnSelchangeCbObjteamType()
{
	if( m_pObj == NULL)
		return;

	// TODO: Add your control notification handler code here
	I3LV_OBJECT_TEAM_TYPE	teamType = m_pObj->getObjTeamType();
	I3LV_OBJECT_TEAM_TYPE	nCurSel = (I3LV_OBJECT_TEAM_TYPE)m_cb_TeamType.GetCurSel();

	if( teamType != nCurSel)
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)m_pObj);

		m_pObj->setObjTeamType( nCurSel);
	}
}

void Prop_LvElm_Object::OnBnClickedChkInvisible()
{
	// TODO: Add your control notification handler code here
	if( m_pObj == NULL)
		return;

	bool bCurInVisible = m_pObj->isInvisible();
	bool bInVisible = (bool)( m_btnInvisible.GetCheck() == BST_CHECKED);

	if( bCurInVisible != bInVisible)
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)m_pObj);

		m_pObj->setInvisible( m_btnInvisible.GetCheck() == BST_CHECKED);
	}
}

void Prop_LvElm_Object::OnEnChangeEditJumpposx()
{
	if( m_pObj == NULL)
		return;

	if( m_ed_JumpPosX.GetModify() == TRUE)
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)m_pObj);

		char conv[ 256];
		m_ed_JumpPosX.GetWindowText( conv, sizeof(conv) - 1);
		m_pObj->setJumpPosX( (REAL32) atof( conv));

		m_ed_JumpPosX.SetModify( FALSE);
	}
}

void Prop_LvElm_Object::OnEnChangeEditJumpposy()
{
	if( m_pObj == NULL)
		return;

	if( m_ed_JumpPosY.GetModify() == TRUE)
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)m_pObj);

		char conv[ 256];
		m_ed_JumpPosY.GetWindowText( conv, sizeof(conv) - 1);
		m_pObj->setJumpPosY( (REAL32) atof( conv));

		m_ed_JumpPosY.SetModify( FALSE);
	}
}

void Prop_LvElm_Object::OnEnChangeEditJumpposz()
{
	if( m_pObj == NULL)
		return;

	if( m_ed_JumpPosZ.GetModify() == TRUE)
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)m_pObj);

		char conv[ 256];
		m_ed_JumpPosZ.GetWindowText( conv, sizeof(conv) - 1);
		m_pObj->setJumpPosZ( (REAL32) atof( conv));

		m_ed_JumpPosZ.SetModify( FALSE);
	}
}

void Prop_LvElm_Object::OnOK()
{
	if( m_pObj == NULL)
		return;

	if( ( m_ed_JumpPosX.GetModify() == TRUE) || ( m_ed_JumpPosY.GetModify() == TRUE) || ( m_ed_JumpPosZ.GetModify() == TRUE))
	{
		i3LevelUndo::ADD_Edit( (i3LevelElement3D*)m_pObj);
	}

	char conv[ 256];
	if( m_ed_JumpPosX.GetModify() == TRUE)
	{
		m_ed_JumpPosX.GetWindowText( conv, sizeof(conv) - 1);
		m_pObj->setJumpPosX( (REAL32) atof( conv));
		m_ed_JumpPosX.SetModify( FALSE);
	}

	if( m_ed_JumpPosY.GetModify() == TRUE)
	{
		m_ed_JumpPosY.GetWindowText( conv, sizeof(conv) - 1);
		m_pObj->setJumpPosY( (REAL32) atof( conv));
		m_ed_JumpPosY.SetModify( FALSE);
	}

	if( m_ed_JumpPosZ.GetModify() == TRUE)
	{
		m_ed_JumpPosZ.GetWindowText( conv, sizeof(conv) - 1);
		m_pObj->setJumpPosZ( (REAL32) atof( conv));
		m_ed_JumpPosZ.SetModify( FALSE);
	}
}
